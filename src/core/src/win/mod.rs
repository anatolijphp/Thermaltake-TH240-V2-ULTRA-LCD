//! Windows-only helpers and features: shared memory blocks, registry, elevation, and the modules for screen capture,
//! video decoding, game detection, the virtual display driver, the Aura plugin registration and the CPU temperature
//! helper / service.
pub mod aura_install;
pub mod capture;
pub mod games;
pub mod mf;
pub mod reg;
pub mod sensors;
pub mod vdd;

use std::ffi::c_void;
use std::io;
use std::ptr::{null, null_mut};
use windows_sys::Win32::Foundation::*;
use windows_sys::Win32::System::Memory::*;

pub fn wide(s: &str) -> Vec<u16> {
    s.encode_utf16().chain(std::iter::once(0)).collect()
}

pub fn from_wide(p: &[u16]) -> String {
    let n = p.iter().position(|&c| c == 0).unwrap_or(p.len());
    String::from_utf16_lossy(&p[..n])
}

/// A named shared-memory block opened read-only (or read/write) by an existing creator.
pub struct SharedView {
    handle: HANDLE,
    view: *mut c_void,
    size: usize,
}

unsafe impl Send for SharedView {}
unsafe impl Sync for SharedView {}

impl SharedView {
    pub fn open(name: &str, size: usize, write: bool) -> Option<SharedView> {
        unsafe {
            let access = if write { FILE_MAP_READ | FILE_MAP_WRITE } else { FILE_MAP_READ };
            let h = OpenFileMappingW(access, 0, wide(name).as_ptr());
            if h.is_null() {
                return None;
            }
            let v = MapViewOfFile(h, access, 0, 0, size);
            if v.Value.is_null() {
                CloseHandle(h);
                return None;
            }
            Some(SharedView { handle: h, view: v.Value, size })
        }
    }

    /// Creates the block (or opens the existing one) with a security descriptor (SDDL), e.g. Everyone read/write.
    pub fn create(name: &str, size: usize, sddl: Option<&str>) -> Option<SharedView> {
        use windows_sys::Win32::Security::Authorization::ConvertStringSecurityDescriptorToSecurityDescriptorW;
        use windows_sys::Win32::Security::SECURITY_ATTRIBUTES;
        unsafe {
            let mut sd: *mut c_void = null_mut();
            let mut sa = SECURITY_ATTRIBUTES { nLength: std::mem::size_of::<SECURITY_ATTRIBUTES>() as u32, lpSecurityDescriptor: null_mut(), bInheritHandle: 0 };
            if let Some(s) = sddl {
                if ConvertStringSecurityDescriptorToSecurityDescriptorW(wide(s).as_ptr(), 1, &mut sd, null_mut()) != 0 {
                    sa.lpSecurityDescriptor = sd;
                }
            }
            let mut h = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, size as u32, wide(name).as_ptr());
            if !sd.is_null() {
                LocalFree(sd as _);
            }
            if h.is_null() {
                // an existing section that only grants read/write refuses full access: open it with what we need
                h = OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, 0, wide(name).as_ptr());
                if h.is_null() {
                    return None;
                }
            }
            let v = MapViewOfFile(h, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, size);
            if v.Value.is_null() {
                CloseHandle(h);
                return None;
            }
            Some(SharedView { handle: h, view: v.Value, size })
        }
    }

    pub fn read_i32(&self, off: usize) -> i32 {
        assert!(off + 4 <= self.size);
        unsafe { std::ptr::read_volatile((self.view as *const u8).add(off) as *const i32) }
    }
    pub fn read_i64(&self, off: usize) -> i64 {
        assert!(off + 8 <= self.size);
        unsafe { std::ptr::read_volatile((self.view as *const u8).add(off) as *const i64) }
    }
    pub fn write_i32(&self, off: usize, v: i32) {
        assert!(off + 4 <= self.size);
        unsafe { std::ptr::write_volatile((self.view as *mut u8).add(off) as *mut i32, v) }
    }
    pub fn write_i64(&self, off: usize, v: i64) {
        assert!(off + 8 <= self.size);
        unsafe { std::ptr::write_volatile((self.view as *mut u8).add(off) as *mut i64, v) }
    }
    pub fn bytes(&self) -> Vec<u8> {
        let mut b = vec![0u8; self.size];
        unsafe { std::ptr::copy_nonoverlapping(self.view as *const u8, b.as_mut_ptr(), self.size) };
        b
    }
}

impl Drop for SharedView {
    fn drop(&mut self) {
        unsafe {
            UnmapViewOfFile(MEMORY_MAPPED_VIEW_ADDRESS { Value: self.view });
            CloseHandle(self.handle);
        }
    }
}

pub fn is_admin() -> bool {
    unsafe { windows_sys::Win32::UI::Shell::IsUserAnAdmin() != 0 }
}

/// Returns true if already admin; otherwise relaunches `ttlcd <args>` elevated (UAC prompt) with its output going to
/// a log file, waits, prints that output and returns false.
pub fn elevated(args: &str, log_name: &str) -> io::Result<bool> {
    use windows_sys::Win32::UI::Shell::{ShellExecuteExW, SEE_MASK_NOCLOSEPROCESS, SHELLEXECUTEINFOW};
    if is_admin() {
        return Ok(true);
    }
    let exe = std::env::current_exe()?;
    let log = std::env::temp_dir().join(log_name);
    let _ = std::fs::remove_file(&log);
    let params = format!("/c \"\"{}\" {} > \"{}\" 2>&1\"", exe.display(), args, log.display());
    unsafe {
        let verb = wide("runas");
        let file = wide("cmd.exe");
        let p = wide(&params);
        let mut info: SHELLEXECUTEINFOW = std::mem::zeroed();
        info.cbSize = std::mem::size_of::<SHELLEXECUTEINFOW>() as u32;
        info.fMask = SEE_MASK_NOCLOSEPROCESS;
        info.lpVerb = verb.as_ptr();
        info.lpFile = file.as_ptr();
        info.lpParameters = p.as_ptr();
        info.nShow = 0; // SW_HIDE
        if ShellExecuteExW(&mut info) == 0 {
            println!("error: administrator rights were refused");
            return Ok(false);
        }
        if !info.hProcess.is_null() {
            windows_sys::Win32::System::Threading::WaitForSingleObject(info.hProcess, u32::MAX);
            CloseHandle(info.hProcess);
        }
    }
    if let Ok(s) = std::fs::read_to_string(&log) {
        print!("{s}");
    }
    Ok(false)
}

/// Runs a system tool and returns its output (stdout + stderr) and exit code, without a console window.
pub fn run_tool(exe: &str, args: &[&str]) -> (String, i32) {
    use std::os::windows::process::CommandExt;
    match std::process::Command::new(exe).args(args).creation_flags(0x0800_0000).output() {
        Ok(o) => (format!("{}{}", String::from_utf8_lossy(&o.stdout), String::from_utf8_lossy(&o.stderr)), o.status.code().unwrap_or(-1)),
        Err(e) => (format!("error: {e}"), -1),
    }
}

pub fn system_dir() -> std::path::PathBuf {
    let mut buf = [0u16; 260];
    let n = unsafe { windows_sys::Win32::System::SystemInformation::GetSystemDirectoryW(buf.as_mut_ptr(), buf.len() as u32) };
    std::path::PathBuf::from(String::from_utf16_lossy(&buf[..n as usize]))
}

/// %ProgramData%
pub fn program_data() -> std::path::PathBuf {
    std::env::var_os("ProgramData").map(Into::into).unwrap_or_else(|| "C:\\ProgramData".into())
}

#[allow(dead_code)]
fn _unused() -> *const u16 {
    null()
}
