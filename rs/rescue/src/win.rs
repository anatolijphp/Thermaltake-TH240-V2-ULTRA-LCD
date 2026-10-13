//! Windows transport: IOCTL_SCSI_PASS_THROUGH_DIRECT on the disk device interface (found with SetupAPI, which
//! also works when the device has no drive letter).

use crate::{Dev, Pt, Res};
use std::mem::{offset_of, size_of, zeroed};
use windows_sys::core::GUID;
use windows_sys::Win32::Devices::DeviceAndDriverInstallation::*;
use windows_sys::Win32::Foundation::{CloseHandle, GetLastError, HANDLE, INVALID_HANDLE_VALUE};
use windows_sys::Win32::Storage::FileSystem::{CreateFileW, FILE_SHARE_READ, FILE_SHARE_WRITE, OPEN_EXISTING};
use windows_sys::Win32::Storage::IscsiDisc::{IOCTL_SCSI_PASS_THROUGH_DIRECT, SCSI_PASS_THROUGH_DIRECT};
use windows_sys::Win32::System::IO::DeviceIoControl;
use windows_sys::Win32::System::SystemInformation::GetLocalTime;

const DISK_GUID: GUID = GUID::from_u128(0x53f56307_b6bf_11d0_94f2_00a0c91efb8b);

pub fn clock() -> String {
    unsafe {
        let mut t = zeroed();
        GetLocalTime(&mut t);
        format!("{:02}:{:02}:{:02}", t.wHour, t.wMinute, t.wSecond)
    }
}

/// Device paths of every present disk interface
fn disks() -> Vec<String> {
    let mut list = Vec::new();
    unsafe {
        let set = SetupDiGetClassDevsW(&DISK_GUID, std::ptr::null(), std::ptr::null_mut(), DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if set as isize == -1 {
            return list;
        }
        let mut di: SP_DEVICE_INTERFACE_DATA = zeroed();
        di.cbSize = size_of::<SP_DEVICE_INTERFACE_DATA>() as u32;
        let mut i = 0;
        while SetupDiEnumDeviceInterfaces(set, std::ptr::null(), &DISK_GUID, i, &mut di) != 0 {
            i += 1;
            let mut need = 0u32;
            SetupDiGetDeviceInterfaceDetailW(set, &di, std::ptr::null_mut(), 0, &mut need, std::ptr::null_mut());
            if need < 8 {
                continue;
            }
            let mut buf = vec![0u64; (need as usize).div_ceil(8)]; // 8-byte aligned
            let detail = buf.as_mut_ptr() as *mut SP_DEVICE_INTERFACE_DETAIL_DATA_W;
            (*detail).cbSize = size_of::<SP_DEVICE_INTERFACE_DETAIL_DATA_W>() as u32;
            if SetupDiGetDeviceInterfaceDetailW(set, &di, detail, need, &mut need, std::ptr::null_mut()) != 0 {
                let p = (detail as *const u8).add(4) as *const u16;
                let max = (need as usize - 4) / 2;
                let len = (0..max).take_while(|&k| *p.add(k) != 0).count();
                list.push(String::from_utf16_lossy(std::slice::from_raw_parts(p, len)));
            }
        }
        SetupDiDestroyDeviceInfoList(set);
    }
    list
}

pub fn devices() -> Vec<String> {
    disks()
}

pub fn find_loader() -> Option<String> {
    disks().into_iter().find(|p| crate::is_loader(p))
}

pub fn find_writer() -> Option<String> {
    disks().into_iter().find(|p| crate::is_writer(p))
}

pub struct Disk(HANDLE);

impl Drop for Disk {
    fn drop(&mut self) {
        unsafe { CloseHandle(self.0) };
    }
}

pub fn open(path: &str) -> Res<Disk> {
    let w: Vec<u16> = path.encode_utf16().chain([0]).collect();
    let h = unsafe {
        CreateFileW(w.as_ptr(), 0xC000_0000, FILE_SHARE_READ | FILE_SHARE_WRITE, std::ptr::null(), OPEN_EXISTING, 0, std::ptr::null_mut())
    };
    if h == INVALID_HANDLE_VALUE {
        return Err(format!("open {path} failed ({})", unsafe { GetLastError() }));
    }
    Ok(Disk(h))
}

#[repr(C)]
struct Sptdwb {
    s: SCSI_PASS_THROUGH_DIRECT,
    pad: u32,
    sense: [u8; 32],
}

impl Dev for Disk {
    fn pt(&mut self, cdb: &[u8], data: &mut [u8], data_in: bool, timeout_s: u32) -> Pt {
        unsafe {
            let mut w: Sptdwb = zeroed();
            w.s.Length = size_of::<SCSI_PASS_THROUGH_DIRECT>() as u16;
            w.s.TargetId = 1;
            w.s.CdbLength = 10;
            w.s.SenseInfoLength = 0x18;
            w.s.DataIn = data_in as u8;
            w.s.DataTransferLength = data.len() as u32;
            w.s.TimeOutValue = timeout_s;
            w.s.DataBuffer = if data.is_empty() { std::ptr::null_mut() } else { data.as_mut_ptr().cast() };
            w.s.SenseInfoOffset = offset_of!(Sptdwb, sense) as u32;
            w.s.Cdb[..cdb.len()].copy_from_slice(cdb);
            let size = size_of::<Sptdwb>() as u32;
            let mut ret = 0u32;
            let p = &mut w as *mut Sptdwb as *mut core::ffi::c_void;
            let ok = DeviceIoControl(self.0, IOCTL_SCSI_PASS_THROUGH_DIRECT, p, size, p, size, &mut ret, std::ptr::null_mut()) != 0;
            let err = if ok { String::new() } else { GetLastError().to_string() };
            Pt { ok, err, status: w.s.ScsiStatus, sense: w.sense }
        }
    }
}
