//! Frames for the 480x480 round panel: composing pictures (fill/crop or fit), rotation, JPEG, messages, and the
//! clock / overlay readings (readings.rs). Everything draws into tiny-skia pixmaps (opaque RGBA).
pub mod pump;
pub mod readings;
pub mod text;

use image::imageops::FilterType;
use tiny_skia::{Color, FillRule, FilterQuality, Paint, Pixmap, PixmapPaint, Rect, Transform};

pub const W: u32 = 480;
pub const H: u32 = 480;

pub fn rgb(v: i32) -> Color {
    Color::from_rgba8((v >> 16 & 255) as u8, (v >> 8 & 255) as u8, (v & 255) as u8, 255)
}

pub fn rgba(r: u8, g: u8, b: u8, a: u8) -> Color {
    Color::from_rgba8(r, g, b, a)
}

pub fn paint(c: Color) -> Paint<'static> {
    let mut p = Paint::default();
    p.set_color(c);
    p.anti_alias = true;
    p
}

pub fn blank(bg: i32) -> Pixmap {
    let mut p = Pixmap::new(W, H).unwrap();
    p.fill(rgb(bg));
    p
}

/// A decoded picture, kept at its own size (RGBA, not premultiplied: image's layout).
pub type Picture = image::RgbaImage;

pub fn decode(bytes: &[u8]) -> Option<Picture> {
    image::load_from_memory(bytes).ok().map(|i| i.to_rgba8())
}

/// Upright 480x480 frame: fill (cover + centre crop) or contain (letterbox on the background colour).
pub fn compose(src: Option<&Picture>, contain: bool, bg: i32) -> Pixmap {
    let mut out = blank(bg);
    if let Some(src) = src {
        draw_scaled(&mut out, src, contain, 0.0, 0.0, 1.0);
    }
    out
}

/// Draws `src` scaled to cover (or fit) the panel, times `zoom`, shifted by (pan_x, pan_y) output pixels.
/// Resampled with a proper filter first (large photos), then placed with sub-pixel accuracy.
pub fn draw_scaled(out: &mut Pixmap, src: &Picture, contain: bool, pan_x: f64, pan_y: f64, zoom: f64) {
    let (w, h) = (out.width() as f64, out.height() as f64);
    let (sw0, sh0) = (src.width().max(1) as f64, src.height().max(1) as f64);
    let s = if contain { (w / sw0).min(h / sh0) } else { (w / sw0).max(h / sh0) } * zoom;
    let (sw, sh) = (sw0 * s, sh0 * s);
    let (tw, th) = (sw.round().max(1.0) as u32, sh.round().max(1.0) as u32);
    // don't resample a huge zoomed picture beyond what can be seen: crop the visible part first
    let resized = if tw == src.width() && th == src.height() {
        src.clone()
    } else if (tw as u64) * (th as u64) > 16_000_000 {
        image::imageops::resize(src, tw.min(8000), th.min(8000), FilterType::Triangle)
    } else {
        let filter = if tw < src.width() { FilterType::Triangle } else { FilterType::CatmullRom };
        image::imageops::resize(src, tw, th, filter)
    };
    let Some(pm) = to_pixmap(&resized) else { return };
    let x = (w - sw) / 2.0 + pan_x;
    let y = (h - sh) / 2.0 + pan_y;
    let sx = sw / resized.width() as f64;
    let sy = sh / resized.height() as f64;
    let pp = PixmapPaint { quality: FilterQuality::Bilinear, ..Default::default() };
    out.draw_pixmap(0, 0, pm.as_ref(), &pp, Transform::from_row(sx as f32, 0.0, 0.0, sy as f32, x as f32, y as f32), None);
}

/// RGBA picture -> premultiplied pixmap.
pub fn to_pixmap(img: &Picture) -> Option<Pixmap> {
    let mut pm = Pixmap::new(img.width(), img.height())?;
    for (d, s) in pm.pixels_mut().iter_mut().zip(img.pixels()) {
        let [r, g, b, a] = s.0;
        *d = tiny_skia::ColorU8::from_rgba(r, g, b, a).premultiply();
    }
    Some(pm)
}

/// Turned clockwise by 0/90/180/270 degrees (square frames).
pub fn rotate(p: &Pixmap, degrees: i32) -> Pixmap {
    let turns = ((degrees % 360 + 360) % 360) / 90;
    if turns == 0 {
        return p.clone();
    }
    let (w, h) = (p.width() as f32, p.height() as f32);
    let mut out = Pixmap::new(p.width(), p.height()).unwrap();
    let t = Transform::from_translate(w / 2.0, h / 2.0)
        .pre_rotate(turns as f32 * 90.0)
        .pre_translate(-w / 2.0, -h / 2.0);
    out.draw_pixmap(0, 0, p.as_ref(), &PixmapPaint { quality: FilterQuality::Nearest, ..Default::default() }, t, None);
    out
}

/// JPEG of an opaque pixmap. `swap_rb` writes red and blue exchanged (the LX800 board's panel order).
pub fn jpeg(p: &Pixmap, quality: i32, swap_rb: bool) -> Vec<u8> {
    jpeg_raw(p.data(), p.width() as u16, p.height() as u16, quality, swap_rb)
}

pub fn jpeg_raw(rgba: &[u8], w: u16, h: u16, quality: i32, swap_rb: bool) -> Vec<u8> {
    let mut out = Vec::with_capacity(48 * 1024);
    let ct = if swap_rb { jpeg_encoder::ColorType::Bgra } else { jpeg_encoder::ColorType::Rgba };
    let q = quality.clamp(1, 100) as u8;
    let _ = jpeg_encoder::Encoder::new(&mut out, q).encode(rgba, w, h, ct);
    out
}

/// Re-encode a JPEG (lower quality, or red/blue exchanged).
pub fn recompress(jpeg_bytes: &[u8], quality: i32, swap_rb: bool) -> Option<Vec<u8>> {
    let img = image::load_from_memory(jpeg_bytes).ok()?.to_rgba8();
    Some(jpeg_raw(img.as_raw(), img.width() as u16, img.height() as u16, quality, swap_rb))
}

pub fn black_jpeg() -> Vec<u8> {
    jpeg(&blank(0), 50, false)
}

/// Centred white text on black, wrapped inside the panel (status screens: "No image selected", ...).
pub fn message(msg: &str) -> Pixmap {
    let mut p = blank(0);
    text_block(&mut p, msg, "Segoe UI", false, 24.0, rgb(0xFFFFFF), Rect::from_xywh(40.0, 0.0, 400.0, 480.0).unwrap());
    p
}

/// Word-wrapped, centred text block inside `r` (like DrawString with centred StringFormat).
pub fn text_block(p: &mut Pixmap, msg: &str, family: &str, bold: bool, size: f32, c: Color, r: Rect) {
    let f = text::fonts();
    let mut lines: Vec<String> = Vec::new();
    for para in msg.split('\n') {
        let mut cur = String::new();
        for word in para.split(' ') {
            let trial = if cur.is_empty() { word.to_owned() } else { format!("{cur} {word}") };
            if !cur.is_empty() && f.width(family, bold, size, &trial) > r.width() {
                lines.push(std::mem::take(&mut cur));
                cur = word.to_owned();
            } else {
                cur = trial;
            }
        }
        lines.push(cur);
    }
    let lh = f.line_height(family, bold, size);
    let mut y = r.top() + (r.height() - lh * lines.len() as f32) / 2.0;
    let pt = paint(c);
    for l in &lines {
        let w = f.width(family, bold, size, l);
        if let Some(path) = f.path(family, bold, false, size, r.left() + (r.width() - w) / 2.0, y, l) {
            p.fill_path(&path, &pt, FillRule::Winding, Transform::identity(), None);
        }
        y += lh;
    }
}

/// Local wall-clock time, for the clock face.
#[derive(Clone, Copy, Debug)]
pub struct LocalTime {
    pub year: i32,
    pub month: u32, // 1-12
    pub day: u32,
    pub hour: u32,
    pub minute: u32,
    pub second: u32,
    pub weekday: u32, // 0 = Sunday
}

pub fn local_now() -> LocalTime {
    #[cfg(unix)]
    unsafe {
        let t = libc::time(std::ptr::null_mut());
        let mut tm: libc::tm = std::mem::zeroed();
        libc::localtime_r(&t, &mut tm);
        LocalTime {
            year: tm.tm_year + 1900,
            month: tm.tm_mon as u32 + 1,
            day: tm.tm_mday as u32,
            hour: tm.tm_hour as u32,
            minute: tm.tm_min as u32,
            second: tm.tm_sec as u32,
            weekday: tm.tm_wday as u32,
        }
    }
    #[cfg(windows)]
    unsafe {
        let mut st = std::mem::zeroed();
        windows_sys::Win32::System::SystemInformation::GetLocalTime(&mut st);
        let st: windows_sys::Win32::Foundation::SYSTEMTIME = st;
        LocalTime {
            year: st.wYear as i32,
            month: st.wMonth as u32,
            day: st.wDay as u32,
            hour: st.wHour as u32,
            minute: st.wMinute as u32,
            second: st.wSecond as u32,
            weekday: st.wDayOfWeek as u32,
        }
    }
}

impl LocalTime {
    /// "HH:mm"
    pub fn hhmm(&self) -> String {
        format!("{:02}:{:02}", self.hour, self.minute)
    }
    /// "ddd d MMM", e.g. "Mon 13 Oct"
    pub fn date(&self) -> String {
        const D: [&str; 7] = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
        const M: [&str; 12] = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
        format!("{} {} {}", D[self.weekday as usize % 7], self.day, M[(self.month as usize + 11) % 12])
    }
}
