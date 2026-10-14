//! Monitors and screen capture of a monitor (or a region of it) for streaming to the LCD. Windows: GDI (see
//! win/capture.rs). Linux has no capture (Wayland needs a portal; the modes that use it are Windows-only, as before).

#[derive(Clone, Debug, PartialEq, Eq)]
pub struct Monitor {
    pub name: String,
    pub x: i32,
    pub y: i32,
    pub w: i32,
    pub h: i32,
    pub primary: bool,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct RectI {
    pub x: i32,
    pub y: i32,
    pub w: i32,
    pub h: i32,
}

pub fn monitors() -> Vec<Monitor> {
    #[cfg(windows)]
    {
        crate::win::capture::monitors()
    }
    #[cfg(not(windows))]
    Vec::new()
}

/// The monitor whose resolution matches the panel (the virtual LCD display), or None.
pub fn find_by_size(w: i32, h: i32) -> Option<usize> {
    monitors().iter().position(|m| m.w == w && m.h == h)
}

pub fn monitor_rect(index: usize, sub: Option<RectI>) -> Option<RectI> {
    let all = monitors();
    let m = all.get(index)?;
    Some(match sub {
        None => RectI { x: m.x, y: m.y, w: m.w, h: m.h },
        Some(s) => RectI { x: m.x + s.x, y: m.y + s.y, w: s.w, h: s.h },
    })
}

/// Grabs a fixed screen rectangle repeatedly.
pub struct ScreenSource {
    #[cfg(windows)]
    inner: crate::win::capture::Grabber,
    pub rect: RectI,
}

impl ScreenSource {
    pub fn new(rect: RectI) -> std::io::Result<ScreenSource> {
        #[cfg(windows)]
        {
            Ok(ScreenSource { inner: crate::win::capture::Grabber::new(rect)?, rect })
        }
        #[cfg(not(windows))]
        {
            let _ = rect;
            Err(std::io::Error::other("screen capture needs the Windows version"))
        }
    }

    pub fn grab(&mut self) -> std::io::Result<crate::render::Picture> {
        #[cfg(windows)]
        {
            self.inner.grab()
        }
        #[cfg(not(windows))]
        Err(std::io::Error::other("screen capture needs the Windows version"))
    }
}
