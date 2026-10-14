//! Windows: single instance via the mutex "TTLcd.Tray.SingleInstance" + event "TTLcd.Tray.Show" (same names as the
//! C# Studio, so the old and the new Studio exclude each other), per-monitor DPI awareness, PrintWindow screenshot,
//! dark window frame, icons from the exe's resources and the built-in WebView2 loader (webview2_loader.rs).
//!
//! Extra helpers for main (Windows only, call them under `#[cfg(windows)]`):
//!   os::style_window(&window)        after the window is built: dark frame, rounded corners, dark border
//!   os::window_icon()                the app icon (exe resource 1) for WindowBuilder::with_window_icon
//!   os::tray_icon()                  the same icon at tray size for tray_icon::TrayIconBuilder::with_icon
//!   os::webview2_runtime_version()   None: no WebView2 runtime installed (show os::no_webview2_message() and quit)
//!   os::message_box(title, text)     a plain error box (no window needed)
#![allow(dead_code)] // helpers are used by main as it grows; Linux has no counterparts for some of them

#[cfg(not(target_arch = "x86"))]
#[path = "webview2_loader.rs"]
mod webview2_loader;

use std::ffi::c_void;
use std::io;
use std::path::Path;
use std::sync::atomic::{AtomicIsize, Ordering};
use windows_sys::Win32::Foundation::{CloseHandle, GetLastError, ERROR_ALREADY_EXISTS, HANDLE, HWND, RECT, WAIT_OBJECT_0};
use windows_sys::Win32::Graphics::Dwm::DwmSetWindowAttribute;
use windows_sys::Win32::Graphics::Gdi::{
    CreateCompatibleDC, CreateDIBSection, DeleteDC, DeleteObject, GdiFlush, GetDC, ReleaseDC, SelectObject, BITMAPINFO,
    BITMAPINFOHEADER, BI_RGB, DIB_RGB_COLORS,
};
use windows_sys::Win32::Storage::Xps::PrintWindow;
use windows_sys::Win32::System::LibraryLoader::{GetModuleHandleW, GetProcAddress};
use windows_sys::Win32::System::Threading::{
    CreateEventW, CreateMutexW, OpenEventW, ReleaseMutex, SetEvent, WaitForSingleObject, EVENT_MODIFY_STATE, INFINITE,
};
use windows_sys::Win32::UI::WindowsAndMessaging::{
    AllowSetForegroundWindow, GetClientRect, MessageBoxW, ASFW_ANY, MB_ICONERROR, MB_OK, MB_SETFOREGROUND,
};

const MUTEX_NAME: &str = "TTLcd.Tray.SingleInstance";
const SHOW_EVENT_NAME: &str = "TTLcd.Tray.Show";

/// The auto-reset event a later launch sets to bring this instance's window up (0 = not created).
static SHOW_EVENT: AtomicIsize = AtomicIsize::new(0);

fn wide(s: &str) -> Vec<u16> {
    s.encode_utf16().chain(std::iter::once(0)).collect()
}

/// Holds the single-instance mutex for the life of the process.
pub struct InstanceGuard {
    mutex: isize,
}

impl Drop for InstanceGuard {
    fn drop(&mut self) {
        if self.mutex != 0 {
            unsafe {
                ReleaseMutex(self.mutex as HANDLE);
                CloseHandle(self.mutex as HANDLE);
            }
        }
    }
}

/// Per-monitor v2 DPI awareness before any window exists (the manifest says the same; this covers a missing or
/// overridden manifest). Looked up at run time: the call is missing before Windows 10 1703.
fn init_dpi() {
    unsafe {
        let user32 = GetModuleHandleW(wide("user32.dll").as_ptr());
        if user32.is_null() {
            return;
        }
        if let Some(f) = GetProcAddress(user32, c"SetProcessDpiAwarenessContext".as_ptr().cast()) {
            let f: unsafe extern "system" fn(isize) -> i32 = std::mem::transmute(f);
            f(-4); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2; fails harmlessly when already set
        }
    }
}

/// Main calls this first. Also sets up the process (DPI awareness). None: another Studio (Rust or C#) runs and was
/// asked to show its window.
pub fn single_instance() -> Option<InstanceGuard> {
    init_dpi();
    unsafe {
        let name = wide(MUTEX_NAME);
        let m = CreateMutexW(std::ptr::null(), 1, name.as_ptr());
        if m.is_null() {
            return Some(InstanceGuard { mutex: 0 }); // cannot tell: run
        }
        if GetLastError() == ERROR_ALREADY_EXISTS {
            CloseHandle(m);
            let e = OpenEventW(EVENT_MODIFY_STATE, 0, wide(SHOW_EVENT_NAME).as_ptr());
            if !e.is_null() {
                // let the running instance take the foreground when it shows its window
                AllowSetForegroundWindow(ASFW_ANY);
                SetEvent(e);
                CloseHandle(e);
            }
            return None;
        }
        show_event();
        Some(InstanceGuard { mutex: m as isize })
    }
}

/// The show event, created on first use (auto-reset, like the C# EventWaitHandle).
fn show_event() -> isize {
    let e = SHOW_EVENT.load(Ordering::Acquire);
    if e != 0 {
        return e;
    }
    let h = unsafe { CreateEventW(std::ptr::null(), 0, 0, wide(SHOW_EVENT_NAME).as_ptr()) } as isize;
    match SHOW_EVENT.compare_exchange(0, h, Ordering::AcqRel, Ordering::Acquire) {
        Ok(_) => h,
        Err(other) => {
            if h != 0 {
                unsafe { CloseHandle(h as HANDLE) };
            }
            other
        }
    }
}

/// Calls `show` (on a background thread) each time a later launch asks this instance to show its window.
pub fn listen_show(show: Box<dyn Fn() + Send>) {
    let e = show_event();
    if e == 0 {
        return;
    }
    let _ = std::thread::Builder::new().name("show-listener".into()).spawn(move || loop {
        if unsafe { WaitForSingleObject(e as HANDLE, INFINITE) } != WAIT_OBJECT_0 {
            break;
        }
        show();
    });
}

pub fn before_start() -> Option<String> {
    None
}

pub fn after_exit() {}

pub fn ui_base_url() -> &'static str {
    "http://ttlcd.localhost/"
}

fn hwnd_of(w: &tao::window::Window) -> HWND {
    use tao::platform::windows::WindowExtWindows;
    w.hwnd() as HWND
}

/// Writes the window's client area (including the WebView2 content, which DirectComposition draws) as a PNG.
/// The window must be shown and not minimised; it may be placed off-screen.
pub fn screenshot(w: &tao::window::Window, path: &Path) -> io::Result<()> {
    const PW_CLIENTONLY: u32 = 1;
    const PW_RENDERFULLCONTENT: u32 = 2;
    let hwnd = hwnd_of(w);
    let mut rc = RECT { left: 0, top: 0, right: 0, bottom: 0 };
    unsafe { GetClientRect(hwnd, &mut rc) };
    let (width, height) = (rc.right - rc.left, rc.bottom - rc.top);
    if width <= 0 || height <= 0 {
        return Err(io::Error::other("the window has no client area (minimised?)"));
    }
    let mut rgb = vec![0u8; width as usize * height as usize * 3];
    unsafe {
        let screen = GetDC(std::ptr::null_mut());
        let mem = CreateCompatibleDC(screen);
        let mut bmi: BITMAPINFO = std::mem::zeroed();
        bmi.bmiHeader = BITMAPINFOHEADER {
            biSize: std::mem::size_of::<BITMAPINFOHEADER>() as u32,
            biWidth: width,
            biHeight: -height, // top-down rows
            biPlanes: 1,
            biBitCount: 32,
            biCompression: BI_RGB,
            ..std::mem::zeroed()
        };
        let mut bits: *mut c_void = std::ptr::null_mut();
        let bmp = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &mut bits, std::ptr::null_mut(), 0);
        let ok = if bmp.is_null() || bits.is_null() {
            false
        } else {
            let old = SelectObject(mem, bmp);
            let ok = PrintWindow(hwnd, mem, PW_CLIENTONLY | PW_RENDERFULLCONTENT) != 0;
            GdiFlush();
            if ok {
                let src = std::slice::from_raw_parts(bits as *const u8, rgb.len() / 3 * 4);
                for (d, s) in rgb.chunks_exact_mut(3).zip(src.chunks_exact(4)) {
                    d[0] = s[2];
                    d[1] = s[1];
                    d[2] = s[0];
                }
            }
            SelectObject(mem, old);
            ok
        };
        if !bmp.is_null() {
            DeleteObject(bmp);
        }
        DeleteDC(mem);
        ReleaseDC(std::ptr::null_mut(), screen);
        if !ok {
            return Err(io::Error::other("PrintWindow failed"));
        }
    }
    let file = io::BufWriter::new(std::fs::File::create(path)?);
    let mut enc = png::Encoder::new(file, width as u32, height as u32);
    enc.set_color(png::ColorType::Rgb);
    enc.set_depth(png::BitDepth::Eight);
    let mut wr = enc.write_header().map_err(io::Error::other)?;
    wr.write_image_data(&rgb).map_err(io::Error::other)?;
    wr.finish().map_err(io::Error::other)
}

/// Dark frame for the (frameless) Studio window: dark mode for the system parts, Windows 11 rounded corners and a
/// border in the UI's line colour. Each attribute is ignored by Windows versions that lack it.
pub fn style_window(w: &tao::window::Window) {
    const DWMWA_USE_IMMERSIVE_DARK_MODE: u32 = 20;
    const DWMWA_WINDOW_CORNER_PREFERENCE: u32 = 33;
    const DWMWA_BORDER_COLOR: u32 = 34;
    const DWMWA_CAPTION_COLOR: u32 = 35;
    const DWMWCP_ROUND: u32 = 2;
    let hwnd = hwnd_of(w);
    let set = |attr: u32, v: u32| unsafe {
        DwmSetWindowAttribute(hwnd, attr, &v as *const u32 as *const c_void, 4);
    };
    set(DWMWA_USE_IMMERSIVE_DARK_MODE, 1);
    set(DWMWA_WINDOW_CORNER_PREFERENCE, DWMWCP_ROUND);
    set(DWMWA_BORDER_COLOR, 0x002C_2929); // COLORREF 0x00BBGGRR of #29292C
    set(DWMWA_CAPTION_COLOR, 0x000E_0D0D); // #0D0D0E
}

/// The app icon embedded in the exe (resource 1, from src/app.ico) for the window and taskbar.
pub fn window_icon() -> Option<tao::window::Icon> {
    use tao::platform::windows::IconExtWindows;
    tao::window::Icon::from_resource(1, None).ok()
}

/// The app icon at the tray's small-icon size.
pub fn tray_icon() -> Option<tray_icon::Icon> {
    let px = unsafe { windows_sys::Win32::UI::WindowsAndMessaging::GetSystemMetrics(49) }; // SM_CXSMICON
    let px = if px > 0 { px as u32 } else { 16 };
    tray_icon::Icon::from_resource(1, Some((px, px))).ok()
}

/// The installed WebView2 runtime's version; None when it is missing and the window cannot be created.
pub fn webview2_runtime_version() -> Option<String> {
    #[cfg(not(target_arch = "x86"))]
    {
        webview2_loader::runtime_version()
    }
    #[cfg(target_arch = "x86")]
    {
        Some(String::new())
    }
}

pub fn no_webview2_message() -> &'static str {
    "TT LCD Studio needs the Microsoft Edge WebView2 Runtime, which is part of Windows 10 and 11 but is missing \
     here.\n\nInstall it from https://go.microsoft.com/fwlink/p/?LinkId=2124703 and start TT LCD Studio again."
}

/// A modal error box without an owner window.
pub fn message_box(title: &str, text: &str) {
    unsafe {
        MessageBoxW(
            std::ptr::null_mut(),
            wide(text).as_ptr(),
            wide(title).as_ptr(),
            MB_OK | MB_ICONERROR | MB_SETFOREGROUND,
        );
    }
}
