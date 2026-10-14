//! The install window: plain Win32 in the app's dark style (port of Setup.cs SetupForm). Laid out once at the system
//! DPI (the setup's manifest is system DPI aware).
use super::install::{self, Options};
use super::{wide, APP_NAME, VERSION};
use std::cell::RefCell;
use std::path::PathBuf;
use windows_sys::Win32::Foundation::{COLORREF, HWND, LPARAM, LRESULT, RECT, WPARAM};
use windows_sys::Win32::Graphics::Dwm::DwmSetWindowAttribute;
use windows_sys::Win32::Graphics::Gdi::{
    BeginPaint, CreateFontW, CreateSolidBrush, DeleteObject, DrawFocusRect, DrawTextW, EndPaint, FillRect, FrameRect, GetDC,
    GetDeviceCaps, GetStockObject, InvalidateRect, ReleaseDC, SelectObject, SetBkColor, SetBkMode, SetDCBrushColor,
    SetTextColor, CLEARTYPE_QUALITY, CLIP_DEFAULT_PRECIS, DC_BRUSH, DEFAULT_CHARSET, DEFAULT_PITCH, DT_CENTER,
    DT_END_ELLIPSIS, DT_LEFT, DT_NOPREFIX, DT_SINGLELINE, DT_VCENTER, DT_WORDBREAK, FF_DONTCARE, FW_NORMAL,
    FW_SEMIBOLD, HBRUSH, HDC, HFONT, LOGPIXELSY, OUT_DEFAULT_PRECIS, PAINTSTRUCT, TRANSPARENT,
};
use windows_sys::Win32::System::LibraryLoader::GetModuleHandleW;
use windows_sys::Win32::UI::Controls::{
    SetWindowTheme, BST_CHECKED, DRAWITEMSTRUCT, ODS_DISABLED, ODS_FOCUS, ODS_SELECTED,
};
use windows_sys::Win32::UI::Input::KeyboardAndMouse::{EnableWindow, IsWindowEnabled, SetFocus};
use windows_sys::Win32::UI::WindowsAndMessaging::{
    AdjustWindowRectEx, CreateWindowExW, DefWindowProcW, DestroyWindow, DispatchMessageW, GetMessageW,
    GetWindowTextLengthW, GetWindowTextW, IsDialogMessageW, KillTimer, LoadCursorW, LoadImageW,
    PostMessageW, PostQuitMessage, RegisterClassExW, SendMessageW, SetCursor, SetTimer, SetWindowTextW, ShowWindow,
    SystemParametersInfoW, TranslateMessage, BM_GETCHECK, BM_SETCHECK, BS_AUTOCHECKBOX, BS_OWNERDRAW, CS_HREDRAW,
    CS_VREDRAW, ES_AUTOHSCROLL, HMENU, IDC_ARROW, IDC_HAND, IMAGE_ICON, LR_DEFAULTCOLOR, MSG, SPI_GETWORKAREA,
    SW_SHOW, WM_APP, WM_CLOSE, WM_COMMAND, WM_CTLCOLORBTN, WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, WM_DESTROY,
    WM_DRAWITEM, WM_PAINT, WM_SETCURSOR, WM_SETFONT, WM_TIMER, WNDCLASSEXW, WS_CAPTION, WS_CHILD, WS_MINIMIZEBOX,
    WS_OVERLAPPED, WS_SYSMENU, WS_TABSTOP, WS_VISIBLE,
};

const fn rgb(r: u8, g: u8, b: u8) -> COLORREF {
    r as u32 | (g as u32) << 8 | (b as u32) << 16
}
const BG: COLORREF = rgb(13, 13, 14);
const CARD: COLORREF = rgb(27, 27, 29);
const LINE: COLORREF = rgb(41, 41, 44);
const FG: COLORREF = rgb(237, 237, 237);
const DIM: COLORREF = rgb(143, 143, 148);
const ACCENT: COLORREF = rgb(255, 31, 75);
const ACCENT_DOWN: COLORREF = rgb(214, 22, 60);
const ACCENT_OFF: COLORREF = rgb(134, 22, 44);
const GREEN: COLORREF = rgb(43, 227, 107);
const WHITE: COLORREF = rgb(255, 255, 255);
/// The RGB line under the title: colour stops at 0, 25, 50, 75 and 100 %.
const RGB_STOPS: [(u8, u8, u8); 5] = [(255, 31, 75), (255, 230, 0), (0, 225, 255), (122, 75, 255), (255, 43, 214)];

const ID_INSTALL: i32 = 1; // IDOK: Enter (through IsDialogMessage) presses it
const ID_CANCEL: i32 = 2; // IDCANCEL: Escape
const ID_BROWSE: i32 = 100;
const ID_EDIT: i32 = 101;
const ID_CHECK: i32 = 110;
const WM_STATUS: u32 = WM_APP + 1;
const WM_DONE: u32 = WM_APP + 2;
const TIMER_BAR: usize = 1;

/// Option checkboxes in the order of the C# setup: label, default.
const CHECKS: [(&str, bool); 7] = [
    ("Start menu shortcut", true),
    ("Desktop shortcut", true),
    ("Start with Windows (in the tray)", true),
    ("Launch TT LCD Studio when done", true),
    ("CPU temperature service (needs TOWEI; administrator)", false),
    ("Armoury Crate / Aura Sync plugin (administrator)", false),
    ("Virtual 480×480 display for Wallpaper Engine (driver; administrator)", false),
];

// layout in 96-DPI pixels (client area 560 x 470, as in the C# setup)
const CLIENT: (i32, i32) = (560, 470);
const DIR_BOX: (i32, i32, i32, i32) = (24, 126, 420, 28);
const BROWSE: (i32, i32, i32, i32) = (452, 126, 84, 28);
const CHECK_Y: i32 = 192;
const CHECK_H: i32 = 26;
const BAR: (i32, i32, i32, i32) = (24, 392, 512, 4);
const STATUS: (i32, i32, i32, i32) = (24, 402, 400, 40);
const INSTALL: (i32, i32, i32, i32) = (436, 418, 100, 34);

struct Ui {
    edit: HWND,
    browse: HWND,
    install: HWND,
    checks: Vec<HWND>,
    f_ui: HFONT,
    f_title: HFONT,
    f_label: HFONT,
    br_bg: HBRUSH,
    br_card: HBRUSH,
    scale: f32,
    status: String,
    status_color: COLORREF,
    busy: bool,
    done: bool,
    anim: i32,
}

thread_local! {
    static UI: RefCell<Option<Ui>> = const { RefCell::new(None) };
}

/// Runs `f` on the window state; None before the window exists. Never call anything that sends window messages
/// inside `f` (the window procedure would find the state borrowed).
fn with<R>(f: impl FnOnce(&mut Ui) -> R) -> Option<R> {
    UI.with(|u| u.try_borrow_mut().ok().and_then(|mut u| u.as_mut().map(f)))
}

impl Ui {
    fn px(&self, v: i32) -> i32 {
        (v as f32 * self.scale).round() as i32
    }
    fn rect(&self, r: (i32, i32, i32, i32)) -> RECT {
        RECT { left: self.px(r.0), top: self.px(r.1), right: self.px(r.0 + r.2), bottom: self.px(r.1 + r.3) }
    }
    fn controls(&self) -> Vec<HWND> {
        let mut v = vec![self.edit, self.browse, self.install];
        v.extend(&self.checks);
        v
    }
}

fn font(dpi: i32, pt: f32, weight: i32) -> HFONT {
    unsafe {
        CreateFontW(
            -((pt * dpi as f32 / 72.0).round() as i32),
            0,
            0,
            0,
            weight,
            0,
            0,
            0,
            DEFAULT_CHARSET as u32,
            OUT_DEFAULT_PRECIS as u32,
            CLIP_DEFAULT_PRECIS as u32,
            CLEARTYPE_QUALITY as u32,
            (DEFAULT_PITCH | FF_DONTCARE) as u32,
            wide("Segoe UI").as_ptr(),
        )
    }
}

pub fn run() -> Result<i32, String> {
    install::com_init();
    unsafe {
        let hinst = GetModuleHandleW(std::ptr::null());
        let screen = GetDC(std::ptr::null_mut());
        let dpi = GetDeviceCaps(screen, LOGPIXELSY as i32).max(96);
        ReleaseDC(std::ptr::null_mut(), screen);
        let scale = dpi as f32 / 96.0;
        let br_bg = CreateSolidBrush(BG);
        let br_card = CreateSolidBrush(CARD);
        let icon = |size: i32| LoadImageW(hinst, 1 as _, IMAGE_ICON, size, size, LR_DEFAULTCOLOR);
        let class = wide("TTLcdStudioSetup");
        let wc = WNDCLASSEXW {
            cbSize: std::mem::size_of::<WNDCLASSEXW>() as u32,
            style: CS_HREDRAW | CS_VREDRAW,
            lpfnWndProc: Some(wndproc),
            cbClsExtra: 0,
            cbWndExtra: 0,
            hInstance: hinst,
            hIcon: icon((32.0 * scale) as i32),
            hCursor: LoadCursorW(std::ptr::null_mut(), IDC_ARROW),
            hbrBackground: br_bg,
            lpszMenuName: std::ptr::null(),
            lpszClassName: class.as_ptr(),
            hIconSm: icon((16.0 * scale) as i32),
        };
        if RegisterClassExW(&wc) == 0 {
            return Err("cannot register the setup window class".into());
        }
        let style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        let px = |v: i32| (v as f32 * scale).round() as i32;
        let mut rc = RECT { left: 0, top: 0, right: px(CLIENT.0), bottom: px(CLIENT.1) };
        AdjustWindowRectEx(&mut rc, style, 0, 0);
        let (w, h) = (rc.right - rc.left, rc.bottom - rc.top);
        let mut work = RECT { left: 0, top: 0, right: 1024, bottom: 768 };
        SystemParametersInfoW(SPI_GETWORKAREA, 0, (&mut work as *mut RECT).cast(), 0);
        let x = work.left + ((work.right - work.left - w) / 2).max(0);
        let y = work.top + ((work.bottom - work.top - h) / 2).max(0);
        let title = wide(format!("{APP_NAME} setup"));
        let hwnd = CreateWindowExW(
            0,
            class.as_ptr(),
            title.as_ptr(),
            style,
            x,
            y,
            w,
            h,
            std::ptr::null_mut(),
            std::ptr::null_mut(),
            hinst,
            std::ptr::null(),
        );
        if hwnd.is_null() {
            return Err("cannot create the setup window".into());
        }
        // dark title bar (attribute 20 since Windows 10 20H1, 19 before)
        let on: u32 = 1;
        if DwmSetWindowAttribute(hwnd, 20, (&on as *const u32).cast(), 4) < 0 {
            DwmSetWindowAttribute(hwnd, 19, (&on as *const u32).cast(), 4);
        }

        let f_ui = font(dpi, 9.5, FW_NORMAL as i32);
        let child = |class: &str, text: &str, style: u32, r: (i32, i32, i32, i32), id: i32| {
            let h = CreateWindowExW(
                0,
                wide(class).as_ptr(),
                wide(text).as_ptr(),
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | style,
                px(r.0),
                px(r.1),
                px(r.2),
                px(r.3),
                hwnd,
                id as HMENU,
                hinst,
                std::ptr::null(),
            );
            SendMessageW(h, WM_SETFONT, f_ui as WPARAM, 0);
            h
        };
        // the edit sits borderless inside the painted dir box, vertically centred
        let edit_h = px(18);
        let edit = CreateWindowExW(
            0,
            wide("EDIT").as_ptr(),
            wide(install::default_dir()).as_ptr(),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL as u32,
            px(DIR_BOX.0 + 7),
            px(DIR_BOX.1) + (px(DIR_BOX.3) - edit_h) / 2,
            px(DIR_BOX.2 - 14),
            edit_h,
            hwnd,
            ID_EDIT as HMENU,
            hinst,
            std::ptr::null(),
        );
        SendMessageW(edit, WM_SETFONT, f_ui as WPARAM, 0);
        let browse = child("BUTTON", "Browse…", BS_OWNERDRAW as u32, BROWSE, ID_BROWSE);
        let mut checks = Vec::new();
        for (i, (text, on)) in CHECKS.iter().enumerate() {
            let r = (24, CHECK_Y + i as i32 * CHECK_H, 512, CHECK_H);
            let c = child("BUTTON", text, BS_AUTOCHECKBOX as u32, r, ID_CHECK + i as i32);
            // classic drawing, so the WM_CTLCOLOR* text colour applies
            SetWindowTheme(c, wide("").as_ptr(), wide("").as_ptr());
            if *on {
                SendMessageW(c, BM_SETCHECK, BST_CHECKED as WPARAM, 0);
            }
            checks.push(c);
        }
        let install = child("BUTTON", "Install", BS_OWNERDRAW as u32, INSTALL, ID_INSTALL);

        UI.with(|u| {
            *u.borrow_mut() = Some(Ui {
                edit,
                browse,
                install,
                checks,
                f_ui,
                f_title: font(dpi, 16.0, FW_SEMIBOLD as i32),
                f_label: font(dpi, 8.5, FW_SEMIBOLD as i32),
                br_bg,
                br_card,
                scale,
                status: String::new(),
                status_color: DIM,
                busy: false,
                done: false,
                anim: 0,
            })
        });
        ShowWindow(hwnd, SW_SHOW);
        SetFocus(install);

        let mut msg: MSG = std::mem::zeroed();
        while GetMessageW(&mut msg, std::ptr::null_mut(), 0, 0) > 0 {
            if IsDialogMessageW(hwnd, &msg) == 0 {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if let Some(u) = UI.with(|u| u.borrow_mut().take()) {
            for o in [u.f_ui, u.f_title, u.f_label] {
                DeleteObject(o);
            }
            DeleteObject(u.br_bg);
            DeleteObject(u.br_card);
        }
    }
    Ok(0)
}

fn text(hdc: HDC, f: HFONT, color: COLORREF, mut r: RECT, s: &str, flags: u32) {
    let w: Vec<u16> = s.encode_utf16().collect();
    unsafe {
        SelectObject(hdc, f);
        SetTextColor(hdc, color);
        DrawTextW(hdc, w.as_ptr(), w.len() as i32, &mut r, flags | DT_NOPREFIX);
    }
}

fn fill(hdc: HDC, r: &RECT, color: COLORREF) {
    unsafe {
        SetDCBrushColor(hdc, color);
        FillRect(hdc, r, GetStockObject(DC_BRUSH));
    }
}

fn frame(hdc: HDC, r: &RECT, color: COLORREF) {
    unsafe {
        SetDCBrushColor(hdc, color);
        FrameRect(hdc, r, GetStockObject(DC_BRUSH));
    }
}

fn gradient_at(t: f32) -> COLORREF {
    let t = t.clamp(0.0, 1.0) * 4.0;
    let i = (t.floor() as usize).min(3);
    let f = t - i as f32;
    let (a, b) = (RGB_STOPS[i], RGB_STOPS[i + 1]);
    let mix = |x: u8, y: u8| (x as f32 + (y as f32 - x as f32) * f).round() as u8;
    rgb(mix(a.0, b.0), mix(a.1, b.1), mix(a.2, b.2))
}

fn paint(u: &Ui, hdc: HDC) {
    unsafe { SetBkMode(hdc, TRANSPARENT as i32) };
    let single = DT_LEFT | DT_SINGLELINE;
    text(hdc, u.f_title, FG, u.rect((24, 20, 512, 36)), "TT  LCD STUDIO", single);
    let line = u.rect((24, 58, 512, 2));
    let width = (line.right - line.left).max(1);
    for x in 0..width {
        let col = RECT { left: line.left + x, top: line.top, right: line.left + x + 1, bottom: line.bottom };
        fill(hdc, &col, gradient_at(x as f32 / (width - 1).max(1) as f32));
    }
    let sub = format!("Control the LCD of your Thermaltake TH240 V2 Ultra cooler.  Version {VERSION}");
    text(hdc, u.f_ui, DIM, u.rect((24, 70, 512, 22)), &sub, single | DT_END_ELLIPSIS);
    text(hdc, u.f_label, DIM, u.rect((24, 106, 300, 18)), "INSTALL TO", single);
    let dir = u.rect(DIR_BOX);
    fill(hdc, &dir, CARD);
    frame(hdc, &dir, LINE);
    text(hdc, u.f_label, DIM, u.rect((24, 170, 300, 18)), "OPTIONS", single);
    if u.busy {
        // indeterminate progress: an accent segment sweeping along the track
        let track = u.rect(BAR);
        fill(hdc, &track, LINE);
        let tw = track.right - track.left;
        let seg = tw / 4;
        let pos = u.anim.rem_euclid(tw + seg) - seg;
        let r = RECT {
            left: track.left + pos.max(0),
            top: track.top,
            right: track.left + (pos + seg).min(tw),
            bottom: track.bottom,
        };
        if r.right > r.left {
            fill(hdc, &r, ACCENT);
        }
    }
    text(hdc, u.f_ui, u.status_color, u.rect(STATUS), &u.status, DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS);
}

fn draw_button(d: &DRAWITEMSTRUCT, f: HFONT) {
    let accent = d.CtlID as i32 == ID_INSTALL;
    let disabled = d.itemState & ODS_DISABLED != 0;
    let pressed = d.itemState & ODS_SELECTED != 0;
    let (bg, border, fg) = if accent {
        let bg = if disabled { ACCENT_OFF } else if pressed { ACCENT_DOWN } else { ACCENT };
        (bg, bg, if disabled { DIM } else { WHITE })
    } else {
        (if pressed { LINE } else { CARD }, LINE, if disabled { DIM } else { FG })
    };
    let hdc = d.hDC;
    let r = d.rcItem;
    fill(hdc, &r, bg);
    frame(hdc, &r, border);
    let len = unsafe { GetWindowTextLengthW(d.hwndItem) }.max(0) as usize;
    let mut buf = vec![0u16; len + 1];
    let n = unsafe { GetWindowTextW(d.hwndItem, buf.as_mut_ptr(), buf.len() as i32) }.max(0) as usize;
    unsafe { SetBkMode(hdc, TRANSPARENT as i32) };
    text(hdc, f, fg, r, &String::from_utf16_lossy(&buf[..n]), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if d.itemState & ODS_FOCUS != 0 {
        let fr = RECT { left: r.left + 3, top: r.top + 3, right: r.right - 3, bottom: r.bottom - 3 };
        unsafe {
            SetTextColor(hdc, fg);
            SetBkColor(hdc, bg);
            DrawFocusRect(hdc, &fr);
        }
    }
}

fn window_text(h: HWND) -> String {
    unsafe {
        let len = GetWindowTextLengthW(h).max(0) as usize;
        let mut buf = vec![0u16; len + 1];
        let n = GetWindowTextW(h, buf.as_mut_ptr(), buf.len() as i32).max(0) as usize;
        String::from_utf16_lossy(&buf[..n])
    }
}

fn start_install(hwnd: HWND) {
    let Some((edit, checks, controls)) = with(|u| (u.edit, u.checks.clone(), u.controls())) else { return };
    let checked = |i: usize| unsafe { SendMessageW(checks[i], BM_GETCHECK, 0, 0) } == BST_CHECKED as LRESULT;
    let dir = window_text(edit).trim().trim_matches('"').to_string();
    if dir.is_empty() {
        return;
    }
    let o = Options {
        dir: PathBuf::from(dir),
        start_menu: checked(0),
        desktop: checked(1),
        autostart: checked(2),
        launch: checked(3),
        sensor_service: checked(4),
        aura: checked(5),
        virtual_display: checked(6),
    };
    for c in controls {
        unsafe { EnableWindow(c, 0) };
    }
    let bar = with(|u| {
        u.busy = true;
        u.status.clear();
        u.status_color = DIM;
        (u.rect(BAR), u.rect(STATUS))
    });
    if let Some((b, s)) = bar {
        unsafe {
            InvalidateRect(hwnd, &b, 1);
            InvalidateRect(hwnd, &s, 1);
            SetTimer(hwnd, TIMER_BAR, 30, None);
        }
    }
    let h = hwnd as isize;
    std::thread::spawn(move || {
        install::com_init();
        let post_status = |s: &str| post(h, WM_STATUS, Box::new(s.to_string()));
        let r = install::install(&o, &post_status);
        post(h, WM_DONE, Box::new(r.err()));
    });
}

/// Hands a boxed value to the window thread (it frees it).
fn post<T>(hwnd: isize, msg: u32, v: Box<T>) {
    let p = Box::into_raw(v);
    if unsafe { PostMessageW(hwnd as HWND, msg, 0, p as LPARAM) } == 0 {
        drop(unsafe { Box::from_raw(p) });
    }
}

fn set_status(hwnd: HWND, s: String, color: COLORREF) {
    if let Some(r) = with(|u| {
        u.status = s;
        u.status_color = color;
        u.rect(STATUS)
    }) {
        unsafe { InvalidateRect(hwnd, &r, 1) };
    }
}

fn finish_install(hwnd: HWND, err: Option<String>) {
    let Some((install, controls, bar)) = with(|u| {
        u.busy = false;
        u.done = err.is_none();
        (u.install, u.controls(), u.rect(BAR))
    }) else {
        return;
    };
    unsafe {
        KillTimer(hwnd, TIMER_BAR);
        InvalidateRect(hwnd, &bar, 1);
    }
    match err {
        Some(e) => {
            set_status(hwnd, format!("Failed: {e}"), ACCENT);
            for c in controls {
                unsafe { EnableWindow(c, 1) };
            }
        }
        None => {
            set_status(hwnd, "Done. TT LCD Studio is installed.".into(), GREEN);
            unsafe {
                SetWindowTextW(install, wide("Close").as_ptr());
                EnableWindow(install, 1);
                SetFocus(install);
            }
        }
    }
}

fn browse(hwnd: HWND) {
    let Some(edit) = with(|u| u.edit) else { return };
    if let Some(picked) = pick_folder(hwnd, &window_text(edit)) {
        let p = PathBuf::from(&picked);
        let ends_with_app = p.file_name().is_some_and(|n| n.to_string_lossy().eq_ignore_ascii_case(APP_NAME));
        let dir = if ends_with_app { p } else { p.join(APP_NAME) };
        unsafe { SetWindowTextW(edit, wide(dir).as_ptr()) };
    }
}

/// The shell's folder picker (IFileOpenDialog with FOS_PICKFOLDERS), starting at the deepest existing folder of
/// `current`. None when cancelled.
fn pick_folder(owner: HWND, current: &str) -> Option<String> {
    use windows::core::PCWSTR;
    use windows::Win32::System::Com::{CoCreateInstance, CoTaskMemFree, IBindCtx, CLSCTX_INPROC_SERVER};
    use windows::Win32::UI::Shell::{
        FileOpenDialog, IFileOpenDialog, IShellItem, SHCreateItemFromParsingName, FOS_FORCEFILESYSTEM,
        FOS_PATHMUSTEXIST, FOS_PICKFOLDERS, SIGDN_FILESYSPATH,
    };
    unsafe {
        let dlg: IFileOpenDialog = CoCreateInstance(&FileOpenDialog, None, CLSCTX_INPROC_SERVER).ok()?;
        let opts = dlg.GetOptions().ok()?;
        dlg.SetOptions(opts | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST).ok()?;
        let mut start = PathBuf::from(current);
        while !start.is_dir() && start.pop() {}
        if start.is_dir() {
            let w = wide(&start);
            if let Ok(item) = SHCreateItemFromParsingName::<_, _, IShellItem>(PCWSTR(w.as_ptr()), None::<&IBindCtx>) {
                let _ = dlg.SetFolder(&item);
            }
        }
        dlg.Show(Some(windows::Win32::Foundation::HWND(owner))).ok()?;
        let item = dlg.GetResult().ok()?;
        let name = item.GetDisplayName(SIGDN_FILESYSPATH).ok()?;
        let s = name.to_string().ok();
        CoTaskMemFree(Some(name.0 as *const _));
        s
    }
}

unsafe extern "system" fn wndproc(hwnd: HWND, msg: u32, wp: WPARAM, lp: LPARAM) -> LRESULT {
    match msg {
        WM_PAINT => {
            let mut ps: PAINTSTRUCT = std::mem::zeroed();
            let hdc = BeginPaint(hwnd, &mut ps);
            UI.with(|u| {
                if let Ok(u) = u.try_borrow() {
                    if let Some(u) = u.as_ref() {
                        paint(u, hdc);
                    }
                }
            });
            EndPaint(hwnd, &ps);
            0
        }
        WM_CTLCOLOREDIT | WM_CTLCOLORSTATIC | WM_CTLCOLORBTN => {
            let hdc = wp as HDC;
            let Some((edit, card, bg)) = with(|u| (u.edit, u.br_card, u.br_bg)) else {
                return DefWindowProcW(hwnd, msg, wp, lp);
            };
            SetTextColor(hdc, FG);
            if lp as HWND == edit {
                SetBkColor(hdc, CARD);
                card as LRESULT
            } else {
                SetBkColor(hdc, BG);
                bg as LRESULT
            }
        }
        WM_DRAWITEM => {
            let d = &*(lp as *const DRAWITEMSTRUCT);
            if let Some(f) = with(|u| u.f_ui) {
                draw_button(d, f);
            }
            1
        }
        WM_SETCURSOR => {
            let over = wp as HWND;
            let button = with(|u| over == u.browse || over == u.install).unwrap_or(false);
            if button && IsWindowEnabled(over) != 0 {
                SetCursor(LoadCursorW(std::ptr::null_mut(), IDC_HAND));
                return 1;
            }
            DefWindowProcW(hwnd, msg, wp, lp)
        }
        WM_COMMAND => {
            match (wp & 0xFFFF) as i32 {
                ID_INSTALL => {
                    let (busy, done) = with(|u| (u.busy, u.done)).unwrap_or((false, false));
                    if done {
                        DestroyWindow(hwnd);
                    } else if !busy {
                        start_install(hwnd);
                    }
                }
                ID_CANCEL => {
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                }
                ID_BROWSE => {
                    if !with(|u| u.busy).unwrap_or(true) {
                        browse(hwnd);
                    }
                }
                _ => {}
            }
            0
        }
        WM_TIMER => {
            if let Some(r) = with(|u| {
                u.anim += u.px(6);
                u.rect(BAR)
            }) {
                InvalidateRect(hwnd, &r, 0);
            }
            0
        }
        WM_STATUS => {
            let s = *Box::from_raw(lp as *mut String);
            set_status(hwnd, s, DIM);
            0
        }
        WM_DONE => {
            let err = *Box::from_raw(lp as *mut Option<String>);
            finish_install(hwnd, err);
            0
        }
        WM_CLOSE => {
            // not in the middle of copying files or waiting for a UAC prompt
            if !with(|u| u.busy).unwrap_or(false) {
                DestroyWindow(hwnd);
            }
            0
        }
        WM_DESTROY => {
            PostQuitMessage(0);
            0
        }
        _ => DefWindowProcW(hwnd, msg, wp, lp),
    }
}
