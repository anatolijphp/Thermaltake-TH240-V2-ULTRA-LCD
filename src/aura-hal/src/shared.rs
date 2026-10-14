//! Shared memory "Global\TTLcdAuraSync" (created here, inside the SYSTEM service, readable by everyone) and the
//! optional debug log. Layout (little-endian), identical to the C# plugin and read by ttlcd_core::aura:
//!   0 magic 'TTAS' | 4 seq | 8 effectId | 12 ledCount | 16 speed | 20 direction |
//!   24 syncMs (i64) | 32 updated (.NET UTC ticks, i64) | 40 colors[64] (0x00BBGGRR)
use std::ffi::c_void;
use std::ptr::null_mut;
use std::sync::Mutex;
use std::time::{Duration, Instant};
use windows_sys::Win32::Foundation::{CloseHandle, GetLastError, LocalFree, HANDLE, INVALID_HANDLE_VALUE};
use windows_sys::Win32::Security::Authorization::ConvertStringSecurityDescriptorToSecurityDescriptorW;
use windows_sys::Win32::Security::SECURITY_ATTRIBUTES;
use windows_sys::Win32::System::Memory::{CreateFileMappingW, MapViewOfFile, OpenFileMappingW, FILE_MAP_READ, FILE_MAP_WRITE, PAGE_READWRITE};

pub const MAX_LEDS: usize = 64;
pub const SIZE: usize = 40 + MAX_LEDS * 4;
const MAGIC: u32 = 0x5341_5454; // "TTAS"
const NAME: &str = "Global\\TTLcdAuraSync";
const NAME_LOCAL: &str = "Local\\TTLcdAuraSync"; // tests from a normal user
/// Everyone: read/write; SYSTEM and Administrators: full.
const SDDL: &str = "D:(A;;GRGW;;;WD)(A;;GA;;;SY)(A;;GA;;;BA)";

struct State {
    handle: HANDLE,
    view: *mut u8,
    next_try: Option<Instant>,
}

// The handle and view are only touched with the mutex held.
unsafe impl Send for State {}

static STATE: Mutex<State> = Mutex::new(State { handle: null_mut(), view: null_mut(), next_try: None });

fn wide(s: &str) -> Vec<u16> {
    s.encode_utf16().chain(std::iter::once(0)).collect()
}

/// .NET DateTime.UtcNow.Ticks (100 ns since 0001-01-01 UTC).
fn utc_ticks() -> i64 {
    let d = std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).unwrap_or_default();
    621_355_968_000_000_000 + (d.as_nanos() / 100) as i64
}

/// Raw CreateFileMapping: it creates the section or opens the existing one (e.g. kept alive by TT LCD Studio across a
/// LightingService restart) in one call.
fn map(name: &str, sddl: Option<&str>) -> HANDLE {
    unsafe {
        let mut sd: *mut c_void = null_mut();
        let mut sa = SECURITY_ATTRIBUTES { nLength: std::mem::size_of::<SECURITY_ATTRIBUTES>() as u32, lpSecurityDescriptor: null_mut(), bInheritHandle: 0 };
        if let Some(s) = sddl {
            if ConvertStringSecurityDescriptorToSecurityDescriptorW(wide(s).as_ptr(), 1, &mut sd, null_mut()) != 0 {
                sa.lpSecurityDescriptor = sd;
            }
        }
        let n = wide(name);
        let mut h = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, SIZE as u32, n.as_ptr());
        if h.is_null() {
            // An existing section that only grants read/write (e.g. created by an older build) refuses the full
            // access CreateFileMapping asks for: open it with just what we need.
            let err = GetLastError();
            h = OpenFileMappingW(FILE_MAP_WRITE | FILE_MAP_READ, 0, n.as_ptr());
            if h.is_null() {
                log(&format!("open {name} failed, errors {err}/{}", GetLastError()));
            }
        }
        if !sd.is_null() {
            LocalFree(sd as _);
        }
        h
    }
}

fn view(st: &mut State) -> Result<*mut u8, String> {
    if !st.view.is_null() {
        return Ok(st.view);
    }
    let now = Instant::now();
    if st.next_try.is_some_and(|t| now < t) {
        return Err("shared memory unavailable".into());
    }
    st.next_try = Some(now + Duration::from_secs(2));
    let mut h = map(NAME, Some(SDDL));
    if h.is_null() {
        h = map(NAME_LOCAL, None);
    }
    if h.is_null() {
        return Err("cannot create shared memory".into());
    }
    let v = unsafe { MapViewOfFile(h, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, SIZE) };
    if v.Value.is_null() {
        let err = unsafe { GetLastError() };
        unsafe { CloseHandle(h) };
        return Err(format!("MapViewOfFile failed, error {err}"));
    }
    st.handle = h;
    st.view = v.Value as *mut u8;
    w32(st.view, 0, MAGIC);
    log("shared memory attached");
    Ok(st.view)
}

fn w32(v: *mut u8, off: usize, x: u32) {
    debug_assert!(off + 4 <= SIZE);
    unsafe { std::ptr::write_volatile(v.add(off) as *mut u32, x) }
}

fn w64(v: *mut u8, off: usize, x: i64) {
    debug_assert!(off + 8 <= SIZE);
    unsafe { std::ptr::write_volatile(v.add(off) as *mut i64, x) }
}

/// Continue the existing sequence so a reattached section keeps counting forward. Written last: readers use it to
/// detect a complete update.
fn bump(v: *mut u8) {
    std::sync::atomic::fence(std::sync::atomic::Ordering::Release);
    let seq = unsafe { std::ptr::read_volatile(v.add(4) as *const u32) };
    w32(v, 4, seq.wrapping_add(1));
}

fn lock() -> std::sync::MutexGuard<'static, State> {
    STATE.lock().unwrap_or_else(|e| e.into_inner())
}

/// SetEffect: effect, colours (n of them, the rest 0), speed and direction; syncMs only when >= 0.
pub fn write(effect: u32, colors: &[u32; MAX_LEDS], n: u32, speed: u32, direction: u32, sync_ms: i64) -> Result<(), String> {
    let mut st = lock();
    let v = view(&mut st)?;
    w32(v, 8, effect);
    w32(v, 12, n);
    w32(v, 16, speed);
    w32(v, 20, direction);
    if sync_ms >= 0 {
        w64(v, 24, sync_ms);
    }
    w64(v, 32, utc_ticks());
    for (i, c) in colors.iter().enumerate() {
        w32(v, 40 + i * 4, *c);
    }
    bump(v);
    Ok(())
}

/// Synchronize: the effect and the shared animation clock.
pub fn write_sync(effect: u32, sync_ms: i64) -> Result<(), String> {
    let mut st = lock();
    let v = view(&mut st)?;
    w32(v, 8, effect);
    w64(v, 24, sync_ms);
    w64(v, 32, utc_ticks());
    bump(v);
    Ok(())
}

/// Folder this DLL was loaded from.
fn module_dir() -> Option<&'static std::path::Path> {
    use windows_sys::Win32::System::LibraryLoader::{GetModuleFileNameW, GetModuleHandleExW, GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT};
    static DIR: std::sync::OnceLock<Option<std::path::PathBuf>> = std::sync::OnceLock::new();
    DIR.get_or_init(|| unsafe {
        let mut m = null_mut();
        let flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if GetModuleHandleExW(flags, module_dir as *const () as *const u16, &mut m) == 0 {
            return None;
        }
        let mut buf = vec![0u16; 32768];
        let n = GetModuleFileNameW(m, buf.as_mut_ptr(), buf.len() as u32) as usize;
        if n == 0 || n >= buf.len() {
            return None;
        }
        let p = std::path::PathBuf::from(String::from_utf16_lossy(&buf[..n]));
        p.parent().map(|d| d.to_path_buf())
    })
    .as_deref()
}

/// Optional diagnostics: create C:\ProgramData\ttlcd\aura\debug (next to the DLL) to enable.
pub fn log(s: &str) {
    use std::io::Write;
    let Some(dir) = module_dir() else { return };
    let dbg = dir.join("debug");
    if !dbg.is_dir() {
        return;
    }
    let mut t = unsafe { std::mem::zeroed() };
    unsafe { windows_sys::Win32::System::SystemInformation::GetLocalTime(&mut t) };
    let t: windows_sys::Win32::Foundation::SYSTEMTIME = t;
    let line = format!("{:02}:{:02}:{:02}.{:03} {}\r\n", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, s);
    if let Ok(mut f) = std::fs::OpenOptions::new().create(true).append(true).open(dbg.join("aura.log")) {
        let _ = f.write_all(line.as_bytes());
    }
}
