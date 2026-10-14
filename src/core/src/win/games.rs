//! Game detection, using only what Windows provides:
//!  - exclusive full-screen Direct3D apps: SHQueryUserNotificationState == QUNS_RUNNING_D3D_FULL_SCREEN (the signal
//!    Windows uses to hold back notifications during games)
//!  - games the Xbox Game Bar knows: HKCU\System\GameConfigStore\Children\*\MatchedExeFullPath; any of them running
//!    counts (covers borderless/windowed games too)
//!  - optionally any foreground window that covers its whole monitor (also full-screen video)
//!  - extra exe names from the settings
use super::{reg, wide};
use crate::config::Config;
use crate::util::parse_names;
use std::collections::HashMap;
use std::sync::Mutex;
use std::time::{Duration, Instant};
use windows_sys::Win32::Foundation::*;
use windows_sys::Win32::Graphics::Gdi::*;
use windows_sys::Win32::System::Diagnostics::ToolHelp::*;
use windows_sys::Win32::System::Threading::*;
use windows_sys::Win32::UI::Shell::SHQueryUserNotificationState;
use windows_sys::Win32::UI::WindowsAndMessaging::*;

pub fn detect(c: &Config) -> Option<String> {
    let ignore = parse_names(&c.GameIgnore);
    if c.GameFullscreen {
        let mut st = 0;
        if unsafe { SHQueryUserNotificationState(&mut st) } == 0 && st == 3 {
            // QUNS_RUNNING_D3D_FULL_SCREEN
            let fg = foreground_name();
            match fg {
                Some(n) if ignore.contains(&n.to_lowercase()) => {}
                Some(n) => return Some(n),
                None => return Some("full-screen game".into()),
            }
        }
    }
    if c.GameBorderless {
        if let Some(n) = full_screen_foreground() {
            if !ignore.contains(&n.to_lowercase()) {
                return Some(n);
            }
        }
    }
    let extra = parse_names(&c.GameExtra);
    let known = if c.GameKnownList { known_games() } else { HashMap::new() };
    if extra.is_empty() && known.is_empty() {
        return None;
    }
    let me = unsafe { GetCurrentProcessId() };
    for (pid, exe) in processes() {
        if pid == me || pid == 0 {
            continue;
        }
        let name = exe.strip_suffix(".exe").or_else(|| exe.strip_suffix(".EXE")).unwrap_or(&exe).to_string();
        let lower = name.to_lowercase();
        if ignore.contains(&lower) {
            continue;
        }
        if extra.contains(&lower) {
            return Some(name);
        }
        if let Some(paths) = known.get(&lower) {
            match image_path(pid) {
                Some(p) if !paths.iter().any(|x| x.eq_ignore_ascii_case(&p)) => {}
                _ => return Some(name),
            }
        }
    }
    None
}

fn processes() -> Vec<(u32, String)> {
    let mut out = Vec::new();
    unsafe {
        let snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if snap == INVALID_HANDLE_VALUE {
            return out;
        }
        let mut e: PROCESSENTRY32W = std::mem::zeroed();
        e.dwSize = std::mem::size_of::<PROCESSENTRY32W>() as u32;
        let mut ok = Process32FirstW(snap, &mut e);
        while ok != 0 {
            out.push((e.th32ProcessID, super::from_wide(&e.szExeFile)));
            ok = Process32NextW(snap, &mut e);
        }
        CloseHandle(snap);
    }
    out
}

/// exe name (lower, no extension) -> full paths the Game Bar has recorded as games; cached 5 minutes
fn known_games() -> HashMap<String, Vec<String>> {
    static CACHE: Mutex<Option<(Instant, HashMap<String, Vec<String>>)>> = Mutex::new(None);
    let mut c = CACHE.lock().unwrap();
    if let Some((at, m)) = c.as_ref() {
        if at.elapsed() < Duration::from_secs(300) {
            return m.clone();
        }
    }
    let mut d: HashMap<String, Vec<String>> = HashMap::new();
    let root = "System\\GameConfigStore\\Children";
    for sub in reg::subkeys(reg::HKCU, root) {
        if let Some(path) = reg::get_string(reg::HKCU, &format!("{root}\\{sub}"), "MatchedExeFullPath") {
            if path.is_empty() {
                continue;
            }
            let name = std::path::Path::new(&path).file_stem().map(|s| s.to_string_lossy().to_lowercase()).unwrap_or_default();
            d.entry(name).or_default().push(path);
        }
    }
    *c = Some((Instant::now(), d.clone()));
    d
}

pub fn known_game_count() -> i32 {
    known_games().len() as i32
}

fn image_path(pid: u32) -> Option<String> {
    unsafe {
        let h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, pid);
        if h.is_null() {
            return None;
        }
        let mut buf = [0u16; 1024];
        let mut n = buf.len() as u32;
        let ok = QueryFullProcessImageNameW(h, 0, buf.as_mut_ptr(), &mut n);
        CloseHandle(h);
        (ok != 0).then(|| String::from_utf16_lossy(&buf[..n as usize]))
    }
}

fn foreground_name() -> Option<String> {
    let mut pid = 0;
    unsafe { GetWindowThreadProcessId(GetForegroundWindow(), &mut pid) };
    let p = image_path(pid)?;
    std::path::Path::new(&p).file_stem().map(|s| s.to_string_lossy().into_owned())
}

// foreground window that exactly covers its monitor (borderless full-screen), excluding the desktop/taskbar
fn full_screen_foreground() -> Option<String> {
    unsafe {
        let hwnd = GetForegroundWindow();
        if hwnd.is_null() {
            return None;
        }
        let mut cls = [0u16; 64];
        let n = GetClassNameW(hwnd, cls.as_mut_ptr(), cls.len() as i32);
        let c = String::from_utf16_lossy(&cls[..n.max(0) as usize]);
        if matches!(c.as_str(), "Progman" | "WorkerW" | "Shell_TrayWnd" | "Shell_SecondaryTrayWnd") {
            return None;
        }
        let mut pid = 0;
        GetWindowThreadProcessId(hwnd, &mut pid);
        if pid == GetCurrentProcessId() {
            return None;
        }
        let mut r: RECT = std::mem::zeroed();
        if GetWindowRect(hwnd, &mut r) == 0 {
            return None;
        }
        let mut mi: MONITORINFO = std::mem::zeroed();
        mi.cbSize = std::mem::size_of::<MONITORINFO>() as u32;
        if GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mut mi) == 0 {
            return None;
        }
        let m = mi.rcMonitor;
        let covers = r.left <= m.left && r.top <= m.top && r.right >= m.right && r.bottom >= m.bottom;
        if covers { foreground_name() } else { None }
    }
}

#[allow(dead_code)]
fn _w() -> Vec<u16> {
    wide("")
}
