//! Windows side of the setup: command line, small Win32 helpers. install.rs = install / uninstall, ui.rs = window.
mod install;
mod ui;

use std::ffi::OsStr;
use std::os::windows::ffi::{OsStrExt, OsStringExt};
use std::path::PathBuf;
use windows_sys::Win32::Foundation::{ERROR_SUCCESS, HWND};
use windows_sys::Win32::System::Com::CoTaskMemFree;
use windows_sys::Win32::System::Registry::{
    RegCloseKey, RegCreateKeyExW, RegDeleteKeyW, RegDeleteTreeW, RegDeleteValueW, RegGetValueW, RegOpenKeyExW,
    RegSetValueExW, HKEY, KEY_READ, KEY_WRITE, REG_DWORD, REG_OPTION_NON_VOLATILE, REG_SAM_FLAGS, REG_SZ, RRF_RT_REG_EXPAND_SZ,
    RRF_RT_REG_SZ, RRF_NOEXPAND,
};
use windows_sys::Win32::UI::Shell::SHGetKnownFolderPath;
use windows_sys::Win32::UI::WindowsAndMessaging::{
    MessageBoxW, IDYES, MB_ICONERROR, MB_ICONINFORMATION, MB_ICONQUESTION, MB_OK, MB_YESNO, MESSAGEBOX_STYLE,
};

pub const APP_NAME: &str = "TT LCD Studio";
pub const VERSION: &str = env!("CARGO_PKG_VERSION");

pub fn main() -> i32 {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let has = |f: &str| args.iter().any(|a| a.eq_ignore_ascii_case(f));
    let silent = has("/S");
    let all = has("/all"); // uninstall: also the shared Aura plugin / display driver
    let r = (|| -> Result<i32, String> {
        if has("/uninstall") {
            return install::uninstall_start(silent, all);
        }
        if has("/uninstall-run") {
            let i = args.iter().position(|a| a.eq_ignore_ascii_case("/uninstall-run")).unwrap();
            return install::uninstall_run(args.get(i + 1).map(String::as_str), silent, all);
        }
        if silent {
            let dir = args
                .iter()
                .find(|a| a.len() >= 3 && a[..3].eq_ignore_ascii_case("/D="))
                .map(|a| PathBuf::from(a[3..].trim_matches('"')))
                .unwrap_or_else(install::default_dir);
            let o = install::Options { dir, start_menu: true, desktop: false, autostart: true, ..Default::default() };
            install::com_init(); // shortcuts
            install::install(&o, &|s: &str| println!("{s}"))?;
            return Ok(0);
        }
        ui::run()
    })();
    match r {
        Ok(code) => code,
        Err(e) => {
            if silent {
                eprintln!("error: {e}");
            } else {
                message(std::ptr::null_mut(), &e, &format!("{APP_NAME} setup"), MB_OK | MB_ICONERROR);
            }
            1
        }
    }
}

// ---------------- helpers ----------------

pub fn wide(s: impl AsRef<OsStr>) -> Vec<u16> {
    s.as_ref().encode_wide().chain(std::iter::once(0)).collect()
}

pub fn message(owner: HWND, text: &str, title: &str, style: MESSAGEBOX_STYLE) -> i32 {
    unsafe { MessageBoxW(owner, wide(text).as_ptr(), wide(title).as_ptr(), style) }
}

pub fn ask(text: &str) -> bool {
    message(std::ptr::null_mut(), text, APP_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES
}

pub fn inform(text: &str) {
    message(std::ptr::null_mut(), text, APP_NAME, MB_OK | MB_ICONINFORMATION);
}

/// A shell known folder (FOLDERID_*).
pub fn known_folder(id: &windows_sys::core::GUID) -> Option<PathBuf> {
    unsafe {
        let mut p: windows_sys::core::PWSTR = std::ptr::null_mut();
        let hr = SHGetKnownFolderPath(id, 0, std::ptr::null_mut(), &mut p);
        let r = if hr >= 0 && !p.is_null() {
            let mut n = 0;
            while *p.add(n) != 0 {
                n += 1;
            }
            Some(PathBuf::from(std::ffi::OsString::from_wide(std::slice::from_raw_parts(p, n))))
        } else {
            None
        };
        if !p.is_null() {
            CoTaskMemFree(p as *const _);
        }
        r
    }
}

/// An open registry key, closed on drop.
pub struct Key(HKEY);

impl Drop for Key {
    fn drop(&mut self) {
        unsafe { RegCloseKey(self.0) };
    }
}

impl Key {
    pub fn create(root: HKEY, path: &str) -> Result<Key, String> {
        let mut h: HKEY = std::ptr::null_mut();
        let r = unsafe {
            RegCreateKeyExW(
                root,
                wide(path).as_ptr(),
                0,
                std::ptr::null(),
                REG_OPTION_NON_VOLATILE,
                KEY_READ | KEY_WRITE,
                std::ptr::null(),
                &mut h,
                std::ptr::null_mut(),
            )
        };
        if r != ERROR_SUCCESS {
            return Err(format!("cannot open registry key {path} (error {r})"));
        }
        Ok(Key(h))
    }

    pub fn open(root: HKEY, path: &str, extra: REG_SAM_FLAGS) -> Option<Key> {
        let mut h: HKEY = std::ptr::null_mut();
        let r = unsafe { RegOpenKeyExW(root, wide(path).as_ptr(), 0, KEY_READ | extra, &mut h) };
        (r == ERROR_SUCCESS).then_some(Key(h))
    }

    pub fn set_str(&self, name: &str, value: &str) -> Result<(), String> {
        let v = wide(value);
        let r = unsafe {
            RegSetValueExW(self.0, wide(name).as_ptr(), 0, REG_SZ, v.as_ptr().cast(), (v.len() * 2) as u32)
        };
        if r == ERROR_SUCCESS {
            Ok(())
        } else {
            Err(format!("cannot write registry value {name} (error {r})"))
        }
    }

    pub fn set_dword(&self, name: &str, value: u32) -> Result<(), String> {
        let r = unsafe {
            RegSetValueExW(self.0, wide(name).as_ptr(), 0, REG_DWORD, (&value as *const u32).cast(), 4)
        };
        if r == ERROR_SUCCESS {
            Ok(())
        } else {
            Err(format!("cannot write registry value {name} (error {r})"))
        }
    }

    /// A REG_SZ / REG_EXPAND_SZ value as stored (not expanded).
    pub fn get_str(&self, name: &str) -> Option<String> {
        let n = wide(name);
        let flags = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;
        let mut size = 0u32;
        unsafe {
            if RegGetValueW(self.0, std::ptr::null(), n.as_ptr(), flags, std::ptr::null_mut(), std::ptr::null_mut(), &mut size)
                != ERROR_SUCCESS
            {
                return None;
            }
            let mut buf = vec![0u16; size as usize / 2 + 1];
            size = (buf.len() * 2) as u32;
            if RegGetValueW(self.0, std::ptr::null(), n.as_ptr(), flags, std::ptr::null_mut(), buf.as_mut_ptr().cast(), &mut size)
                != ERROR_SUCCESS
            {
                return None;
            }
            let len = buf.iter().position(|&c| c == 0).unwrap_or(buf.len());
            Some(String::from_utf16_lossy(&buf[..len]))
        }
    }

    pub fn delete_value(&self, name: &str) {
        unsafe { RegDeleteValueW(self.0, wide(name).as_ptr()) };
    }
}

/// Deletes a key with everything below it.
pub fn delete_key_tree(root: HKEY, path: &str) {
    let p = wide(path);
    unsafe {
        RegDeleteTreeW(root, p.as_ptr());
        RegDeleteKeyW(root, p.as_ptr());
    }
}
