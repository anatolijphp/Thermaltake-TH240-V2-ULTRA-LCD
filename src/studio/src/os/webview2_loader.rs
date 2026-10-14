//! A pure-Rust stand-in for Microsoft's WebView2Loader.dll, so TTLcdStudio.exe needs no DLL next to it.
//!
//! wry calls `CreateCoreWebView2EnvironmentWithOptions` through webview2-com-sys, which on the gnu targets imports
//! it from WebView2Loader.dll (`#[link(name = "WebView2Loader.dll")]`, so every call goes through the import
//! pointer `__imp_CreateCoreWebView2EnvironmentWithOptions` or its thunk). This module defines both itself. The
//! linker then finds the symbol in our own object file and never pulls the member of the import library that would
//! add WebView2Loader.dll to the import table. (The generic wrappers of webview2-com-sys are instantiated in this
//! crate and call the plain function names, so those are defined here too.)
//!
//! What the real loader does, and this one too (checked against the disassembly of WebView2LoaderStatic.lib,
//! SDK 1.0.x as shipped in webview2-com-sys 0.39):
//! 1. Find the WebView2 runtime's client DLL, `<version folder>\EBWebView\<arch>\EmbeddedBrowserWebView.dll`:
//!    - from `browserExecutableFolder` (or the WEBVIEW2_BROWSER_EXECUTABLE_FOLDER environment variable), a fixed-
//!      version runtime: runtime type 1;
//!    - otherwise from the installed Evergreen runtime (runtime type 0): `HKLM\SOFTWARE\WOW6432Node\Microsoft\
//!      EdgeUpdate\ClientState\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}` value `EBWebView` (the version folder), then
//!      the same under HKCU (per-user install), then the Beta / Dev / Canary channels.
//! 2. `LoadLibrary` it and call its export `CreateWebViewEnvironmentWithOptionsInternal(true, runtimeType,
//!    userDataFolder, environmentOptions, completedHandler)`. An empty user data folder lets the runtime pick its
//!    default (`<exe folder>\<exe name>.WebView2`); WEBVIEW2_USER_DATA_FOLDER overrides it, as with the real loader.
//!
//! If that fails and a WebView2Loader.dll happens to sit next to the exe, the call is forwarded to it.
use std::ffi::{c_void, OsString};
use std::os::windows::ffi::{OsStrExt, OsStringExt};
use std::path::{Path, PathBuf};
use windows_sys::Win32::Foundation::{ERROR_FILE_NOT_FOUND, E_POINTER};
use windows_sys::Win32::System::Com::CoTaskMemAlloc;
use windows_sys::Win32::System::LibraryLoader::{GetProcAddress, LoadLibraryExW, LOAD_WITH_ALTERED_SEARCH_PATH};
use windows_sys::Win32::System::Registry::{
    RegGetValueW, HKEY, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, RRF_RT_REG_SZ, RRF_SUBKEY_WOW6432KEY,
};

type CreateEnvironmentFn = unsafe extern "system" fn(*const u16, *const u16, *mut c_void, *mut c_void) -> i32;
type VersionStringFn = unsafe extern "system" fn(*const u16, *mut *mut u16) -> i32;
type InternalFn = unsafe extern "system" fn(bool, i32, *const u16, *mut c_void, *mut c_void) -> i32;

// The exports themselves: webview2-com-sys's generic wrappers are instantiated in this crate and call the plain
// names; dllimport-style callers would go through the `__imp_` pointers, so both are defined.
#[no_mangle]
pub unsafe extern "system" fn CreateCoreWebView2EnvironmentWithOptions(
    browser_folder: *const u16,
    user_data_folder: *const u16,
    options: *mut c_void,
    handler: *mut c_void,
) -> i32 {
    create_environment_with_options(browser_folder, user_data_folder, options, handler)
}

#[no_mangle]
pub unsafe extern "system" fn GetAvailableCoreWebView2BrowserVersionString(
    browser_folder: *const u16,
    version: *mut *mut u16,
) -> i32 {
    available_browser_version_string(browser_folder, version)
}

#[used]
#[no_mangle]
#[allow(non_upper_case_globals)]
static __imp_CreateCoreWebView2EnvironmentWithOptions: CreateEnvironmentFn = create_environment_with_options;

#[used]
#[no_mangle]
#[allow(non_upper_case_globals)]
static __imp_GetAvailableCoreWebView2BrowserVersionString: VersionStringFn = available_browser_version_string;

/// Stable (the WebView2 Runtime), then Beta, Dev and Canary (Edge preview channels also carry the client DLL).
const CHANNELS: [&str; 4] = [
    "{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}",
    "{2CD8A007-E189-409D-A2C8-9AF4EF3C72AA}",
    "{0D50BFEC-CD6A-4F9A-964C-C7416E3ACB10}",
    "{65C35B14-6C1D-4122-AC46-7148CC9D6497}",
];

#[cfg(target_arch = "aarch64")]
const ARCH: &str = "arm64";
#[cfg(target_arch = "x86")]
const ARCH: &str = "x86";
#[cfg(not(any(target_arch = "aarch64", target_arch = "x86")))]
const ARCH: &str = "x64";

const RUNTIME_INSTALLED: i32 = 0;
const RUNTIME_FIXED_VERSION: i32 = 1;

fn hresult_from_win32(e: u32) -> i32 {
    if e == 0 {
        0
    } else {
        ((e & 0xFFFF) | 0x8007_0000) as i32
    }
}

fn wide(s: &std::ffi::OsStr) -> Vec<u16> {
    s.encode_wide().chain(std::iter::once(0)).collect()
}

/// A NUL-terminated wide string from the caller; None for a null pointer or an empty string.
unsafe fn from_pcwstr(p: *const u16) -> Option<OsString> {
    if p.is_null() {
        return None;
    }
    let mut n = 0;
    while *p.add(n) != 0 {
        n += 1;
    }
    (n > 0).then(|| OsString::from_wide(std::slice::from_raw_parts(p, n)))
}

fn reg_string(root: HKEY, key: &str, value: &str, flags: u32) -> Option<OsString> {
    let key = wide(key.as_ref());
    let value = wide(value.as_ref());
    let mut size = 0u32;
    unsafe {
        let flags = RRF_RT_REG_SZ | flags;
        if RegGetValueW(root, key.as_ptr(), value.as_ptr(), flags, std::ptr::null_mut(), std::ptr::null_mut(), &mut size)
            != 0
            || size < 2
        {
            return None;
        }
        let mut buf = vec![0u16; size as usize / 2 + 1];
        size = (buf.len() * 2) as u32;
        if RegGetValueW(root, key.as_ptr(), value.as_ptr(), flags, std::ptr::null_mut(), buf.as_mut_ptr().cast(), &mut size)
            != 0
        {
            return None;
        }
        let n = buf.iter().position(|&c| c == 0).unwrap_or(buf.len());
        (n > 0).then(|| OsString::from_wide(&buf[..n]))
    }
}

fn client_dll_in(version_folder: &Path) -> Option<PathBuf> {
    let p = version_folder.join("EBWebView").join(ARCH).join("EmbeddedBrowserWebView.dll");
    p.is_file().then_some(p)
}

/// The installed runtime's version folder and client DLL.
fn find_installed() -> Option<(PathBuf, PathBuf)> {
    for guid in CHANNELS {
        // per-machine installs live in the 32-bit registry view; HKCU\Software is not redirected
        for (root, flags) in [(HKEY_LOCAL_MACHINE, RRF_SUBKEY_WOW6432KEY), (HKEY_CURRENT_USER, 0)] {
            let state = format!("Software\\Microsoft\\EdgeUpdate\\ClientState\\{guid}");
            if let Some(dir) = reg_string(root, &state, "EBWebView", flags) {
                let dir = PathBuf::from(dir);
                if let Some(dll) = client_dll_in(&dir) {
                    return Some((dir, dll));
                }
            }
            // older layout: Clients\{guid} "location" + "pv"
            let client = format!("Software\\Microsoft\\EdgeUpdate\\Clients\\{guid}");
            if let (Some(loc), Some(pv)) =
                (reg_string(root, &client, "location", flags), reg_string(root, &client, "pv", flags))
            {
                let dir = PathBuf::from(loc).join(pv);
                if let Some(dll) = client_dll_in(&dir) {
                    return Some((dir, dll));
                }
            }
        }
    }
    None
}

/// (version folder, client DLL, runtime type) for an explicit browser folder or the installed runtime.
fn find_client_dll(browser_folder: Option<&OsString>) -> Option<(PathBuf, PathBuf, i32)> {
    match browser_folder {
        Some(f) => {
            let dir = PathBuf::from(f);
            client_dll_in(&dir).map(|dll| (dir, dll, RUNTIME_FIXED_VERSION))
        }
        None => find_installed().map(|(dir, dll)| (dir, dll, RUNTIME_INSTALLED)),
    }
}

fn env_override(name: &str, arg: Option<OsString>) -> Option<OsString> {
    std::env::var_os(name).filter(|v| !v.is_empty()).or(arg)
}

unsafe fn load(path: &Path) -> *mut c_void {
    // altered search path: the DLL's own dependencies resolve from its folder first
    LoadLibraryExW(wide(path.as_os_str()).as_ptr(), std::ptr::null_mut(), LOAD_WITH_ALTERED_SEARCH_PATH)
}

/// Microsoft's WebView2Loader.dll next to the exe, if somebody put it there.
unsafe fn loader_dll_export(name: &[u8]) -> Option<unsafe extern "system" fn() -> isize> {
    let exe = std::env::current_exe().ok()?;
    let dll = exe.parent()?.join("WebView2Loader.dll");
    if !dll.is_file() {
        return None;
    }
    let h = load(&dll);
    if h.is_null() {
        return None;
    }
    GetProcAddress(h, name.as_ptr())
}

unsafe extern "system" fn create_environment_with_options(
    browser_folder: *const u16,
    user_data_folder: *const u16,
    options: *mut c_void,
    handler: *mut c_void,
) -> i32 {
    if handler.is_null() {
        return E_POINTER;
    }
    let folder = env_override("WEBVIEW2_BROWSER_EXECUTABLE_FOLDER", from_pcwstr(browser_folder));
    let data = env_override("WEBVIEW2_USER_DATA_FOLDER", from_pcwstr(user_data_folder)).map(|d| wide(&d));
    let data_ptr = data.as_ref().map_or(std::ptr::null(), |d| d.as_ptr());
    let mut hr = hresult_from_win32(ERROR_FILE_NOT_FOUND);
    if let Some((_, dll, kind)) = find_client_dll(folder.as_ref()) {
        let h = load(&dll);
        if h.is_null() {
            hr = hresult_from_win32(windows_sys::Win32::Foundation::GetLastError());
        } else if let Some(f) = GetProcAddress(h, c"CreateWebViewEnvironmentWithOptionsInternal".as_ptr().cast()) {
            // the client DLL stays loaded for the life of the process, as with the real loader
            let f: InternalFn = std::mem::transmute(f);
            hr = f(true, kind, data_ptr, options, handler);
            if hr >= 0 {
                return hr;
            }
        }
    }
    if let Some(f) = loader_dll_export(b"CreateCoreWebView2EnvironmentWithOptions\0") {
        let f: CreateEnvironmentFn = std::mem::transmute(f);
        return f(browser_folder, user_data_folder, options, handler);
    }
    hr
}

unsafe extern "system" fn available_browser_version_string(browser_folder: *const u16, version: *mut *mut u16) -> i32 {
    if version.is_null() {
        return E_POINTER;
    }
    *version = std::ptr::null_mut();
    let folder = env_override("WEBVIEW2_BROWSER_EXECUTABLE_FOLDER", from_pcwstr(browser_folder));
    let Some(v) = runtime_version_in(folder.as_ref()) else {
        if let Some(f) = loader_dll_export(b"GetAvailableCoreWebView2BrowserVersionString\0") {
            let f: VersionStringFn = std::mem::transmute(f);
            return f(browser_folder, version);
        }
        return hresult_from_win32(ERROR_FILE_NOT_FOUND);
    };
    let w: Vec<u16> = v.encode_utf16().chain(std::iter::once(0)).collect();
    let p = CoTaskMemAlloc(w.len() * 2) as *mut u16;
    if p.is_null() {
        return 0x8007_000Eu32 as i32; // E_OUTOFMEMORY
    }
    std::ptr::copy_nonoverlapping(w.as_ptr(), p, w.len());
    *version = p;
    0
}

/// The version folder's name is the runtime version ("141.0.3537.71").
fn runtime_version_in(browser_folder: Option<&OsString>) -> Option<String> {
    let (dir, _, _) = find_client_dll(browser_folder)?;
    dir.file_name().map(|n| n.to_string_lossy().into_owned())
}

/// The installed WebView2 runtime's version, or None when Studio's window cannot work (no runtime installed).
pub fn runtime_version() -> Option<String> {
    let folder = env_override("WEBVIEW2_BROWSER_EXECUTABLE_FOLDER", None);
    runtime_version_in(folder.as_ref())
}
