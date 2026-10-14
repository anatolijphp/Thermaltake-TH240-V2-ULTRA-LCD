//! The coolant temperature exactly as the LX800 firmware draws it on its idle screen: its own two bitmap fonts
//! (4-bit antialiased glyphs, an Arial look; 100 px and 60 px cells) and its own layout, so Studio's preview and the
//! frame sent before the cooler takes over look the same as the cooler.
//!
//! Found in firmware/lx800_tt021panel.bin (loaded at 0xFFFE0): each font is a range list (first char, last char,
//! glyph table, next range) whose glyph entries are {width, advance, bytes per row, 0, bitmap pointer}; a bitmap is
//! `cell height` rows of `bytes per row`, two pixels per byte (high nibble first), 0 = transparent, 15 = solid.
//! The idle screen's drawing function (0x12808c) prints "%d" of the reading (1/100 °C, truncated) with the big font
//! at (x, 100), then "°" with the small font at (x + number width, 103), then "C " with the big font after the
//! degree sign; x centres number + small "C " + small "°" on the 480 px screen. The text colour is the config
//! record's 0xCCCCCCCC (never changed: command 0x16 isn't sent), shown as #CCCCCC.
//!
//! The glyphs it needs are kept in fonts/lx800-idle.bin (made from the firmware by the `extract` test below and
//! checked against it by `matches_the_firmware`).
use tiny_skia::Pixmap;

const GLYPHS: &[u8] = include_bytes!("../fonts/lx800-idle.bin");
const MAGIC: &[u8; 4] = b"TTIG";

/// The idle screen's text colour.
pub const COLOUR: [u8; 3] = [0xCC, 0xCC, 0xCC];
/// Where the firmware puts the number's cell (config record 0xFC / 0x100 defaults: x offset 0, y 100).
const TOP: i32 = 100;
/// The degree sign's cell is 3 px lower.
const DEG_DROP: i32 = 3;

#[derive(Clone, Copy, PartialEq, Eq, Debug)]
enum Font {
    Big,
    Small,
}

struct Glyph<'a> {
    width: i32,
    bpr: usize,
    height: usize,
    data: &'a [u8],
}

fn glyph(font: Font, c: char) -> Option<Glyph<'static>> {
    // "TTIG", then per glyph: font (0 big, 1 small), char (u32 LE), width, bytes per row, height, bitmap
    let mut o = MAGIC.len();
    while o + 8 <= GLYPHS.len() {
        let f = GLYPHS[o];
        let ch = u32::from_le_bytes(GLYPHS[o + 1..o + 5].try_into().ok()?);
        let (width, bpr, height) = (GLYPHS[o + 5] as i32, GLYPHS[o + 6] as usize, GLYPHS[o + 7] as usize);
        let data = &GLYPHS[o + 8..o + 8 + bpr * height];
        if f == font as u8 && ch == c as u32 {
            return Some(Glyph { width, bpr, height, data });
        }
        o += 8 + bpr * height;
    }
    None
}

fn width(font: Font, s: &str) -> i32 {
    s.chars().filter_map(|c| glyph(font, c)).map(|g| g.width).sum()
}

/// Draws `s` with its cell's top-left at (x, y), blending the glyphs' 4-bit coverage in `COLOUR` over `p`.
fn draw(p: &mut Pixmap, font: Font, s: &str, mut x: i32, y: i32) {
    let (pw, ph) = (p.width() as i32, p.height() as i32);
    let px = p.pixels_mut();
    for c in s.chars() {
        let Some(g) = glyph(font, c) else { continue };
        for row in 0..g.height {
            let yy = y + row as i32;
            if yy < 0 || yy >= ph {
                continue;
            }
            for col in 0..g.bpr * 2 {
                let b = g.data[row * g.bpr + col / 2];
                let a = if col % 2 == 0 { b >> 4 } else { b & 15 } as u32 * 17;
                let xx = x + col as i32;
                if a == 0 || xx < 0 || xx >= pw {
                    continue;
                }
                let i = (yy * pw + xx) as usize;
                let d = px[i];
                // premultiplied "source over"
                let mix = |s: u8, d: u8| ((s as u32 * a + d as u32 * (255 - a) + 127) / 255) as u8;
                px[i] = tiny_skia::PremultipliedColorU8::from_rgba(
                    mix(COLOUR[0], d.red()),
                    mix(COLOUR[1], d.green()),
                    mix(COLOUR[2], d.blue()),
                    mix(255, d.alpha()),
                )
                .unwrap_or(d);
            }
        }
        x += g.width;
    }
}

/// The whole degrees the firmware shows for a reading: it prints the reading in 1/100 °C divided by 100, so the
/// fraction is dropped (31.9 shows as 31).
pub fn degrees(coolant: f64) -> i64 {
    coolant.trunc() as i64
}

/// The temperature as the idle screen draws it, over a 480×480 picture (upright, like the firmware's own screen
/// before its rotation).
pub fn draw_temperature(p: &mut Pixmap, degrees: i64) {
    let num = degrees.to_string();
    let unit = "C ";
    let (wn, wu, wd) = (width(Font::Big, &num), width(Font::Small, unit), width(Font::Small, "°"));
    let x = (480 - wn - wu - wd) / 2; // C division: truncates toward zero, like the firmware's
    draw(p, Font::Big, &num, x, TOP);
    draw(p, Font::Big, unit, x + wn + wd, TOP);
    draw(p, Font::Small, "°", x + wn, TOP + DEG_DROP);
}

/// Just the temperature on a transparent 480×480 image (Studio lays it over the idle picture in the preview).
pub fn overlay_png(degrees: i64) -> Vec<u8> {
    let mut p = Pixmap::new(480, 480).expect("480x480");
    draw_temperature(&mut p, degrees);
    let rgba: Vec<u8> = p.pixels().iter().flat_map(|c| { let c = c.demultiply(); [c.red(), c.green(), c.blue(), c.alpha()] }).collect();
    let mut out = Vec::new();
    let img = image::RgbaImage::from_raw(480, 480, rgba).expect("480x480 RGBA");
    let _ = img.write_to(&mut std::io::Cursor::new(&mut out), image::ImageFormat::Png);
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    const BASE: usize = 0xFFFE0;
    /// The glyphs kept from each font: what the idle screen can print in °C (and °F, if the scale were ever switched).
    const BIG_CHARS: &str = "0123456789- CF";
    const SMALL_CHARS: &str = " CF°";
    /// The two fonts' range lists in the firmware image (file offsets).
    const RANGES: [(Font, usize); 2] = [(Font::Big, 0x6a778), (Font::Small, 0x801d4)];

    fn u32_at(d: &[u8], o: usize) -> u32 {
        u32::from_le_bytes(d[o..o + 4].try_into().unwrap())
    }

    /// The glyph file made straight from the firmware image.
    fn from_firmware() -> Vec<u8> {
        let fw = std::fs::read(concat!(env!("CARGO_MANIFEST_DIR"), "/../../firmware/lx800_tt021panel.bin")).unwrap();
        let mut out = MAGIC.to_vec();
        for (font, ranges) in RANGES {
            let chars = if font == Font::Big { BIG_CHARS } else { SMALL_CHARS };
            for c in chars.chars() {
                // walk the range list to the glyph entry
                let mut r = ranges;
                let entry = loop {
                    let (lo, hi) = (u16::from_le_bytes([fw[r], fw[r + 1]]) as u32, u16::from_le_bytes([fw[r + 2], fw[r + 3]]) as u32);
                    let table = u32_at(&fw, r + 4) as usize - BASE;
                    if (lo..=hi).contains(&(c as u32)) {
                        break table + (c as u32 - lo) as usize * 8;
                    }
                    let next = u32_at(&fw, r + 8) as usize;
                    assert_ne!(next, 0, "{c:?} is not in the firmware's font");
                    r = next - BASE;
                };
                let (w, bpr) = (fw[entry], fw[entry + 2] as usize);
                let bitmap = u32_at(&fw, entry + 4) as usize - BASE;
                // every glyph of a font has the same cell height: its bitmap size over its row size
                let next = u32_at(&fw, entry + 12) as usize - BASE;
                let height = if font == Font::Big { 100 } else { 60 };
                assert!(next == bitmap + bpr * height || c == '°' || c == 'F', "{c:?}: cell height");
                out.push(font as u8);
                out.extend_from_slice(&(c as u32).to_le_bytes());
                out.extend_from_slice(&[w, bpr as u8, height as u8]);
                out.extend_from_slice(&fw[bitmap..bitmap + bpr * height]);
            }
        }
        out
    }

    #[test]
    fn matches_the_firmware() {
        assert!(GLYPHS == from_firmware().as_slice(), "fonts/lx800-idle.bin differs from the firmware: run the extract test");
    }

    /// Writes fonts/lx800-idle.bin: `cargo test -p ttlcd-core idle_text::tests::extract -- --ignored`
    #[test]
    #[ignore]
    fn extract() {
        std::fs::write(concat!(env!("CARGO_MANIFEST_DIR"), "/fonts/lx800-idle.bin"), from_firmware()).unwrap();
    }

    #[test]
    fn layout_as_the_firmware() {
        // big digits are 50 px, small "C " 39 + 15, small "°" 22: "31" starts at (480 - 100 - 54 - 22) / 2
        assert_eq!((width(Font::Big, "31"), width(Font::Small, "C "), width(Font::Small, "°")), (100, 54, 22));
        let mut p = Pixmap::new(480, 480).unwrap();
        p.fill(tiny_skia::Color::BLACK);
        draw_temperature(&mut p, 31);
        let lit = |x: u32, y: u32| p.pixel(x, y).unwrap().red() > 128;
        let rows: Vec<u32> = (0..480).filter(|&y| (0..480).any(|x| lit(x, y))).collect();
        let cols: Vec<u32> = (0..480).filter(|&x| (0..480).any(|y| lit(x, y))).collect();
        assert_eq!((rows[0], *rows.last().unwrap()), (113, 181), "degree sign top .. digits bottom");
        assert_eq!(cols[0], 156, "the 3's left edge");
        assert_eq!(degrees(31.9), 31);
    }

    /// Studio's Idle preview lays overlay_png over the picture; it must be the same drawing as the frame sent to the
    /// cooler (2026-10-15: the CSS-drawn preview and the frame differed).
    #[test]
    fn preview_overlay_is_the_frame_drawing() {
        let png = image::load_from_memory(&overlay_png(42)).unwrap().to_rgba8();
        let mut frame = Pixmap::new(480, 480).unwrap();
        frame.fill(tiny_skia::Color::BLACK);
        draw_temperature(&mut frame, 42);
        let mut worst = 0i32;
        for (i, px) in frame.pixels().iter().enumerate() {
            let o = png.as_raw()[i * 4..i * 4 + 4].to_vec();
            // the overlay composited over black
            let over = (o[0] as i32 * o[3] as i32 + 127) / 255;
            worst = worst.max((over - px.red() as i32).abs());
        }
        assert!(worst <= 2, "overlay and frame differ by up to {worst} levels");
    }
}
