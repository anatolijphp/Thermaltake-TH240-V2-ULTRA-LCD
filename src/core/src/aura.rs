//! Aura Sync: the state the HAL plugin (TTLcdAura.dll inside ASUS LightingService) or armory-crated (Linux) publishes,
//! and the MilkDrop-style visualiser that turns the effect + LED colours into frames.
//! Block layout (little-endian): 0 magic 'TTAS' | 4 seq | 8 effectId | 12 ledCount | 16 speed | 20 direction |
//! 24 syncMs (i64) | 32 updated (.NET UTC ticks, i64) | 40 colors[64] (0x00BBGGRR).
use crate::render::{self, rgb};
use tiny_skia::{FilterQuality, Pixmap, PixmapPaint, Transform};

pub const SIZE: usize = 40 + 64 * 4;
pub const MAGIC: u32 = 0x5341_5454; // "TTAS"

#[derive(Clone, Debug)]
pub struct Snapshot {
    pub seq: i32,
    pub effect: i32,
    pub led_count: i32,
    pub speed: i32,
    pub direction: i32,
    pub sync_ms: i64,
    pub updated_ticks: i64,
    pub colors: [u32; 64],
}

fn i32_at(b: &[u8], o: usize) -> i32 {
    i32::from_le_bytes(b[o..o + 4].try_into().unwrap())
}
fn i64_at(b: &[u8], o: usize) -> i64 {
    i64::from_le_bytes(b[o..o + 8].try_into().unwrap())
}

impl Snapshot {
    pub fn parse(b: &[u8]) -> Option<Snapshot> {
        if b.len() < SIZE || i32_at(b, 0) as u32 != MAGIC {
            return None;
        }
        let mut colors = [0u32; 64];
        for (i, c) in colors.iter_mut().enumerate() {
            *c = i32_at(b, 40 + i * 4) as u32;
        }
        Some(Snapshot {
            seq: i32_at(b, 4),
            effect: i32_at(b, 8),
            led_count: i32_at(b, 12),
            speed: i32_at(b, 16),
            direction: i32_at(b, 20),
            sync_ms: i64_at(b, 24),
            updated_ticks: i64_at(b, 32),
            colors,
        })
    }

    /// Seconds since the last update (.NET ticks = 100 ns since 0001-01-01 UTC).
    pub fn age_secs(&self) -> f64 {
        if self.updated_ticks <= 0 {
            return f64::INFINITY;
        }
        (crate::util::dotnet_ticks_now() - self.updated_ticks) as f64 / 1e7
    }
}

// ---------------- reader ----------------

#[cfg(target_os = "linux")]
pub fn linux_path() -> Option<std::path::PathBuf> {
    let run = std::env::var_os("XDG_RUNTIME_DIR").filter(|v| !v.is_empty())?;
    Some(std::path::PathBuf::from(run).join("armory-crate/virtual/ttlcd.ttas"))
}

/// Linux: is armory-crated (the Aura Sync service) installed? Its user unit, or a published TTAS file, proves it.
#[cfg(target_os = "linux")]
pub fn linux_service_installed() -> bool {
    if linux_path().is_some_and(|p| p.exists()) {
        return true;
    }
    let cfg = std::env::var_os("XDG_CONFIG_HOME")
        .filter(|v| !v.is_empty())
        .map(std::path::PathBuf::from)
        .unwrap_or_else(|| std::path::PathBuf::from(std::env::var_os("HOME").unwrap_or_default()).join(".config"));
    let dirs = [cfg.join("systemd/user"), "/etc/systemd/user".into(), "/usr/local/lib/systemd/user".into(), "/usr/lib/systemd/user".into(), "/lib/systemd/user".into()];
    dirs.iter().any(|d| d.join("armory-crated.service").exists())
}

#[cfg(not(target_os = "linux"))]
pub fn linux_service_installed() -> bool {
    false
}

/// The current Aura state, or None while nothing publishes it.
pub struct Reader {
    #[cfg(windows)]
    view: Option<crate::win::SharedView>,
    #[cfg(windows)]
    next_try: std::time::Instant,
}

impl Default for Reader {
    fn default() -> Self {
        Reader {
            #[cfg(windows)]
            view: None,
            #[cfg(windows)]
            next_try: std::time::Instant::now(),
        }
    }
}

impl Reader {
    #[cfg(target_os = "linux")]
    pub fn read(&mut self) -> Option<Snapshot> {
        // armory-crated's writer uses a seqlock (odd seq = mid-write). Each read opens the file fresh (no buffer that
        // could serve stale data), twice, and both copies must agree on an even seq.
        let path = linux_path()?;
        for _ in 0..3 {
            let a = std::fs::read(&path).ok()?;
            let b = std::fs::read(&path).ok()?;
            if a.len() < SIZE || b.len() < SIZE {
                return None;
            }
            let seq = i32_at(&a, 4);
            if seq & 1 != 0 || seq != i32_at(&b, 4) {
                continue;
            }
            return Snapshot::parse(&a); // magic 0 = closed
        }
        None
    }

    #[cfg(windows)]
    pub fn read(&mut self) -> Option<Snapshot> {
        if self.view.is_none() {
            if std::time::Instant::now() < self.next_try {
                return None;
            }
            self.next_try = std::time::Instant::now() + std::time::Duration::from_secs(2);
            self.view = ["Global\\TTLcdAuraSync", "Local\\TTLcdAuraSync"].iter().find_map(|n| crate::win::SharedView::open(n, SIZE, false));
        }
        let v = self.view.as_ref()?;
        for _ in 0..3 {
            let seq = v.read_i32(4);
            let b = v.bytes();
            if v.read_i32(4) == seq {
                return Snapshot::parse(&b);
            }
        }
        None
    }

    #[cfg(not(any(windows, target_os = "linux")))]
    pub fn read(&mut self) -> Option<Snapshot> {
        None
    }
}

// ---------------- visualiser ----------------

const N: usize = 200;

/// MilkDrop-style visualiser. Every frame the previous image is warped (zoom, swirl, wavy displacement) and faded,
/// then a fresh waveform is drawn on top in the Aura colours; the feedback smears each waveform into flowing abstract
/// trails. Motion parameters drift on unrelated periods so the picture keeps morphing, and the waveform shape changes
/// every few seconds. Rendered at low resolution and upscaled (soft glow).
pub struct Visualiser {
    fb: Vec<f32>,
    fb_next: Vec<f32>,
    row_warp: [f32; N],
    col_warp: [f32; N],
    tone: Vec<u8>,
    last_t: f64,
    sync_base: i64,
    sync_local: f64,
    clock: std::time::Instant,
}

impl Default for Visualiser {
    fn default() -> Self {
        Visualiser {
            fb: vec![0.0; N * N * 3],
            fb_next: vec![0.0; N * N * 3],
            row_warp: [0.0; N],
            col_warp: [0.0; N],
            tone: (0..4096).map(|i| (255.0 * (1.0 - (-(i as f64) / 160.0).exp())) as u8).collect(), // soft clip
            last_t: f64::NAN,
            sync_base: -1,
            sync_local: 0.0,
            clock: std::time::Instant::now(),
        }
    }
}

type Rgb = (f64, f64, f64);

fn hsv(h: f64, v: f64) -> Rgb {
    let h = (h % 1.0 + 1.0) % 1.0 * 6.0;
    let i = h as i32;
    let f = h - i as f64;
    let (q, t) = (1.0 - f, f);
    let (r, g, b) = match i {
        0 => (1.0, t, 0.0),
        1 => (q, 1.0, 0.0),
        2 => (0.0, 1.0, t),
        3 => (0.0, q, 1.0),
        4 => (t, 0.0, 1.0),
        _ => (1.0, 0.0, q),
    };
    ((r * v * 255.0).floor(), (g * v * 255.0).floor(), (b * v * 255.0).floor())
}

fn from_aura(c: u32) -> Rgb {
    ((c & 0xFF) as f64, (c >> 8 & 0xFF) as f64, (c >> 16 & 0xFF) as f64)
}

fn scale(c: Rgb, k: f64) -> Rgb {
    let k = k.clamp(0.0, 1.0);
    ((c.0 * k).floor(), (c.1 * k).floor(), (c.2 * k).floor())
}

impl Visualiser {
    /// Aura time in s: follows Synchronize() so several devices animate in phase.
    fn now(&mut self, s: &Snapshot) -> f64 {
        let local = self.clock.elapsed().as_secs_f64() * 1000.0;
        if s.sync_ms != self.sync_base {
            self.sync_base = s.sync_ms;
            self.sync_local = local;
        }
        (if self.sync_base > 0 { self.sync_base as f64 + (local - self.sync_local) } else { local }) / 1000.0
    }

    /// The frame and a status line ("Aura Sync · Breathing").
    pub fn render(&mut self, s: Option<&Snapshot>) -> (Pixmap, String) {
        let Some(s) = s else {
            let linux = cfg!(target_os = "linux");
            let mut p = render::blank(0);
            let msg = if linux { "Waiting for\narmory-crated / Aura Sync" } else { "Waiting for\nArmoury Crate / Aura Sync" };
            render::text_block(&mut p, msg, "Segoe UI", false, 24.0, rgb(0xFFFFFF), tiny_skia::Rect::from_xywh(0.0, 0.0, 480.0, 480.0).unwrap());
            let st = if linux { "Waiting for armory-crated (Aura Sync service not running)" } else { "Waiting for Armoury Crate (Aura plugin not active)" };
            return (p, st.into());
        };
        let n = s.led_count.clamp(1, 64) as usize;
        let mut leds: Vec<Rgb> = s.colors[..n].iter().map(|c| from_aura(*c)).collect();
        let t = self.now(s);
        let period = match s.speed {
            i if i <= 0 => 4.0,
            1 => 3.0,
            2 => 2.0,
            _ => 1.2,
        }; // speed level -> seconds per cycle
        let name = match s.effect {
            2 => {
                let k = 0.5 - 0.5 * (2.0 * std::f64::consts::PI * t / period).cos();
                leds.iter_mut().for_each(|c| *c = scale(*c, k));
                "Breathing"
            }
            3 => {
                let on = (t / period * 4.0) % 1.0 < 0.5;
                if !on {
                    leds.iter_mut().for_each(|c| *c = (0.0, 0.0, 0.0));
                }
                "Strobing"
            }
            4 => {
                let cc = hsv(t / (period * 3.0), 1.0);
                leds.iter_mut().for_each(|c| *c = cc);
                "Color cycle"
            }
            5 | 0xFF => {
                for (i, c) in leds.iter_mut().enumerate() {
                    *c = hsv(t / (period * 2.0) + i as f64 / n as f64, 1.0);
                }
                if s.effect == 5 { "Rainbow" } else { "Default" }
            }
            1 => "Static",
            _ => "Manual",
        };
        let p = self.paint(&leds, t);
        let stale = s.age_secs() > 30.0 && s.effect <= 1;
        (p, format!("Aura Sync · {}{}", name, if stale { " (no recent updates)" } else { "" }))
    }

    fn paint(&mut self, leds: &[Rgb], t: f64) -> Pixmap {
        let dt = if self.last_t.is_nan() || t < self.last_t { 1.0 / 30.0 } else { (t - self.last_t).min(0.2) };
        self.last_t = t;
        let step = dt * 30.0; // per-frame amounts below are tuned for 30 fps
        self.warp(t, step);
        self.wave(leds, t);
        let mut low = Pixmap::new(N as u32, N as u32).unwrap();
        for (i, px) in low.pixels_mut().iter_mut().enumerate() {
            let o = i * 3;
            let c = |v: f32| self.tone[(v as usize).min(4095)];
            *px = tiny_skia::ColorU8::from_rgba(c(self.fb[o]), c(self.fb[o + 1]), c(self.fb[o + 2]), 255).premultiply();
        }
        let mut out = render::blank(0);
        let k = (480.0 + 8.0) / N as f32;
        let pp = PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() };
        out.draw_pixmap(0, 0, low.as_ref(), &pp, Transform::from_row(k, 0.0, 0.0, k, -4.0, -4.0), None);
        out
    }

    // fb_next = previous frame sampled through a zoom/swirl/wave field, faded; then swapped into fb.
    fn warp(&mut self, t: f64, step: f64) {
        let zoom = 1.0 + (0.018 + 0.014 * (t * 0.21).sin()) * step; // >1 flows outward
        let zoom_exp = 0.35 * (t * 0.17 + 1.0).sin(); // centre vs edge speed
        let rot = (0.006 + 0.012 * (t * 0.11).sin()) * step;
        let swirl = 0.03 * (t * 0.07 + 2.0).sin() * step; // extra twist near the centre
        let wa = (0.006 + 0.005 * (t * 0.29).sin()) * step;
        let wf = 5.0 + 3.0 * (t * 0.05).sin();
        for k in 0..N {
            let c = (k as f64 + 0.5) / N as f64 * 2.0 - 1.0;
            self.row_warp[k] = (wa * (c * wf + t * 1.3).sin()) as f32;
            self.col_warp[k] = (wa * (c * wf - t * 1.1).cos()) as f32;
        }
        // slow, slightly different decay per channel tints the trails as they fade
        let d = 0.935f64.powf(step);
        let dr = (d * (1.0 + 0.012 * (t * 0.23).sin())) as f32;
        let dg = (d * (1.0 + 0.012 * (t * 0.31 + 2.0).sin())) as f32;
        let db = (d * (1.0 + 0.012 * (t * 0.19 + 4.0).sin())) as f32;
        let (src, dst) = (&self.fb, &mut self.fb_next);
        for y in 0..N {
            let v = (y as f64 + 0.5) / N as f64 * 2.0 - 1.0;
            for x in 0..N {
                let u = (x as f64 + 0.5) / N as f64 * 2.0 - 1.0;
                let r = (u * u + v * v).sqrt();
                let a = rot + swirl * (1.0 - r).max(0.0);
                let (sa, ca) = a.sin_cos();
                let z = zoom * (1.0 + (zoom - 1.0) * zoom_exp * (r - 0.5) * 8.0);
                let su = (u * ca - v * sa) / z + self.row_warp[y] as f64;
                let sv = (u * sa + v * ca) / z + self.col_warp[x] as f64;
                let fx = (su + 1.0) * 0.5 * N as f64 - 0.5;
                let fy = (sv + 1.0) * 0.5 * N as f64 - 0.5;
                let (x0, y0) = (fx.floor() as i64, fy.floor() as i64);
                let o = (y * N + x) * 3;
                if x0 < 0 || y0 < 0 || x0 >= N as i64 - 1 || y0 >= N as i64 - 1 {
                    dst[o] = 0.0;
                    dst[o + 1] = 0.0;
                    dst[o + 2] = 0.0;
                    continue;
                }
                let (ax, ay) = ((fx - x0 as f64) as f32, (fy - y0 as f64) as f32);
                let (w00, w10, w01, w11) = ((1.0 - ax) * (1.0 - ay), ax * (1.0 - ay), (1.0 - ax) * ay, ax * ay);
                let i00 = (y0 as usize * N + x0 as usize) * 3;
                let (i10, i01) = (i00 + 3, i00 + N * 3);
                let i11 = i01 + 3;
                for (ch, dch) in [(0, dr), (1, dg), (2, db)] {
                    dst[o + ch] = (src[i00 + ch] * w00 + src[i10 + ch] * w10 + src[i01 + ch] * w01 + src[i11 + ch] * w11) * dch;
                }
            }
        }
        std::mem::swap(&mut self.fb, &mut self.fb_next);
    }

    // The "waveform": a pseudo-audio signal (sum of drifting sines) drawn as one of several shapes, coloured along
    // its length by the Aura LEDs.
    fn wave(&mut self, leds: &[Rgb], t: f64) {
        use std::f64::consts::PI;
        const K: usize = 900;
        let shape = (t / 14.0) as i64 % 4;
        let energy = 0.55 + 0.45 * (t * 1.7).sin().powi(2);
        let spin = t * 0.35;
        for k in 0..K {
            let s = k as f64 / K as f64;
            let th = s * 2.0 * PI;
            let wave = 0.55 * (3.0 * th + t * 2.1).sin() + 0.3 * (7.0 * th - t * 3.3).sin() + 0.15 * (13.0 * th + t * 5.2).sin();
            let (x, y) = match shape {
                0 => {
                    // pulsing ring
                    let r = 0.32 + 0.14 * energy * wave;
                    (r * (th + spin).cos(), r * (th + spin).sin())
                }
                1 => {
                    // Lissajous knot
                    let fa = 3.0 + (t * 0.05).sin();
                    let fbq = 2.0 + (t * 0.04).cos();
                    (0.5 * (fa * th + t * 0.7).sin() + 0.04 * wave, 0.5 * (fbq * th + t * 0.5).sin())
                }
                2 => {
                    // spinning oscilloscope line
                    let along = (s * 2.0 - 1.0) * 0.8;
                    let off = 0.18 * energy * wave;
                    let (sa, ca) = spin.sin_cos();
                    (along * ca - off * sa, along * sa + off * ca)
                }
                _ => {
                    // three spiral arms
                    let arm = k % 3;
                    let p = (k / 3) as f64 / (K / 3) as f64;
                    let a = p * 5.0 + spin * 2.0 + arm as f64 * 2.0 * PI / 3.0;
                    let r = p * 0.7 * (0.85 + 0.15 * wave);
                    (r * a.cos(), r * a.sin())
                }
            };
            let c = led_at(leds, s);
            self.splat((x + 1.0) * 0.5 * N as f64 - 0.5, (y + 1.0) * 0.5 * N as f64 - 0.5, c, 0.9);
        }
        // a soft beat at the centre
        let core = led_at(leds, (t * 0.05) % 1.0);
        let beat = ((t * 3.4).sin().max(0.0).powi(8) * 6.0) as f32;
        for k in 0..24 {
            let a = k as f64 * PI / 12.0 + spin;
            self.splat(N as f64 / 2.0 - 0.5 + 3.0 * a.cos(), N as f64 / 2.0 - 0.5 + 3.0 * a.sin(), core, beat);
        }
    }

    // Additive bilinear dot into the feedback buffer.
    fn splat(&mut self, fx: f64, fy: f64, c: Rgb, amount: f32) {
        let (x0, y0) = (fx.floor() as i64, fy.floor() as i64);
        if x0 < 0 || y0 < 0 || x0 >= N as i64 - 1 || y0 >= N as i64 - 1 {
            return;
        }
        let (ax, ay) = ((fx - x0 as f64) as f32, (fy - y0 as f64) as f32);
        let w = [(1.0 - ax) * (1.0 - ay), ax * (1.0 - ay), (1.0 - ax) * ay, ax * ay];
        let (x0, y0) = (x0 as usize, y0 as usize);
        let idx = [(y0 * N + x0) * 3, (y0 * N + x0 + 1) * 3, ((y0 + 1) * N + x0) * 3, ((y0 + 1) * N + x0 + 1) * 3];
        for j in 0..4 {
            let k = w[j] * amount;
            self.fb[idx[j]] += c.0 as f32 * k;
            self.fb[idx[j] + 1] += c.1 as f32 * k;
            self.fb[idx[j] + 2] += c.2 as f32 * k;
        }
    }
}

fn led_at(leds: &[Rgb], f: f64) -> Rgb {
    let n = leds.len();
    let p = (f % 1.0 + 1.0) % 1.0 * n as f64;
    let i0 = p as usize % n;
    let i1 = (i0 + 1) % n;
    let k = p - p.floor();
    let (a, b) = (leds[i0], leds[i1]);
    ((a.0 + (b.0 - a.0) * k).floor(), (a.1 + (b.1 - a.1) * k).floor(), (a.2 + (b.2 - a.2) * k).floor())
}

#[cfg(test)]
mod tests {
    use super::*;

    /// A TTAS block as the plugin writes it.
    fn block(seq: i32, effect: i32, leds: &[u32], speed: i32, sync_ms: i64) -> Vec<u8> {
        let mut b = vec![0u8; SIZE];
        b[0..4].copy_from_slice(&MAGIC.to_le_bytes());
        b[4..8].copy_from_slice(&seq.to_le_bytes());
        b[8..12].copy_from_slice(&effect.to_le_bytes());
        b[12..16].copy_from_slice(&(leds.len() as i32).to_le_bytes());
        b[16..20].copy_from_slice(&speed.to_le_bytes());
        b[24..32].copy_from_slice(&sync_ms.to_le_bytes());
        b[32..40].copy_from_slice(&crate::util::dotnet_ticks_now().to_le_bytes());
        for (i, c) in leds.iter().enumerate() {
            b[40 + i * 4..44 + i * 4].copy_from_slice(&c.to_le_bytes());
        }
        b
    }

    const LEDS: [u32; 8] = [0x0000FF, 0x00FF00, 0xFF0000, 0x00FFFF, 0xFF00FF, 0xFFFF00, 0x808080, 0xFFFFFF];

    #[test]
    fn parses_the_block() {
        let b = block(42, 2, &LEDS, 1, 123_456);
        let s = Snapshot::parse(&b).unwrap();
        assert_eq!((s.seq, s.effect, s.led_count, s.speed, s.sync_ms), (42, 2, 8, 1, 123_456));
        assert_eq!(s.colors[2], 0xFF0000);
        assert!(s.age_secs() < 5.0);
        let mut closed = b.clone();
        closed[0..4].fill(0);
        assert!(Snapshot::parse(&closed).is_none());
        assert!(Snapshot::parse(&b[..SIZE - 1]).is_none());
    }

    fn brightness(p: &Pixmap) -> u64 {
        p.pixels().iter().map(|c| c.red() as u64 + c.green() as u64 + c.blue() as u64).sum()
    }

    /// Frames at chosen Aura times: Synchronize() moves the clock base, so sync_ms = t gives that time.
    fn frames(effect: i32, n: usize) -> Vec<(Pixmap, String)> {
        let mut v = Visualiser::default();
        (0..n)
            .map(|k| {
                let s = Snapshot::parse(&block(k as i32 * 2, effect, &LEDS, 2, 1 + k as i64 * 33)).unwrap();
                v.render(Some(&s))
            })
            .collect()
    }

    #[test]
    fn visualiser_draws_and_animates() {
        for (effect, name) in [(0, "Manual"), (1, "Static"), (2, "Breathing"), (4, "Color cycle"), (5, "Rainbow"), (0xFF, "Default")] {
            let f = frames(effect, 12);
            assert_eq!(f[11].1, format!("Aura Sync · {name}"));
            assert!(brightness(&f[11].0) > 0, "{name}: frame is black");
            assert_ne!(f[8].0.data(), f[11].0.data(), "{name}: frames don't change");
        }
    }

    #[test]
    fn strobing_goes_dark_and_bright() {
        // speed 2 = 2 s per cycle, 4 flashes: on for 0.25 s, off for 0.25 s
        let f = frames(3, 12); // t = 0 .. 0.36 s
        assert!(brightness(&f[7].0) > 0);
        let mut v = Visualiser::default();
        let s = Snapshot::parse(&block(0, 3, &LEDS, 2, 300)).unwrap(); // t = 0.3 s: off
        let (dark, _) = v.render(Some(&s));
        assert_eq!(brightness(&dark), 0, "nothing drawn while off (empty feedback buffer)");
        assert_eq!(f[0].1, "Aura Sync · Strobing");
    }

    #[test]
    fn waiting_screen_without_data() {
        let (p, st) = Visualiser::default().render(None);
        assert!(st.starts_with("Waiting for"));
        assert!(brightness(&p) > 0, "the message is drawn");
    }

    #[test]
    fn colour_helpers() {
        let c = (200.0, 100.0, 50.0);
        assert_eq!(scale(c, 0.5), (100.0, 50.0, 25.0));
        assert_eq!(scale(c, 2.0), c);
        assert_eq!(hsv(0.0, 1.0), (255.0, 0.0, 0.0));
        assert_eq!(hsv(1.0 / 3.0, 1.0), (0.0, 255.0, 0.0));
        assert_eq!(led_at(&[(0.0, 0.0, 0.0), (100.0, 200.0, 50.0)], 0.25), (50.0, 100.0, 25.0));
    }

    /// Writes frames to $TTLCD_DUMP/aura-<effect>-<k>.png for a look:
    /// `TTLCD_DUMP=/tmp/x cargo test -p ttlcd-core aura_dump -- --ignored`
    #[test]
    #[ignore]
    fn aura_dump() {
        let Some(dir) = std::env::var_os("TTLCD_DUMP") else { return };
        for effect in [0, 2, 5] {
            for (k, (p, _)) in frames(effect, 90).iter().enumerate().filter(|(k, _)| k % 30 == 29) {
                let img = image::RgbaImage::from_raw(p.width(), p.height(), p.data().to_vec()).unwrap(); // opaque
                img.save(std::path::Path::new(&dir).join(format!("aura-{effect}-{k}.png"))).unwrap();
            }
        }
    }

    /// One frame of the visualiser costs: `cargo test --release -p ttlcd-core aura_speed -- --ignored --nocapture`
    #[test]
    #[ignore]
    fn aura_speed() {
        let t = std::time::Instant::now();
        let f = frames(5, 60);
        println!("visualiser: {:.2} ms per frame", t.elapsed().as_secs_f64() * 1000.0 / f.len() as f64);
    }
}
