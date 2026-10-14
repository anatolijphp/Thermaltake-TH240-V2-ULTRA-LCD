//! Linux: single instance via 127.0.0.1:47913 ("ttlcd show", same protocol as the C# GTK Studio), ttlcd user-service
//! handover, X11 screenshot of the window.
use std::io::{BufRead, BufReader, Write};
use std::net::{Ipv4Addr, SocketAddr, TcpListener, TcpStream};
use std::time::Duration;

const PORT: u16 = 47913;

pub struct InstanceGuard;

/// None: another Studio runs and was asked to show its window.
pub fn single_instance() -> Option<InstanceGuard> {
    let addr = SocketAddr::from((Ipv4Addr::LOCALHOST, PORT));
    if let Ok(mut c) = TcpStream::connect_timeout(&addr, Duration::from_millis(300)) {
        if c.write_all(b"ttlcd show\n").is_ok() {
            let _ = c.flush();
            return None;
        }
    }
    Some(InstanceGuard)
}

/// A later launch asks this one to show its window. Port taken by something else: run without the rule.
pub fn listen_show(show: Box<dyn Fn() + Send>) {
    let Ok(l) = TcpListener::bind((Ipv4Addr::LOCALHOST, PORT)) else { return };
    let _ = std::thread::Builder::new().name("single-instance".into()).spawn(move || {
        for c in l.incoming() {
            let Ok(c) = c else { continue };
            let _ = c.set_read_timeout(Some(Duration::from_secs(2)));
            let mut line = String::new();
            if BufReader::new(c).read_line(&mut line).is_ok() && line.trim() == "ttlcd show" {
                show();
            }
        }
    });
}

/// The `ttlcd run` user service would drive the cooler too: Studio takes over while it is open.
pub fn before_start() -> Option<String> {
    if ttlcd_core::platform::service_active() {
        ttlcd_core::platform::stop_service();
        return Some("The ttlcd background service was stopped while Studio runs.".into());
    }
    None
}

pub fn after_exit() {}

/// The window's pixels as the X server has them (GDK on X11; screenshot mode forces GDK_BACKEND=x11, so this also
/// works under XWayland and Xvfb).
pub fn screenshot(w: &tao::window::Window, path: &std::path::Path) -> std::io::Result<()> {
    use gtk::prelude::*;
    use tao::platform::unix::WindowExtUnix;
    let gw = w.gtk_window();
    let win = gw.window().ok_or_else(|| std::io::Error::other("window not realised"))?;
    let (ww, wh) = (win.width(), win.height());
    let pb = win.pixbuf(0, 0, ww, wh).ok_or_else(|| std::io::Error::other("could not read the window pixels"))?;
    pb.savev(path, "png", &[]).map_err(|e| std::io::Error::other(e.to_string()))
}

pub fn ui_base_url() -> &'static str {
    "ttlcd://localhost/"
}
