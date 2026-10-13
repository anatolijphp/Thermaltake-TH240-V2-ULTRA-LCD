// SetupAPI enumeration + overlapped I/O on the HID interface paths (\\?\hid#vid_1a2c&pid_151d&mi_00#...).
use super::DevInfo;
use std::io;
use std::ptr::{null, null_mut};
use windows_sys::core::GUID;
use windows_sys::Win32::Devices::DeviceAndDriverInstallation::*;
use windows_sys::Win32::Devices::HumanInterfaceDevice::HidD_GetHidGuid;
use windows_sys::Win32::Foundation::*;
use windows_sys::Win32::Storage::FileSystem::*;
use windows_sys::Win32::System::IO::*;
use windows_sys::Win32::System::Threading::*;

fn wide(s: &str) -> Vec<u16> {
    s.encode_utf16().chain(std::iter::once(0)).collect()
}

fn interface_of(path: &str) -> i32 {
    path.find("&mi_")
        .and_then(|i| path.get(i + 4..i + 6))
        .and_then(|s| i32::from_str_radix(s, 16).ok())
        .unwrap_or(-1)
}

pub fn enumerate(vid: u16, pid: u16) -> Vec<DevInfo> {
    let mut list = Vec::new();
    let want = format!("vid_{vid:04x}&pid_{pid:04x}");
    unsafe {
        let mut g: GUID = std::mem::zeroed();
        HidD_GetHidGuid(&mut g);
        let set = SetupDiGetClassDevsW(&g, null(), null_mut(), DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if set as isize == INVALID_HANDLE_VALUE as isize { return list; }
        let mut d: SP_DEVICE_INTERFACE_DATA = std::mem::zeroed();
        d.cbSize = std::mem::size_of::<SP_DEVICE_INTERFACE_DATA>() as u32;
        let mut i = 0;
        while SetupDiEnumDeviceInterfaces(set, null(), &g, i, &mut d) != 0 {
            i += 1;
            let mut req = 0u32;
            SetupDiGetDeviceInterfaceDetailW(set, &d, null_mut(), 0, &mut req, null_mut());
            if req < 8 { continue; }
            let mut buf = vec![0u32; (req as usize).div_ceil(4)]; // u32 for the struct's alignment
            let detail = buf.as_mut_ptr() as *mut SP_DEVICE_INTERFACE_DETAIL_DATA_W;
            (*detail).cbSize = std::mem::size_of::<SP_DEVICE_INTERFACE_DETAIL_DATA_W>() as u32; // 8 on x64
            if SetupDiGetDeviceInterfaceDetailW(set, &d, detail, req, &mut req, null_mut()) == 0 { continue; }
            let p = (detail as *const u8).add(4) as *const u16;
            let mut n = 0;
            while *p.add(n) != 0 { n += 1; }
            let path = String::from_utf16_lossy(std::slice::from_raw_parts(p, n));
            let lp = path.to_lowercase();
            if !lp.contains(&want) { continue; }
            list.push(DevInfo { interface: interface_of(&lp), path, vid, pid });
        }
        SetupDiDestroyDeviceInfoList(set);
    }
    list.sort_by_key(|d| d.interface);
    list
}

/// A HID handle opened for overlapped I/O. A read that times out stays pending and is collected by the next
/// `read`: cancelling it would let the cooler's next reply land in a dead buffer, so every later query times out.
pub struct Dev {
    h: HANDLE,
    rd_ev: HANDLE,
    wr_ev: HANDLE,
    rd_ov: Box<OVERLAPPED>,
    rd_buf: Box<[u8; 2048]>,
    pending: bool,
}

unsafe impl Send for Dev {}
unsafe impl Sync for Dev {}

impl Dev {
    pub fn open(path: &str) -> io::Result<Dev> {
        unsafe {
            let h = CreateFileW(
                wide(path).as_ptr(),
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                null(),
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                null_mut(),
            );
            if h == INVALID_HANDLE_VALUE {
                let e = io::Error::last_os_error();
                return Err(io::Error::new(e.kind(), format!("cannot open HID device (in use by TT RGB Plus?): {e}")));
            }
            Ok(Dev {
                h,
                rd_ev: CreateEventW(null(), 1, 0, null()),
                wr_ev: CreateEventW(null(), 1, 0, null()),
                rd_ov: Box::new(std::mem::zeroed()),
                rd_buf: Box::new([0; 2048]),
                pending: false,
            })
        }
    }

    /// Windows shares the device between processes; the `Global\TTLcd.Device` mutex in `Lcd` is the guard there.
    pub fn try_lock(&self) -> bool {
        true
    }

    /// [report ID 0][packet]; the length must equal the interface's output report length (441 / 1025).
    pub fn write(&mut self, report: &[u8]) -> io::Result<()> {
        unsafe {
            let mut ov: OVERLAPPED = std::mem::zeroed();
            ov.hEvent = self.wr_ev;
            ResetEvent(self.wr_ev);
            let mut n = 0u32;
            if WriteFile(self.h, report.as_ptr(), report.len() as u32, null_mut(), &mut ov) == 0
                && GetLastError() != ERROR_IO_PENDING
            {
                return Err(io::Error::last_os_error());
            }
            if GetOverlappedResult(self.h, &ov, &mut n, 1) == 0 { return Err(io::Error::last_os_error()); }
            Ok(())
        }
    }

    /// Ok(None) on timeout (the read stays pending). Returns the report without the report-ID byte.
    pub fn read(&mut self, buf: &mut [u8], timeout_ms: u32) -> io::Result<Option<usize>> {
        unsafe {
            if !self.pending {
                *self.rd_ov = std::mem::zeroed();
                self.rd_ov.hEvent = self.rd_ev;
                ResetEvent(self.rd_ev);
                let len = (buf.len() + 1).min(self.rd_buf.len()) as u32;
                if ReadFile(self.h, self.rd_buf.as_mut_ptr(), len, null_mut(), &mut *self.rd_ov) == 0
                    && GetLastError() != ERROR_IO_PENDING
                {
                    return Err(io::Error::last_os_error());
                }
                self.pending = true;
            }
            if WaitForSingleObject(self.rd_ev, timeout_ms) == WAIT_TIMEOUT { return Ok(None); }
            self.pending = false;
            let mut n = 0u32;
            if GetOverlappedResult(self.h, &*self.rd_ov, &mut n, 0) == 0 { return Err(io::Error::last_os_error()); }
            let n = (n as usize).saturating_sub(1).min(buf.len());
            buf[..n].copy_from_slice(&self.rd_buf[1..1 + n]);
            Ok(Some(n))
        }
    }
}

impl Drop for Dev {
    fn drop(&mut self) {
        unsafe {
            if self.pending {
                let mut n = 0u32;
                CancelIoEx(self.h, &*self.rd_ov);
                GetOverlappedResult(self.h, &*self.rd_ov, &mut n, 1);
            }
            CloseHandle(self.h);
            CloseHandle(self.rd_ev);
            CloseHandle(self.wr_ev);
        }
    }
}

/// `Global\TTLcd.Device`, the same mutex the C# Studio and ttlcd.exe hold, so old and new builds exclude each other.
pub struct DeviceMutex(HANDLE);

unsafe impl Send for DeviceMutex {}
unsafe impl Sync for DeviceMutex {}

impl DeviceMutex {
    pub fn acquire() -> io::Result<DeviceMutex> {
        unsafe {
            let m = CreateMutexW(null(), 0, wide("Global\\TTLcd.Device").as_ptr());
            if m.is_null() { return Err(io::Error::last_os_error()); }
            match WaitForSingleObject(m, 0) {
                WAIT_OBJECT_0 | WAIT_ABANDONED => Ok(DeviceMutex(m)), // abandoned: previous owner was killed
                _ => {
                    CloseHandle(m);
                    Err(io::Error::new(io::ErrorKind::WouldBlock, "Cooler is in use by another TT LCD program (Studio or ttlcd)."))
                }
            }
        }
    }
}

impl Drop for DeviceMutex {
    fn drop(&mut self) {
        unsafe {
            ReleaseMutex(self.0);
            CloseHandle(self.0);
        }
    }
}
