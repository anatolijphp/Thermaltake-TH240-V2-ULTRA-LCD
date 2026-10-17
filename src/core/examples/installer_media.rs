//! The installer's component GIFs (src/installer/media/), one per optional component, each showing only what that
//! component adds to the LCD:
//!   wallpaper.gif  Wallpaper Engine on the LCD through the virtual display  } the live preview cut from
//!   aura.gif       Aura Sync effects from Armoury Crate                     } docs/media/modes.gif (real footage)
//!   cpu-temp.gif   the CPU temperature reading rising and falling, coloured by value: drawn by the app's own clock
//!                  renderer (render::readings), as the preview shows it
//! Run from src/:  cargo run --release -p ttlcd-core --example installer_media
use image::codecs::gif::{GifDecoder, GifEncoder, Repeat};
use image::imageops::{self, FilterType};
use image::{AnimationDecoder, Delay, Frame, Rgba, RgbaImage};
use std::path::{Path, PathBuf};
use ttlcd_core::config::{Config, ElementStyle, TextEffect};
use ttlcd_core::render::{self, readings};

/// Output size: shown at 180 px (96 DPI) by the installer, its own step per component.
const SIZE: u32 = 216;
/// The recording holds its last frame for ~1 s; looped, that reads as a stall.
const MAX_DELAY: u32 = 200;
/// The installer's card colour (ui.rs CARD): everything outside the round screen.
const CARD: [u8; 3] = [27, 27, 29];
/// The live preview in modes.gif (930x594): the disc with its RGB ring, centre and outer radius.
const DISC_CX: f32 = 775.5;
const DISC_CY: f32 = 230.5;
const CROP: u32 = 260;
const RING_OUT: f32 = 126.5;
const RING_IN: f32 = 123.5;
const SCREEN_R: f32 = 121.5;
/// Studio's preview ring colours (the setup's RGB line uses the same stops).
const RING_STOPS: [[u8; 3]; 5] = [[255, 31, 75], [255, 230, 0], [0, 225, 255], [122, 75, 255], [255, 43, 214]];

fn main() {
    let root = PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("../..");
    let out = root.join("src/installer/media");
    std::fs::create_dir_all(&out).unwrap();
    let modes = read_gif(&root.join("docs/media/modes.gif"));
    // modes.gif: 12..=36 Wallpaper Engine (virtual display), 37..=81 Aura Sync
    write_gif(&out.join("wallpaper.gif"), modes[12..=36].iter().map(|(f, d)| (disc(f), (*d).min(MAX_DELAY))).collect());
    write_gif(&out.join("aura.gif"), modes[37..=81].iter().map(|(f, d)| (disc(f), (*d).min(MAX_DELAY))).collect());
    write_gif(&out.join("cpu-temp.gif"), cpu_temp());
}

fn read_gif(p: &Path) -> Vec<(RgbaImage, u32)> {
    let d = GifDecoder::new(std::io::BufReader::new(std::fs::File::open(p).unwrap())).unwrap();
    d.into_frames()
        .map(|f| {
            let f = f.unwrap();
            let (n, den) = f.delay().numer_denom_ms();
            let ms = n / den.max(1);
            (f.into_buffer(), ms)
        })
        .collect()
}

/// The live preview disc from a Studio frame, everything outside its ring in the card colour.
fn disc(frame: &RgbaImage) -> RgbaImage {
    let (x, y) = ((DISC_CX - CROP as f32 / 2.0).round() as u32, (DISC_CY - CROP as f32 / 2.0).round() as u32);
    let mut c = imageops::crop_imm(frame, x, y, CROP, CROP).to_image();
    let mid = CROP as f32 / 2.0;
    for (px, py, p) in c.enumerate_pixels_mut() {
        let r = ((px as f32 + 0.5 - mid).powi(2) + (py as f32 + 0.5 - mid).powi(2)).sqrt();
        let a = (r - RING_OUT).clamp(0.0, 1.0); // soft edge, 1 px
        p.0 = mix(p.0, CARD, a);
    }
    imageops::resize(&c, SIZE, SIZE, FilterType::Lanczos3)
}

fn mix(p: [u8; 4], c: [u8; 3], a: f32) -> [u8; 4] {
    let m = |x: u8, y: u8| (x as f32 + (y as f32 - x as f32) * a).round() as u8;
    [m(p[0], c[0]), m(p[1], c[1]), m(p[2], c[2]), 255]
}

/// The clock face set to show only the CPU temperature, large, labelled, coloured by value (Studio's "colour by
/// value": cool cyan to hot red), while the temperature climbs under load and settles again.
fn cpu_temp() -> Vec<(RgbaImage, u32)> {
    let mut cfg = Config::default();
    cfg.ShowTime = false;
    cfg.ShowDate = false;
    cfg.ShowWaterTemp = false;
    cfg.ShowCpuLoad = false;
    cfg.ShowCpuTemp = true;
    cfg.ColorRgb = 0x000000;
    cfg.ClockStyles = Some(vec![ElementStyle {
        Id: readings::ID_CPU_TEMP,
        Size: 150,
        ByTemp: true,
        Label: true,
        LabelAbove: true,
        Effect: TextEffect::Glow,
        ..Default::default()
    }]);
    // idle 41 °C, a load spike to 86 °C, back down: eased, 100 ms a frame
    let mut temps = Vec::new();
    let ease = |a: f64, b: f64, n: usize, out: &mut Vec<i32>| {
        for i in 0..n {
            let t = i as f64 / n as f64;
            let s = t * t * (3.0 - 2.0 * t);
            out.push((a + (b - a) * s).round() as i32);
        }
    };
    temps.extend(std::iter::repeat_n(41, 6));
    ease(41.0, 86.0, 16, &mut temps);
    temps.extend(std::iter::repeat_n(86, 6));
    ease(86.0, 41.0, 18, &mut temps);
    let n = temps.len();
    temps
        .into_iter()
        .enumerate()
        .map(|(i, t)| {
            let mut p = render::blank(cfg.ColorRgb);
            readings::draw_clock(&mut p, &cfg, &readings::Values { water: f64::NAN, cpu_temp: t, cpu_load: 0 });
            let lcd = RgbaImage::from_raw(480, 480, p.data().to_vec()).unwrap();
            (preview_disc(&lcd, i as f32 / n as f32), 100)
        })
        .collect()
}

/// A 480x480 LCD frame drawn like Studio's live preview: round screen inside the turning RGB ring.
fn preview_disc(lcd: &RgbaImage, turn: f32) -> RgbaImage {
    let d = (SCREEN_R * 2.0).round() as u32;
    let screen = imageops::resize(lcd, d, d, FilterType::Lanczos3);
    let mid = CROP as f32 / 2.0;
    let off = mid - d as f32 / 2.0;
    let mut c = RgbaImage::from_pixel(CROP, CROP, Rgba([8, 8, 8, 255]));
    for (px, py, p) in c.enumerate_pixels_mut() {
        let (dx, dy) = (px as f32 + 0.5 - mid, py as f32 + 0.5 - mid);
        let r = (dx * dx + dy * dy).sqrt();
        if r < SCREEN_R + 0.5 {
            let (sx, sy) = ((px as f32 - off).clamp(0.0, d as f32 - 1.0), (py as f32 - off).clamp(0.0, d as f32 - 1.0));
            let s = screen.get_pixel(sx as u32, sy as u32).0;
            p.0 = mix(s, [8, 8, 8], (r - SCREEN_R + 0.5).clamp(0.0, 1.0));
        } else if r > RING_IN - 0.5 && r < RING_OUT + 0.5 {
            let t = ((dy.atan2(dx) / std::f32::consts::TAU + 0.5 + turn) % 1.0) * 4.0;
            let i = (t.floor() as usize).min(3);
            let f = t - i as f32;
            let (a, b) = (RING_STOPS[i], RING_STOPS[(i + 1) % 5]);
            let col = [0, 1, 2].map(|k| (a[k] as f32 + (b[k] as f32 - a[k] as f32) * f).round() as u8);
            let edge = ((r - (RING_IN - 0.5)).min(RING_OUT + 0.5 - r)).clamp(0.0, 1.0);
            p.0 = mix([col[0], col[1], col[2], 255], [8, 8, 8], 1.0 - edge);
        }
        let a = (r - RING_OUT).clamp(0.0, 1.0);
        p.0 = mix(p.0, CARD, a);
    }
    imageops::resize(&c, SIZE, SIZE, FilterType::Lanczos3)
}

fn write_gif(p: &Path, frames: Vec<(RgbaImage, u32)>) {
    let n = frames.len();
    let f = std::fs::File::create(p).unwrap();
    let mut enc = GifEncoder::new_with_speed(f, 10);
    enc.set_repeat(Repeat::Infinite).unwrap();
    enc.encode_frames(frames.into_iter().map(|(img, ms)| Frame::from_parts(img, 0, 0, Delay::from_numer_denom_ms(ms, 1))))
        .unwrap();
    println!("{} ({n} frames, {} KB)", p.display(), std::fs::metadata(p).unwrap().len() / 1024);
}
