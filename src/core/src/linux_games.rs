//! Game detection on Linux, from what the desktop already provides:
//!  - games started by Steam (also through Proton) or running under Wine/Lutris/Heroic: /proc (cmdline, exe, env)
//!  - the foreground window being full screen: X11 _NET_ACTIVE_WINDOW + _NET_WM_STATE_FULLSCREEN + _NET_WM_PID
//!    (libX11 is loaded at run time; under GNOME Wayland most games run through XWayland and show up here)
//!  - extra / ignored names from the settings
use crate::config::Config;
use crate::util::parse_names;
use std::sync::Mutex;
use std::time::{Duration, Instant};

struct Proc {
    pid: u32,
    name: String, // comm, or the exe's file name
    game_like: bool,
    wine: bool,
}

fn read(path: String) -> Vec<u8> {
    std::fs::read(path).unwrap_or_default()
}

/// Running processes of this user, with the signs of a game.
fn processes() -> Vec<Proc> {
    let me = std::process::id();
    let mut out = Vec::new();
    let Ok(dir) = std::fs::read_dir("/proc") else { return out };
    for e in dir.flatten() {
        let Ok(pid) = e.file_name().to_string_lossy().parse::<u32>() else { continue };
        if pid == me {
            continue;
        }
        let comm = String::from_utf8_lossy(&read(format!("/proc/{pid}/comm"))).trim().to_string();
        if comm.is_empty() {
            continue;
        }
        let cmdline = String::from_utf8_lossy(&read(format!("/proc/{pid}/cmdline"))).replace('\0', " ");
        let exe = std::fs::read_link(format!("/proc/{pid}/exe")).map(|p| p.to_string_lossy().into_owned()).unwrap_or_default();
        let env = read(format!("/proc/{pid}/environ")); // readable for our own processes only
        let has_env = |k: &str| {
            env.split(|b| *b == 0).any(|v| {
                let v = String::from_utf8_lossy(v);
                v.strip_prefix(k).and_then(|r| r.strip_prefix('=')).is_some_and(|val| !val.is_empty() && val != "0")
            })
        };
        let lower = comm.to_lowercase();
        // Steam's own client, its runtime and helpers carry the same markers as the games they start
        let launcher = ["steam", "reaper", "pressure-vessel", "pv-", "srt-", "steamwebhelper", "gameoverlayui", "wineserver", "lutris", "heroic", "python"]
            .iter()
            .any(|p| lower.starts_with(p));
        let steam_game = cmdline.contains("/steamapps/common/") || exe.contains("/steamapps/common/") || has_env("SteamGameId") || has_env("SteamAppId");
        let other_store = has_env("LUTRIS_GAME_UUID") || has_env("HEROIC_APP_NAME") || has_env("HEROIC_GAME_ID");
        let wine = lower.ends_with(".exe") || exe.ends_with("wine64-preloader") || exe.ends_with("wine-preloader");
        let name = if lower.ends_with(".exe") { comm[..comm.len() - 4].to_string() } else { comm.clone() };
        out.push(Proc { pid, name, game_like: !launcher && (steam_game || other_store), wine: !launcher && wine });
    }
    out
}

pub fn detect(c: &Config) -> Option<String> {
    let ignore = parse_names(&c.GameIgnore);
    let extra = parse_names(&c.GameExtra);
    let ignored = |n: &str| ignore.contains(&n.to_lowercase());
    let procs = processes();
    if let Some(p) = procs.iter().find(|p| extra.contains(&p.name.to_lowercase()) && !ignored(&p.name)) {
        return Some(p.name.clone());
    }
    if c.GameKnownList {
        if let Some(p) = procs.iter().find(|p| (p.game_like || p.wine) && !ignored(&p.name)) {
            return Some(p.name.clone());
        }
    }
    if c.GameFullscreen || c.GameBorderless {
        if let Some(pid) = fullscreen_window_pid() {
            if let Some(p) = procs.iter().find(|p| p.pid == pid) {
                let game = p.game_like || p.wine;
                if !ignored(&p.name) && (c.GameBorderless || game) {
                    return Some(p.name.clone());
                }
            }
        }
    }
    None
}

// ---------------- X11 ----------------

struct X {
    xlib: x11_dl::xlib::Xlib,
    dpy: *mut x11_dl::xlib::Display,
    active: u64,
    state: u64,
    fullscreen: u64,
    pid: u64,
}

// only the game-detection thread uses it
unsafe impl Send for X {}

static XCONN: Mutex<(Option<X>, Option<Instant>)> = Mutex::new((None, None));

/// PID of the foreground window's process when that window is full screen.
fn fullscreen_window_pid() -> Option<u32> {
    let mut g = XCONN.lock().unwrap();
    if g.0.is_none() {
        if g.1.is_some_and(|t| t.elapsed() < Duration::from_secs(30)) {
            return None; // no X server / libX11: try again later
        }
        g.1 = Some(Instant::now());
        g.0 = open_x();
    }
    let x = g.0.as_ref()?;
    unsafe {
        let root = (x.xlib.XDefaultRootWindow)(x.dpy);
        let win = prop_u64(x, root, x.active)?.first().copied().filter(|w| *w != 0)?;
        let states = prop_u64(x, win, x.state).unwrap_or_default();
        if !states.contains(&x.fullscreen) {
            return None;
        }
        prop_u64(x, win, x.pid)?.first().map(|p| *p as u32)
    }
}

fn open_x() -> Option<X> {
    std::env::var_os("DISPLAY")?;
    let xlib = x11_dl::xlib::Xlib::open().ok()?;
    unsafe {
        let dpy = (xlib.XOpenDisplay)(std::ptr::null());
        if dpy.is_null() {
            return None;
        }
        let atom = |n: &str| {
            let c = std::ffi::CString::new(n).unwrap();
            (xlib.XInternAtom)(dpy, c.as_ptr(), 0)
        };
        Some(X { active: atom("_NET_ACTIVE_WINDOW"), state: atom("_NET_WM_STATE"), fullscreen: atom("_NET_WM_STATE_FULLSCREEN"), pid: atom("_NET_WM_PID"), xlib, dpy })
    }
}

/// A 32-bit-format window property (atoms, windows, cardinals) as values.
unsafe fn prop_u64(x: &X, w: u64, prop: u64) -> Option<Vec<u64>> {
    use x11_dl::xlib;
    let mut ty: u64 = 0;
    let mut fmt: i32 = 0;
    let (mut n, mut after): (std::os::raw::c_ulong, std::os::raw::c_ulong) = (0, 0);
    let mut data: *mut u8 = std::ptr::null_mut();
    let r = (x.xlib.XGetWindowProperty)(x.dpy, w, prop, 0, 64, 0, xlib::AnyPropertyType as u64, &mut ty, &mut fmt, &mut n, &mut after, &mut data);
    if r != 0 || data.is_null() {
        return None;
    }
    // format 32 properties are returned as C longs
    let v = if fmt == 32 { std::slice::from_raw_parts(data as *const std::os::raw::c_ulong, n as usize).iter().map(|v| *v as u64).collect() } else { Vec::new() };
    (x.xlib.XFree)(data as *mut _);
    Some(v)
}
