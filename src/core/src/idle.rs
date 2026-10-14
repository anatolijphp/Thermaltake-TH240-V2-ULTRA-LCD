//! The cooler's idle picture (stored in its flash, LX800 firmware): choosing and placing a new picture, encoding the
//! exact JPEG that would be uploaded, storing it with read-back, and reading the stored one (port of IdleEditor.cs).
//! No UI: front ends show `jpeg` and `info` and the progress, and call pan / set_zoom while the user drags and zooms.
use crate::engine::{DeviceJob, Engine};
use crate::lcd::{STANDBY_FILE, STANDBY_MAX_LX};
use crate::render::{self, Picture};
use std::io;
use std::path::PathBuf;
use std::sync::{Arc, Mutex};

pub const LIMIT: usize = STANDBY_MAX_LX;
pub const VIDEO_EXT: [&str; 7] = ["mp4", "mov", "avi", "mkv", "webm", "wmv", "m4v"];

pub fn cache() -> PathBuf {
    crate::config::folder().join("idle-current.jpg")
}

/// The stored picture as last uploaded or read back (cached in idle-current.jpg).
pub fn stored() -> Option<Vec<u8>> {
    std::fs::read(cache()).ok()
}

#[derive(Default)]
pub struct IdleEditor {
    src: Option<Picture>, // picture being placed; None = showing the stored one
    pub zoom: f64,        // over the "cover" scale
    pub pan_x: f64,       // output pixels
    pub pan_y: f64,
    pub jpeg: Option<Vec<u8>>, // pending upload
    pub info: String,
    pub busy: bool,
    pub tried_read: bool,
}

impl IdleEditor {
    pub fn new() -> IdleEditor {
        IdleEditor { zoom: 1.0, ..Default::default() }
    }

    pub fn editing(&self) -> bool {
        self.src.is_some()
    }

    pub fn fits(&self) -> bool {
        self.jpeg.as_ref().is_some_and(|j| j.len() <= LIMIT)
    }

    /// Picture, GIF (first frame) or video (first frame; Windows). Err with a readable message.
    pub fn load(&mut self, file: &str) -> io::Result<()> {
        let ext = file.rsplit('.').next().unwrap_or("").to_ascii_lowercase();
        let pic = if VIDEO_EXT.contains(&ext.as_str()) {
            if !crate::video::supported() {
                return Err(io::Error::other("videos need the Windows version; choose a picture or GIF"));
            }
            let o = crate::video::Options { path: file.into(), start: 0.0, length: 1.0, fps: 1, zoom: 1.0, pan_x: 0.0, pan_y: 0.0, quality: 92 };
            let (p, _) = crate::video::thumbnail(&o, 0.0)?;
            let p = p.ok_or_else(|| io::Error::other("could not decode the video"))?;
            image::RgbaImage::from_raw(p.width(), p.height(), p.data().to_vec()).ok_or_else(|| io::Error::other("bad frame"))?
        } else {
            crate::media::load_picture(file).ok_or_else(|| io::Error::other("not a picture this app can read"))?
        };
        self.drop_pending();
        self.src = Some(pic);
        self.pan_x = 0.0;
        self.pan_y = 0.0;
        self.zoom = 1.0;
        self.encode();
        Ok(())
    }

    pub fn drop_pending(&mut self) {
        self.src = None;
        self.jpeg = None;
    }

    fn cover(&self) -> f64 {
        let s = self.src.as_ref().unwrap();
        (480.0 / s.width() as f64).max(480.0 / s.height() as f64)
    }

    pub fn set_zoom(&mut self, z: f64) {
        self.zoom = z;
        self.clamp_pan();
    }

    pub fn pan(&mut self, dx: f64, dy: f64) {
        self.pan_x += dx;
        self.pan_y += dy;
        self.clamp_pan();
    }

    // keep the picture covering the whole screen
    fn clamp_pan(&mut self) {
        let Some(src) = &self.src else { return };
        let s = self.cover() * self.zoom;
        let mx = ((src.width() as f64 * s - 480.0) / 2.0).max(0.0);
        let my = ((src.height() as f64 * s - 480.0) / 2.0).max(0.0);
        self.pan_x = self.pan_x.clamp(-mx, mx);
        self.pan_y = self.pan_y.clamp(-my, my);
    }

    /// Crop -> 480x480 -> JPEG, lowering quality until it fits the limit. Not rotated: unlike streamed frames, the
    /// idle screen is drawn by the firmware, which already shows it upright.
    pub fn encode(&mut self) {
        let Some(src) = &self.src else { return };
        let mut out = render::blank(0);
        render::draw_scaled(&mut out, src, false, self.pan_x, self.pan_y, self.zoom);
        let mut q = 92;
        let mut j = render::jpeg(&out, q, false);
        while j.len() > LIMIT && q > 30 {
            q -= 6;
            j = render::jpeg(&out, q, false);
        }
        self.info = format!(
            "480×480 JPEG · quality {} · {:.0} KB of {} KB{}",
            q,
            j.len() as f64 / 1024.0,
            LIMIT / 1024,
            if j.len() <= LIMIT { "" } else { " · too large" }
        );
        self.jpeg = Some(j);
    }
}

pub fn store_progress_text(p: i32) -> &'static str {
    if p < 80 { "Writing to the cooler, do not unplug" } else { "Checking the written image" }
}

/// Read the stored picture back from the cooler (read-only commands 0x0C/0x0D, LX800 firmware). progress and done
/// run on the engine's control thread.
pub fn read(ed: &Arc<Mutex<IdleEditor>>, engine: &Engine, progress: impl FnMut(i32) + Send + 'static, done: impl FnOnce(String) + Send + 'static) {
    {
        let mut e = ed.lock().unwrap();
        if e.busy {
            return;
        }
        e.busy = true;
        e.tried_read = true;
    }
    let data: Arc<Mutex<Option<Vec<u8>>>> = Arc::default();
    let (d1, d2, ed2) = (data.clone(), data, ed.clone());
    engine.enqueue(DeviceJob {
        name: "idle-read".into(),
        progress: Box::new(progress),
        work: Box::new(move |lcd, prog| {
            if !lcd.swap_red_blue {
                return Err(io::Error::other("reading the idle image needs the LX800 firmware"));
            }
            let b = lcd.read_file_lx(STANDBY_FILE, prog)?;
            *d1.lock().unwrap() = Some(if b.is_empty() { b } else { lcd.for_panel(&b) }); // undo the panel's swap
            Ok(())
        }),
        done: Box::new(move |err| {
            let data = d2.lock().unwrap().take();
            if let (None, Some(d)) = (&err, &data) {
                if !d.is_empty() {
                    let _ = std::fs::write(cache(), d);
                }
            }
            ed2.lock().unwrap().busy = false;
            done(match (err, data) {
                (Some(e), _) => format!("Could not read it: {e}"),
                (None, Some(d)) if !d.is_empty() => "Read from the cooler.".into(),
                _ => "The cooler has no idle image stored.".into(),
            });
        }),
    });
}

/// Write the pending picture into the cooler's flash (LX800 only; verified by read-back). Streaming pauses.
/// done(status, ok) runs on the engine's control thread.
pub fn store(ed: &Arc<Mutex<IdleEditor>>, engine: &Engine, progress: impl FnMut(i32) + Send + 'static, done: impl FnOnce(String, bool) + Send + 'static) {
    let jpg = {
        let mut e = ed.lock().unwrap();
        let Some(j) = e.jpeg.clone() else { return };
        if e.busy {
            return;
        }
        e.busy = true;
        j
    };
    let (j1, j2, ed2) = (jpg.clone(), jpg, ed.clone());
    engine.enqueue(DeviceJob {
        name: "idle".into(),
        progress: Box::new(progress),
        work: Box::new(move |lcd, prog| {
            if !crate::store::write_standby(lcd, &j1, prog)? {
                return Err(io::Error::other("read-back did not match"));
            }
            Ok(())
        }),
        done: Box::new(move |err| {
            if err.is_none() {
                let _ = std::fs::write(cache(), &j2);
            }
            ed2.lock().unwrap().busy = false;
            match err {
                None => done("Stored and verified on the cooler.".into(), true),
                Some(e) => done(format!("Failed: {e}"), false),
            }
        }),
    });
}
