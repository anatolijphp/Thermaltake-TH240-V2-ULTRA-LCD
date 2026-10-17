//! The optional components, one setup step each, with a GIF of what it adds to the LCD (media/*.gif, made
//! by core/examples/installer_media.rs), and the GIF decoding into frames the window can draw as they are.

/// An optional component: they all need administrator rights (ttlcd.exe asks through UAC while installing).
pub struct Component {
    pub title: &'static str,
    /// the checkbox on its step
    pub check: &'static str,
    pub text: &'static str,
    pub gif: &'static [u8],
}

/// In step order, which is also install::Options' order: sensor_service, aura, virtual_display.
/// How many there are (one setup step each; layout::STEPS).
pub const COMPONENT_COUNT: usize = 3;

/// A static, not a const: a const would copy the GIF bytes into the exe at every use.
pub static COMPONENTS: [Component; COMPONENT_COUNT] = [
    Component {
        title: "CPU temperature",
        check: "Install the CPU temperature service",
        text: "Shows the CPU temperature on the LCD and the dashboard, coloured by value. A small Windows service \
               reads it through the TOWEI driver.",
        gif: include_bytes!("../media/cpu-temp.gif"),
    },
    Component {
        title: "Armoury Crate (Aura Sync)",
        check: "Install the Armoury Crate plugin",
        text: "The cooler screen follows your Aura Sync lighting effects: adds the TT LCD plugin to Armoury Crate.",
        gif: include_bytes!("../media/aura.gif"),
    },
    Component {
        title: "Wallpaper Engine",
        check: "Install the virtual display",
        text: "Adds a virtual 480×480 monitor (display driver). Give it a wallpaper in Wallpaper Engine and it plays \
               on the LCD.",
        gif: include_bytes!("../media/wallpaper.gif"),
    },
];

pub struct Anim {
    pub width: u32,
    pub height: u32,
    pub frames: Vec<AnimFrame>,
}

pub struct AnimFrame {
    /// whole picture, top-down rows, B G R x (a 32-bit DIB)
    pub bgra: Vec<u8>,
    pub delay_ms: u32,
}

/// Decodes a GIF into whole pictures (frames composited with their disposal methods); transparent pixels get `bg`.
pub fn decode(bytes: &[u8], bg: [u8; 3]) -> Result<Anim, String> {
    use gif::DisposalMethod;
    let mut o = gif::DecodeOptions::new();
    o.set_color_output(gif::ColorOutput::RGBA);
    let mut d = o.read_info(bytes).map_err(|e| e.to_string())?;
    let (w, h) = (d.width() as usize, d.height() as usize);
    let mut canvas = vec![0u8; w * h * 4]; // RGBA, alpha 0 = nothing drawn yet
    let mut frames = Vec::new();
    while let Some(f) = d.read_next_frame().map_err(|e| e.to_string())? {
        let saved = (f.dispose == DisposalMethod::Previous).then(|| canvas.clone());
        let (fx, fy, fw, fh) = (f.left as usize, f.top as usize, f.width as usize, f.height as usize);
        for y in 0..fh.min(h.saturating_sub(fy)) {
            for x in 0..fw.min(w.saturating_sub(fx)) {
                let s = &f.buffer[(y * fw + x) * 4..][..4];
                if s[3] != 0 {
                    canvas[((fy + y) * w + fx + x) * 4..][..4].copy_from_slice(s);
                }
            }
        }
        let mut bgra = vec![0u8; w * h * 4];
        for (o, c) in bgra.chunks_exact_mut(4).zip(canvas.chunks_exact(4)) {
            let p = if c[3] == 0 { [bg[2], bg[1], bg[0]] } else { [c[2], c[1], c[0]] };
            o[..3].copy_from_slice(&p);
            o[3] = 255;
        }
        // browsers show delays below 20 ms as 100 ms
        let delay_ms = if f.delay < 2 { 100 } else { f.delay as u32 * 10 };
        frames.push(AnimFrame { bgra, delay_ms });
        match f.dispose {
            DisposalMethod::Background => {
                for y in fy..(fy + fh).min(h) {
                    for x in fx..(fx + fw).min(w) {
                        canvas[(y * w + x) * 4..][..4].fill(0);
                    }
                }
            }
            DisposalMethod::Previous => canvas = saved.unwrap_or(canvas),
            _ => {}
        }
    }
    if frames.is_empty() {
        return Err("GIF without frames".into());
    }
    Ok(Anim { width: w as u32, height: h as u32, frames })
}

/// Fades the lower half into `bg`: fully transparent at the bottom row, fully opaque from half the height up
/// (linear in between).
pub fn fade_bottom(a: &mut Anim, bg: [u8; 3]) {
    let (w, h) = (a.width as usize, a.height as usize);
    let half = h as f32 / 2.0;
    for f in &mut a.frames {
        for y in 0..h {
            // pixel centres: the bottom row is fully faded, the row at half height is untouched
            let opacity = ((h - 1 - y) as f32 / (half - 1.0).max(1.0)).min(1.0);
            if opacity >= 1.0 {
                continue;
            }
            for p in f.bgra[y * w * 4..(y + 1) * w * 4].chunks_exact_mut(4) {
                for (k, c) in [bg[2], bg[1], bg[0]].into_iter().enumerate() {
                    p[k] = (c as f32 + (p[k] as f32 - c as f32) * opacity).round() as u8;
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// 2026-10-17: the component GIFs fade out towards the bottom: transparent (the card colour) at the bottom
    /// edge, opaque from half the height up, half way at a quarter.
    #[test]
    fn gifs_fade_out_at_the_bottom() {
        let (w, h) = (4usize, 100usize);
        let frame = AnimFrame { bgra: vec![200; w * h * 4], delay_ms: 100 };
        let mut a = Anim { width: w as u32, height: h as u32, frames: vec![frame] };
        let bg = [10, 20, 30];
        fade_bottom(&mut a, bg);
        let px = |y: usize| &a.frames[0].bgra[y * w * 4..][..3];
        assert_eq!(px(h - 1), &[30, 20, 10], "bottom row: the background");
        assert_eq!(px(h / 2), &[200, 200, 200], "half height: untouched");
        assert_eq!(px(0), &[200, 200, 200]);
        let quarter = px(h - 1 - (h / 2 - 1) / 2)[0] as i32; // half way between bottom and half height
        assert!((quarter - (30 + 200) / 2).abs() <= 2, "{quarter}");
        for y in h / 2..h - 1 {
            assert!(px(y)[0] >= px(y + 1)[0], "fades monotonically at row {y}");
        }
    }

    /// 2026-10-17, a step per component in the setup: every component GIF decodes into an animation the window can
    /// play (the size the steps are laid out for, real frame delays, frames that actually change).
    #[test]
    fn component_gifs_play() {
        for c in &COMPONENTS {
            let a = decode(c.gif, [27, 27, 29]).unwrap_or_else(|e| panic!("{}: {e}", c.title));
            assert_eq!((a.width, a.height), (216, 216), "{}", c.title);
            assert!(a.frames.len() >= 10, "{}: {} frames", c.title, a.frames.len());
            for f in &a.frames {
                assert_eq!(f.bgra.len(), 216 * 216 * 4);
                assert!((50..=500).contains(&f.delay_ms), "{}: delay {}", c.title, f.delay_ms);
            }
            let changing = a.frames.windows(2).filter(|p| p[0].bgra != p[1].bgra).count();
            assert!(changing >= a.frames.len() / 2, "{}: only {changing} frames change", c.title);
            // the corners are the card colour, so the GIF sits on its card without a visible square
            assert_eq!(&a.frames[0].bgra[..3], &[29, 27, 27], "{}", c.title);
        }
    }

    /// A frame that covers only part of the picture keeps the rest from before (Keep), and Background disposal
    /// clears its area for the next frame.
    #[test]
    fn partial_frames_compose() {
        let mut out = Vec::new();
        {
            let mut e = gif::Encoder::new(&mut out, 2, 1, &[255, 0, 0, 0, 0, 255]).unwrap();
            let mut f = gif::Frame { width: 2, height: 1, buffer: vec![0, 0].into(), delay: 5, ..Default::default() };
            e.write_frame(&f).unwrap(); // red red
            f = gif::Frame { left: 1, width: 1, height: 1, buffer: vec![1].into(), delay: 5, ..Default::default() };
            f.dispose = gif::DisposalMethod::Background;
            e.write_frame(&f).unwrap(); // red blue, then the right pixel is cleared
            f = gif::Frame { width: 1, height: 1, buffer: vec![1].into(), delay: 0, ..Default::default() };
            e.write_frame(&f).unwrap(); // blue, background
        }
        let a = decode(&out, [9, 8, 7]).unwrap();
        let px = |i: usize| a.frames[i].bgra.chunks(4).map(|p| [p[2], p[1], p[0]]).collect::<Vec<_>>();
        assert_eq!(px(0), [[255, 0, 0], [255, 0, 0]]);
        assert_eq!(px(1), [[255, 0, 0], [0, 0, 255]]);
        assert_eq!(px(2), [[0, 0, 255], [9, 8, 7]]);
        assert_eq!(a.frames[1].delay_ms, 50);
        assert_eq!(a.frames[2].delay_ms, 100);
    }
}
