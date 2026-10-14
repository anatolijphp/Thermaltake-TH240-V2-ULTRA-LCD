//! Registry access (both views: the Aura plugin's host, ASUS LightingService, is 32-bit).
use super::wide;
use std::ptr::null_mut;
use windows_sys::Win32::Foundation::ERROR_SUCCESS;
use windows_sys::Win32::System::Registry::*;

pub const HKCU: HKEY = HKEY_CURRENT_USER;
pub const HKLM: HKEY = HKEY_LOCAL_MACHINE;
pub const VIEW32: u32 = KEY_WOW64_32KEY;
pub const VIEW64: u32 = KEY_WOW64_64KEY;

fn open(root: HKEY, path: &str, access: u32) -> Option<HKEY> {
    let mut k: HKEY = null_mut();
    let r = unsafe { RegOpenKeyExW(root, wide(path).as_ptr(), 0, access, &mut k) };
    (r == ERROR_SUCCESS).then_some(k)
}

fn create(root: HKEY, path: &str, view: u32) -> Option<HKEY> {
    let mut k: HKEY = null_mut();
    let r = unsafe { RegCreateKeyExW(root, wide(path).as_ptr(), 0, std::ptr::null(), 0, KEY_ALL_ACCESS | view, std::ptr::null(), &mut k, null_mut()) };
    (r == ERROR_SUCCESS).then_some(k)
}

pub fn key_exists(root: HKEY, path: &str, view: u32) -> bool {
    match open(root, path, KEY_READ | view) {
        Some(k) => {
            unsafe { RegCloseKey(k) };
            true
        }
        None => false,
    }
}

pub fn get_string(root: HKEY, path: &str, name: &str) -> Option<String> {
    get_string_view(root, path, name, 0)
}

pub fn get_string_view(root: HKEY, path: &str, name: &str, view: u32) -> Option<String> {
    let k = open(root, path, KEY_READ | view)?;
    let mut buf = vec![0u16; 4096];
    let mut size = (buf.len() * 2) as u32;
    let mut ty = 0u32;
    let r = unsafe { RegQueryValueExW(k, wide(name).as_ptr(), null_mut(), &mut ty, buf.as_mut_ptr() as *mut u8, &mut size) };
    unsafe { RegCloseKey(k) };
    if r != ERROR_SUCCESS || (ty != REG_SZ && ty != REG_EXPAND_SZ) {
        return None;
    }
    buf.truncate(size as usize / 2);
    Some(super::from_wide(&buf))
}

pub fn set_string(root: HKEY, path: &str, name: &str, value: &str) -> bool {
    set_string_view(root, path, name, value, 0)
}

pub fn set_string_view(root: HKEY, path: &str, name: &str, value: &str, view: u32) -> bool {
    let Some(k) = create(root, path, view) else { return false };
    let v = wide(value);
    let r = unsafe { RegSetValueExW(k, wide(name).as_ptr(), 0, REG_SZ, v.as_ptr() as *const u8, (v.len() * 2) as u32) };
    unsafe { RegCloseKey(k) };
    r == ERROR_SUCCESS
}

pub fn set_dword_view(root: HKEY, path: &str, name: &str, value: u32, view: u32) -> bool {
    let Some(k) = create(root, path, view) else { return false };
    let r = unsafe { RegSetValueExW(k, wide(name).as_ptr(), 0, REG_DWORD, &value as *const u32 as *const u8, 4) };
    unsafe { RegCloseKey(k) };
    r == ERROR_SUCCESS
}

pub fn delete_value(root: HKEY, path: &str, name: &str) {
    if let Some(k) = open(root, path, KEY_SET_VALUE) {
        unsafe {
            RegDeleteValueW(k, wide(name).as_ptr());
            RegCloseKey(k);
        }
    }
}

/// Deletes a key and everything under it.
pub fn delete_tree(root: HKEY, path: &str, view: u32) {
    if let Some(k) = open(root, path, KEY_ALL_ACCESS | view) {
        unsafe {
            RegDeleteTreeW(k, std::ptr::null());
            RegCloseKey(k);
        }
    }
    unsafe { RegDeleteKeyExW(root, wide(path).as_ptr(), view, 0) };
}

pub fn subkeys(root: HKEY, path: &str) -> Vec<String> {
    let Some(k) = open(root, path, KEY_READ) else { return Vec::new() };
    let mut out = Vec::new();
    for i in 0.. {
        let mut buf = [0u16; 512];
        let mut n = buf.len() as u32;
        let r = unsafe { RegEnumKeyExW(k, i, buf.as_mut_ptr(), &mut n, null_mut(), null_mut(), null_mut(), null_mut()) };
        if r != ERROR_SUCCESS {
            break;
        }
        out.push(String::from_utf16_lossy(&buf[..n as usize]));
    }
    unsafe { RegCloseKey(k) };
    out
}

/// Value count + subkey count (to remove a vendor key once it's empty).
pub fn is_empty(root: HKEY, path: &str) -> bool {
    let Some(k) = open(root, path, KEY_READ) else { return false };
    let (mut sub, mut vals) = (0u32, 0u32);
    unsafe {
        RegQueryInfoKeyW(k, null_mut(), null_mut(), null_mut(), &mut sub, null_mut(), null_mut(), &mut vals, null_mut(), null_mut(), null_mut(), null_mut());
        RegCloseKey(k);
    }
    sub == 0 && vals == 0
}
