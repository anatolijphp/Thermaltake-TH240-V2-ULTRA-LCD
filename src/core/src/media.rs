//! Pictures and animated GIFs from files, as panel frames.
use crate::render::{self, Picture};
use tiny_skia::Pixmap;

pub fn load_picture(path: &str) -> Option<Picture> {
    render::decode(&std::fs::read(path).ok()?)
}

/// Every frame of a GIF (fully composited) with its delay in ms. Delays under 20 ms play at 100 ms, as browsers do.
pub fn gif_frames(path: &str) -> Option<Vec<(Picture, i32)>> {
    use image::AnimationDecoder;
    let data = std::fs::read(path).ok()?;
    let dec = image::codecs::gif::GifDecoder::new(std::io::Cursor::new(data)).ok()?;
    let mut out = Vec::new();
    for f in dec.into_frames() {
        let f = f.ok()?;
        let (n, d) = f.delay().numer_denom_ms();
        let ms = if d == 0 { 0 } else { (n / d) as i32 };
        out.push((f.into_buffer(), if ms < 20 { 100 } else { ms }));
    }
    if out.is_empty() {
        // a still image saved as .gif (or another format with that name)
        out.push((load_picture(path)?, 1000));
    }
    Some(out)
}

/// GIF frames composed for the panel (fill or fit on the background colour).
pub fn gif_panel_frames(path: &str, contain: bool, bg: i32) -> Option<Vec<(Pixmap, i32)>> {
    Some(gif_frames(path)?.into_iter().map(|(p, d)| (render::compose(Some(&p), contain, bg), d)).collect())
}
