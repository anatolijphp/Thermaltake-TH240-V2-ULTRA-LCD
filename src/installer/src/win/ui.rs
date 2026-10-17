//! The install window: plain Win32 in the app's dark style (port of Setup.cs SetupForm). Laid out once at the system
//! DPI (the setup's manifest is system DPI aware) from ttlcd_setup::layout. Steps: folder and options, then one
//! per optional component (ttlcd_setup::media::COMPONENTS) with a GIF of what it adds to the LCD; the last installs.
use super::install::{self, Options};
use super::{wide, APP_NAME, VERSION};
use std::cell::RefCell;
use std::path::PathBuf;
use std::time::Instant;
use ttlcd_setup::layout::{self as l, Control, R};
use ttlcd_setup::media::{self, Anim, COMPONENTS};
use windows_sys::Win32::Foundation::{COLORREF, HWND, LPARAM, LRESULT, RECT, WPARAM};
use windows_sys::Win32::Graphics::Dwm::DwmSetWindowAttribute;
use windows_sys::Win32::Graphics::Gdi::{
    BeginPaint, BitBlt, CreateCompatibleBitmap, CreateCompatibleDC, CreateFontW, CreateSolidBrush, DeleteDC,
    DeleteObject, DrawFocusRect, DrawTextW, EndPaint, FillRect, FrameRect, GetDC, GetDeviceCaps, GetStockObject,
    InvalidateRect, ReleaseDC, SelectObject, SetBkColor, SetBkMode, SetBrushOrgEx, SetDCBrushColor, SetStretchBltMode,
    SetTextColor, StretchDIBits, BITMAPINFO, BITMAPINFOHEADER, BI_RGB, CLEARTYPE_QUALITY, CLIP_DEFAULT_PRECIS, DC_BRUSH,
    DEFAULT_CHARSET, DEFAULT_PITCH, DIB_RGB_COLORS, DT_CENTER, DT_END_ELLIPSIS, DT_LEFT, DT_NOPREFIX, DT_SINGLELINE,
    DT_VCENTER, DT_WORDBREAK, FF_DONTCARE, FW_NORMAL, FW_SEMIBOLD, HALFTONE, HBRUSH, HDC, HFONT, LOGPIXELSY,
    OUT_DEFAULT_PRECIS, PAINTSTRUCT, SRCCOPY, TRANSPARENT,
};
use windows_sys::Win32::System::LibraryLoader::GetModuleHandleW;
use windows_sys::Win32::UI::Controls::{
    SetWindowTheme, BST_CHECKED, DRAWITEMSTRUCT, ODS_DISABLED, ODS_FOCUS, ODS_SELECTED,
};
use windows_sys::Win32::UI::Input::KeyboardAndMouse::{EnableWindow, IsWindowEnabled, SetFocus};
use windows_sys::Win32::UI::WindowsAndMessaging::{
    AdjustWindowRectEx, CreateWindowExW, DefWindowProcW, DestroyWindow, DispatchMessageW, GetClientRect, GetMessageW,
    GetWindowTextLengthW, GetWindowTextW, IsDialogMessageW, KillTimer, LoadCursorW, LoadImageW, PostMessageW,
    PostQuitMessage, RegisterClassExW, SendMessageW, SetCursor, SetTimer, SetWindowTextW, ShowWindow,
    SystemParametersInfoW, TranslateMessage, BM_GETCHECK, BM_SETCHECK, BS_AUTOCHECKBOX, BS_OWNERDRAW, CS_HREDRAW,
    CS_VREDRAW, ES_AUTOHSCROLL, HMENU, IDC_ARROW, IDC_HAND, IMAGE_ICON, LR_DEFAULTCOLOR, MSG, SPI_GETWORKAREA, SW_HIDE,
    SW_SHOW, WM_APP, WM_CLOSE, WM_COMMAND, WM_CTLCOLORBTN, WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, WM_DESTROY,
    WM_DRAWITEM, WM_ERASEBKGND, WM_PAINT, WM_SETCURSOR, WM_SETFONT, WM_TIMER, WNDCLASSEXW, WS_CAPTION, WS_CHILD,
    WS_CLIPCHILDREN, WS_MINIMIZEBOX, WS_OVERLAPPED, WS_SYSMENU, WS_TABSTOP,
};

const fn rgb(r: u8, g: u8, b: u8) -> COLORREF {
    r as u32 | (g as u32) << 8 | (b as u32) << 16
}
const BG: COLORREF = rgb(13, 13, 14);
const CARD_RGB: [u8; 3] = [27, 27, 29]; // also the colour around the round screen in the GIFs
const CARD: COLORREF = rgb(CARD_RGB[0], CARD_RGB[1], CARD_RGB[2]);
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

const ID_INSTALL: i32 = 1; // IDOK: Enter (through IsDialogMessage) presses it; "Next" on step 1
const ID_CANCEL: i32 = 2; // IDCANCEL: Escape
const ID_BROWSE: i32 = 100;
const ID_EDIT: i32 = 101;
const ID_BACK: i32 = 102;
const ID_CHECK: i32 = 110;
const ID_COMPONENT: i32 = 120;
const WM_STATUS: u32 = WM_APP + 1;
const WM_DONE: u32 = WM_APP + 2;
const TIMER_BAR: usize = 1;
const TIMER_GIF: usize = 2;

/// Step 1 checkboxes: label, default. The optional components (all off by default) are on step 2.
const CHECKS: [(&str, bool); 4] = [
    ("Start menu shortcut", true),
    ("Desktop shortcut", true),
    ("Start with Windows (in the tray)", true),
    ("Launch TT LCD Studio when done", true),
];

/// A component GIF while it plays.
struct Playing {
    anim: Anim,
    frame: usize,
    elapsed_ms: u32,
}

struct Ui {
    edit: HWND,
    browse: HWND,
    back: HWND,
    install: HWND,
    checks: Vec<HWND>,
    components: Vec<HWND>,
    gifs: Vec<Option<Playing>>,
    gif_tick: Instant,
    f_ui: HFONT,
    f_title: HFONT,
    f_label: HFONT,
    f_item: HFONT,
    f_small: HFONT,
    br_bg: HBRUSH,
    br_card: HBRUSH,
    scale: f32,
    step: usize,
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
    fn rect(&self, r: R) -> RECT {
        RECT { left: self.px(r.0), top: self.px(r.1), right: self.px(r.0 + r.2), bottom: self.px(r.1 + r.3) }
    }
    /// Every control with its kind (which step shows it: layout::shown).
    fn kinds(&self) -> Vec<(HWND, Control)> {
        let mut v = vec![
            (self.edit, Control::Folder),
            (self.browse, Control::Folder),
            (self.back, Control::Back),
            (self.install, Control::Install),
        ];
        v.extend(self.checks.iter().map(|h| (*h, Control::Option)));
        v.extend(self.components.iter().enumerate().map(|(i, h)| (*h, Control::Component(i))));
        v
    }
    fn controls(&self) -> Vec<HWND> {
        self.kinds().into_iter().map(|(h, _)| h).collect()
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
        // clip children: the window is painted from a back buffer, which must not cover the controls
        let style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
        let px = |v: i32| (v as f32 * scale).round() as i32;
        let mut rc = RECT { left: 0, top: 0, right: px(l::CLIENT.0), bottom: px(l::CLIENT.1) };
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
        let f_item = font(dpi, 13.0, FW_SEMIBOLD as i32);
        let child = |class: &str, text: &str, style: u32, r: R, id: i32, f: HFONT| {
            let h = CreateWindowExW(
                0,
                wide(class).as_ptr(),
                wide(text).as_ptr(),
                WS_CHILD | WS_TABSTOP | style,
                px(r.0),
                px(r.1),
                px(r.2),
                px(r.3),
                hwnd,
                id as HMENU,
                hinst,
                std::ptr::null(),
            );
            SendMessageW(h, WM_SETFONT, f as WPARAM, 0);
            h
        };
        let checkbox = |text: &str, r: R, id: i32, on: bool, f: HFONT| {
            let c = child("BUTTON", text, BS_AUTOCHECKBOX as u32, r, id, f);
            // classic drawing, so the WM_CTLCOLOR* text colour applies
            SetWindowTheme(c, wide("").as_ptr(), wide("").as_ptr());
            if on {
                SendMessageW(c, BM_SETCHECK, BST_CHECKED as WPARAM, 0);
            }
            c
        };
        // the edit sits borderless inside the painted dir box, vertically centred
        let edit_h = px(18);
        let edit = CreateWindowExW(
            0,
            wide("EDIT").as_ptr(),
            wide(install::default_dir()).as_ptr(),
            WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL as u32,
            px(l::DIR_BOX.0 + 7),
            px(l::DIR_BOX.1) + (px(l::DIR_BOX.3) - edit_h) / 2,
            px(l::DIR_BOX.2 - 14),
            edit_h,
            hwnd,
            ID_EDIT as HMENU,
            hinst,
            std::ptr::null(),
        );
        SendMessageW(edit, WM_SETFONT, f_ui as WPARAM, 0);
        let browse = child("BUTTON", "Browse…", BS_OWNERDRAW as u32, l::BROWSE, ID_BROWSE, f_ui);
        let checks: Vec<HWND> = CHECKS
            .iter()
            .enumerate()
            .map(|(i, (text, on))| checkbox(text, l::option_check(i), ID_CHECK + i as i32, *on, f_ui))
            .collect();
        let components: Vec<HWND> = COMPONENTS
            .iter()
            .enumerate()
            .map(|(i, c)| checkbox(c.check, l::CHECK, ID_COMPONENT + i as i32, false, f_ui))
            .collect();
        let back = child("BUTTON", "Back", BS_OWNERDRAW as u32, l::BACK, ID_BACK, f_ui);
        let install = child("BUTTON", "Next", BS_OWNERDRAW as u32, l::INSTALL, ID_INSTALL, f_ui);

        UI.with(|u| {
            *u.borrow_mut() = Some(Ui {
                edit,
                browse,
                back,
                install,
                checks,
                components,
                gifs: Vec::new(),
                gif_tick: Instant::now(),
                f_ui,
                f_title: font(dpi, 16.0, FW_SEMIBOLD as i32),
                f_label: font(dpi, 8.5, FW_SEMIBOLD as i32),
                f_item,
                f_small: font(dpi, 9.0, FW_NORMAL as i32),
                br_bg,
                br_card,
                scale,
                step: 0,
                status: String::new(),
                status_color: DIM,
                busy: false,
                done: false,
                anim: 0,
            })
        });
        set_step(hwnd, 0);
        ShowWindow(hwnd, SW_SHOW);

        let mut msg: MSG = std::mem::zeroed();
        while GetMessageW(&mut msg, std::ptr::null_mut(), 0, 0) > 0 {
            if IsDialogMessageW(hwnd, &msg) == 0 {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        if let Some(u) = UI.with(|u| u.borrow_mut().take()) {
            for o in [u.f_ui, u.f_title, u.f_label, u.f_item, u.f_small] {
                DeleteObject(o);
            }
            DeleteObject(u.br_bg);
            DeleteObject(u.br_card);
        }
    }
    Ok(0)
}

/// Shows step 0 (folder, options) or 1 (components): its controls, the button texts, the GIFs playing on step 1.
fn set_step(hwnd: HWND, step: usize) {
    let Some((kinds, install)) = with(|u| {
        u.step = step;
        if step > 0 && u.gifs.is_empty() {
            // decoded when the first component step shows: frames the folder step doesn't need
            u.gifs = COMPONENTS
                .iter()
                .map(|c| {
                    let mut anim = media::decode(c.gif, CARD_RGB).ok()?;
                    media::fade_bottom(&mut anim, CARD_RGB);
                    Some(Playing { anim, frame: 0, elapsed_ms: 0 })
                })
                .collect();
        }
        u.gif_tick = Instant::now();
        (u.kinds(), u.install)
    }) else {
        return;
    };
    unsafe {
        for (h, kind) in kinds {
            ShowWindow(h, if l::shown(kind, step) { SW_SHOW } else { SW_HIDE });
        }
        SetWindowTextW(install, wide(if step + 1 < l::STEPS { "Next" } else { "Install" }).as_ptr());
        InvalidateRect(install, std::ptr::null(), 0);
        if l::step_component(step).is_some() {
            SetTimer(hwnd, TIMER_GIF, 20, None);
        } else {
            KillTimer(hwnd, TIMER_GIF);
        }
        InvalidateRect(hwnd, std::ptr::null(), 0);
        SetFocus(install);
    }
}

fn text(hdc: HDC, f: HFONT, color: COLORREF, mut r: RECT, s: &str, flags: u32) {
    // DrawTextW reads the first character even for a count of 0, and an empty Vec's pointer is a dangling 0x2:
    // the empty status line crashed the setup on its first paint (before the buttons were drawn)
    if s.is_empty() {
        return;
    }
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

/// The current frame of a component GIF, scaled into `r` (halftone: smooth at any DPI).
fn draw_gif(hdc: HDC, p: &Playing, r: &RECT) {
    let a = &p.anim;
    let f = &a.frames[p.frame];
    let mut bmi: BITMAPINFO = unsafe { std::mem::zeroed() };
    bmi.bmiHeader = BITMAPINFOHEADER {
        biSize: std::mem::size_of::<BITMAPINFOHEADER>() as u32,
        biWidth: a.width as i32,
        biHeight: -(a.height as i32), // top-down rows
        biPlanes: 1,
        biBitCount: 32,
        biCompression: BI_RGB,
        ..unsafe { std::mem::zeroed() }
    };
    unsafe {
        SetStretchBltMode(hdc, HALFTONE);
        SetBrushOrgEx(hdc, 0, 0, std::ptr::null_mut());
        StretchDIBits(
            hdc,
            r.left,
            r.top,
            r.right - r.left,
            r.bottom - r.top,
            0,
            0,
            a.width as i32,
            a.height as i32,
            f.bgra.as_ptr().cast(),
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY,
        );
    }
}

fn paint(u: &Ui, hdc: HDC, client: &RECT) {
    fill(hdc, client, BG);
    unsafe { SetBkMode(hdc, TRANSPARENT as i32) };
    let single = DT_LEFT | DT_SINGLELINE;
    text(hdc, u.f_title, FG, u.rect(l::TITLE), "TT  LCD STUDIO", single);
    let line = u.rect(l::RGB_LINE);
    let width = (line.right - line.left).max(1);
    for x in 0..width {
        let col = RECT { left: line.left + x, top: line.top, right: line.left + x + 1, bottom: line.bottom };
        fill(hdc, &col, gradient_at(x as f32 / (width - 1).max(1) as f32));
    }
    let sub = format!("Control the LCD of your Thermaltake TH240 V2 Ultra cooler.  Version {VERSION}");
    text(hdc, u.f_ui, DIM, u.rect(l::SUBTITLE), &sub, single | DT_END_ELLIPSIS);
    if u.step == 0 {
        text(hdc, u.f_label, DIM, u.rect(l::SECTION), "INSTALL TO", single);
        let dir = u.rect(l::DIR_BOX);
        fill(hdc, &dir, CARD);
        frame(hdc, &dir, LINE);
        text(hdc, u.f_label, DIM, u.rect(l::OPTIONS_LABEL), "OPTIONS", single);
    } else if let Some(i) = l::step_component(u.step) {
        let c = &COMPONENTS[i];
        let section = format!("OPTIONAL COMPONENT {} OF {}", i + 1, COMPONENTS.len());
        text(hdc, u.f_label, DIM, u.rect(l::SECTION), &section, single);
        let card = u.rect(l::CARD);
        fill(hdc, &card, CARD);
        frame(hdc, &card, LINE);
        if let Some(Some(p)) = u.gifs.get(i) {
            draw_gif(hdc, p, &u.rect(l::GIF));
        }
        text(hdc, u.f_item, FG, u.rect(l::NAME), c.title, single | DT_END_ELLIPSIS);
        text(hdc, u.f_ui, DIM, u.rect(l::TEXT), c.text, DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS);
        let note = "Needs administrator rights: Windows asks while installing. It can also be added later in Studio.";
        text(hdc, u.f_small, DIM, u.rect(l::NOTE), note, DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS);
    }
    if u.busy {
        // indeterminate progress: an accent segment sweeping along the track
        let track = u.rect(l::BAR);
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
    text(hdc, u.f_ui, u.status_color, u.rect(l::STATUS), &u.status, DT_LEFT | DT_WORDBREAK | DT_END_ELLIPSIS);
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
    unsafe { SetBkMode(hdc, TRANSPARENT as i32) };
    text(hdc, f, fg, r, &window_text(d.hwndItem), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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

fn install_dir() -> Option<String> {
    let edit = with(|u| u.edit)?;
    let dir = window_text(edit).trim().trim_matches('"').to_string();
    (!dir.is_empty()).then_some(dir)
}

fn start_install(hwnd: HWND) {
    let Some(dir) = install_dir() else { return };
    let Some((checks, components, controls)) = with(|u| (u.checks.clone(), u.components.clone(), u.controls()))
    else {
        return;
    };
    let on = |h: HWND| unsafe { SendMessageW(h, BM_GETCHECK, 0, 0) } == BST_CHECKED as LRESULT;
    let o = Options {
        dir: PathBuf::from(dir),
        start_menu: on(checks[0]),
        desktop: on(checks[1]),
        autostart: on(checks[2]),
        launch: on(checks[3]),
        sensor_service: on(components[0]),
        aura: on(components[1]),
        virtual_display: on(components[2]),
    };
    for c in controls {
        unsafe { EnableWindow(c, 0) };
    }
    let bar = with(|u| {
        u.busy = true;
        u.status.clear();
        u.status_color = DIM;
        (u.rect(l::BAR), u.rect(l::STATUS))
    });
    if let Some((b, s)) = bar {
        unsafe {
            InvalidateRect(hwnd, &b, 0);
            InvalidateRect(hwnd, &s, 0);
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
        u.rect(l::STATUS)
    }) {
        unsafe { InvalidateRect(hwnd, &r, 0) };
    }
}

fn finish_install(hwnd: HWND, err: Option<String>) {
    let Some((install, back, controls, bar)) = with(|u| {
        u.busy = false;
        u.done = err.is_none();
        (u.install, u.back, u.controls(), u.rect(l::BAR))
    }) else {
        return;
    };
    unsafe {
        KillTimer(hwnd, TIMER_BAR);
        InvalidateRect(hwnd, &bar, 0);
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
                ShowWindow(back, SW_HIDE);
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

/// Advances the shown step's GIF by the time passed; its area when it changed frame.
fn tick_gif() -> Option<RECT> {
    with(|u| {
        let now = Instant::now();
        let dt = now.duration_since(u.gif_tick).as_millis().min(1000) as u32;
        u.gif_tick = now;
        let i = l::step_component(u.step)?;
        let p = u.gifs.get_mut(i)?.as_mut()?;
        p.elapsed_ms += dt;
        let before = p.frame;
        while p.elapsed_ms >= p.anim.frames[p.frame].delay_ms {
            p.elapsed_ms -= p.anim.frames[p.frame].delay_ms;
            p.frame = (p.frame + 1) % p.anim.frames.len();
        }
        (p.frame != before).then(|| u.rect(l::GIF))
    })
    .flatten()
}

unsafe extern "system" fn wndproc(hwnd: HWND, msg: u32, wp: WPARAM, lp: LPARAM) -> LRESULT {
    match msg {
        WM_ERASEBKGND => 1, // WM_PAINT covers everything
        WM_PAINT => {
            let mut ps: PAINTSTRUCT = std::mem::zeroed();
            let hdc = BeginPaint(hwnd, &mut ps);
            // drawn into a back buffer and copied in one go: the GIFs repaint 10 times a second without flicker
            let mut client = RECT { left: 0, top: 0, right: 0, bottom: 0 };
            GetClientRect(hwnd, &mut client);
            let mem = CreateCompatibleDC(hdc);
            let bmp = CreateCompatibleBitmap(hdc, client.right.max(1), client.bottom.max(1));
            let old = SelectObject(mem, bmp);
            UI.with(|u| {
                if let Ok(u) = u.try_borrow() {
                    if let Some(u) = u.as_ref() {
                        paint(u, mem, &client);
                    }
                }
            });
            let r = ps.rcPaint;
            BitBlt(hdc, r.left, r.top, r.right - r.left, r.bottom - r.top, mem, r.left, r.top, SRCCOPY);
            SelectObject(mem, old);
            DeleteObject(bmp);
            DeleteDC(mem);
            EndPaint(hwnd, &ps);
            0
        }
        WM_CTLCOLOREDIT | WM_CTLCOLORSTATIC | WM_CTLCOLORBTN => {
            let hdc = wp as HDC;
            let Some((on_card, card, bg)) =
                with(|u| (lp as HWND == u.edit || u.components.contains(&(lp as HWND)), u.br_card, u.br_bg))
            else {
                return DefWindowProcW(hwnd, msg, wp, lp);
            };
            SetTextColor(hdc, FG);
            if on_card {
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
            let button = with(|u| over == u.browse || over == u.install || over == u.back).unwrap_or(false);
            if button && IsWindowEnabled(over) != 0 {
                SetCursor(LoadCursorW(std::ptr::null_mut(), IDC_HAND));
                return 1;
            }
            DefWindowProcW(hwnd, msg, wp, lp)
        }
        WM_COMMAND => {
            match (wp & 0xFFFF) as i32 {
                ID_INSTALL => {
                    let (busy, done, step) = with(|u| (u.busy, u.done, u.step)).unwrap_or((false, false, 0));
                    if done {
                        DestroyWindow(hwnd);
                    } else if !busy && step + 1 < l::STEPS {
                        if install_dir().is_some() {
                            set_step(hwnd, step + 1);
                        }
                    } else if !busy {
                        start_install(hwnd);
                    }
                }
                ID_BACK => {
                    let (stuck, step) = with(|u| (u.busy || u.done, u.step)).unwrap_or((true, 0));
                    if !stuck && step > 0 {
                        set_step(hwnd, step - 1);
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
            if wp == TIMER_GIF {
                if let Some(r) = tick_gif() {
                    InvalidateRect(hwnd, &r, 0);
                }
            } else if let Some(r) = with(|u| {
                u.anim += u.px(6);
                u.rect(l::BAR)
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
