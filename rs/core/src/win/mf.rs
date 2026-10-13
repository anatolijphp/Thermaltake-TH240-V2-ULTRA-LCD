//! Media Foundation source reader (video frames as RGBA pictures). Port of the Reader in Video.cs.
use crate::render::Picture;
use std::io;

pub struct Reader {
    pub duration: f64,
}

impl Reader {
    pub fn open(path: &str) -> io::Result<Reader> {
        let _ = path;
        Err(io::Error::other("not ported yet"))
    }
    pub fn seek(&mut self, seconds: f64) {
        let _ = seconds;
    }
    /// want(ts): decode this frame?  got(picture, ts): return false to stop.
    pub fn read_frames(&mut self, want: &mut dyn FnMut(f64) -> bool, got: &mut dyn FnMut(Picture, f64) -> bool) -> io::Result<()> {
        let _ = (want, got);
        Ok(())
    }
}
