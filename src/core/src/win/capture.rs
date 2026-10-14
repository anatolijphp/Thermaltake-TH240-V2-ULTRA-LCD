//! Monitors (EnumDisplayMonitors order, like Screen.AllScreens) and GDI screen capture (BitBlt from the screen DC,
//! like Graphics.CopyFromScreen). The process is made per-monitor DPI aware so bounds are physical pixels.
use crate::capture::{Monitor, RectI};
use std::io;
use std::ptr::null_mut;
use windows_sys::Win32::Foundation::{LPARAM, RECT};
use windows_sys::Win32::Graphics::Gdi::*;

pub fn init_dpi() {
    use std::sync::Once;
    static ONCE: Once = Once::new();
    ONCE.call_once(|| unsafe {
        // per-monitor DPI aware v2 (-4)
        windows_sys::Win32::UI::HiDpi::SetProcessDpiAwarenessContext(-4isize as _);
    });
}

pub fn monitors() -> Vec<Monitor> {
    init_dpi();
    unsafe extern "system" fn cb(h: HMONITOR, _: HDC, _: *mut RECT, data: LPARAM) -> i32 {
        let list = &mut *(data as *mut Vec<Monitor>);
        let mut mi: MONITORINFOEXW = std::mem::zeroed();
        mi.monitorInfo.cbSize = std::mem::size_of::<MONITORINFOEXW>() as u32;
        if GetMonitorInfoW(h, &mut mi as *mut MONITORINFOEXW as *mut MONITORINFO) != 0 {
            let r = mi.monitorInfo.rcMonitor;
            list.push(Monitor {
                name: super::from_wide(&mi.szDevice),
                x: r.left,
                y: r.top,
                w: r.right - r.left,
                h: r.bottom - r.top,
                primary: mi.monitorInfo.dwFlags & 1 != 0, // MONITORINFOF_PRIMARY
            });
        }
        1
    }
    let mut list: Vec<Monitor> = Vec::new();
    unsafe { EnumDisplayMonitors(null_mut(), std::ptr::null(), Some(cb), &mut list as *mut _ as LPARAM) };
    list
}

pub struct Grabber {
    rect: RectI,
    mem: HDC,
    bmp: HBITMAP,
    old: HGDIOBJ,
    screen: HDC,
}

unsafe impl Send for Grabber {}

impl Grabber {
    pub fn new(rect: RectI) -> io::Result<Grabber> {
        init_dpi();
        unsafe {
            let screen = GetDC(null_mut());
            let mem = CreateCompatibleDC(screen);
            let bmp = CreateCompatibleBitmap(screen, rect.w, rect.h);
            if mem.is_null() || bmp.is_null() {
                return Err(io::Error::last_os_error());
            }
            let old = SelectObject(mem, bmp as HGDIOBJ);
            Ok(Grabber { rect, mem, bmp, old, screen })
        }
    }

    pub fn grab(&mut self) -> io::Result<crate::render::Picture> {
        let (w, h) = (self.rect.w, self.rect.h);
        unsafe {
            if BitBlt(self.mem, 0, 0, w, h, self.screen, self.rect.x, self.rect.y, SRCCOPY | CAPTUREBLT) == 0 {
                return Err(io::Error::last_os_error());
            }
            let mut bi: BITMAPINFO = std::mem::zeroed();
            bi.bmiHeader.biSize = std::mem::size_of::<BITMAPINFOHEADER>() as u32;
            bi.bmiHeader.biWidth = w;
            bi.bmiHeader.biHeight = -h; // top-down
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            let mut px = vec![0u8; (w * h * 4) as usize];
            if GetDIBits(self.mem, self.bmp, 0, h as u32, px.as_mut_ptr() as _, &mut bi, DIB_RGB_COLORS) == 0 {
                return Err(io::Error::last_os_error());
            }
            for p in px.chunks_exact_mut(4) {
                p.swap(0, 2); // BGRA -> RGBA
                p[3] = 255;
            }
            image::RgbaImage::from_raw(w as u32, h as u32, px).ok_or_else(|| io::Error::other("bad capture size"))
        }
    }
}

impl Drop for Grabber {
    fn drop(&mut self) {
        unsafe {
            SelectObject(self.mem, self.old);
            DeleteObject(self.bmp as HGDIOBJ);
            DeleteDC(self.mem);
            ReleaseDC(null_mut(), self.screen);
        }
    }
}
