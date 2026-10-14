// hidraw: /sys/class/hidraw/hidrawN/device/uevent has HID_ID=0003:00001A2C:0000151D (bus:vid:pid) and
// HID_PHYS=usb-0000:00:14.0-2/input0, whose "inputN" is the USB interface number.
use super::DevInfo;
use std::fs::{self, File, OpenOptions};
use std::io::{self, Read, Write};
use std::os::fd::AsRawFd;

pub fn enumerate(vid: u16, pid: u16) -> Vec<DevInfo> {
    let mut list = Vec::new();
    let Ok(dir) = fs::read_dir("/sys/class/hidraw") else { return list };
    for e in dir.flatten() {
        let Ok(uevent) = fs::read_to_string(e.path().join("device/uevent")) else { continue };
        let (mut id, mut phys) = (None, None);
        for line in uevent.lines() {
            if let Some(v) = line.strip_prefix("HID_ID=") { id = Some(v.to_owned()); }
            if let Some(v) = line.strip_prefix("HID_PHYS=") { phys = Some(v.to_owned()); }
        }
        let Some(id) = id else { continue };
        let p: Vec<&str> = id.split(':').collect();
        if p.len() < 3 { continue; }
        let (Ok(v), Ok(d)) = (u32::from_str_radix(p[1], 16), u32::from_str_radix(p[2], 16)) else { continue };
        if v != vid as u32 || d != pid as u32 { continue; }
        let interface = phys
            .as_deref()
            .and_then(|s| s.rfind("/input").map(|k| &s[k + 6..]))
            .and_then(|s| s.parse().ok())
            .unwrap_or(-1);
        list.push(DevInfo { path: format!("/dev/{}", e.file_name().to_string_lossy()), vid, pid, interface });
    }
    list.sort_by_key(|d| d.interface);
    list
}

pub struct Dev {
    f: File,
}

impl Dev {
    pub fn open(path: &str) -> io::Result<Dev> {
        match OpenOptions::new().read(true).write(true).open(path) {
            Ok(f) => Ok(Dev { f }),
            Err(e) if e.kind() == io::ErrorKind::PermissionDenied => Err(io::Error::new(
                e.kind(),
                format!("no permission for {path}: install the udev rule (the .deb does), then replug the cooler or reboot"),
            )),
            Err(e) => Err(e),
        }
    }

    /// Exclusive advisory lock, so two ttlcd processes never interleave frames (that can hang the cooler's decoder).
    pub fn try_lock(&self) -> bool {
        unsafe { libc::flock(self.f.as_raw_fd(), libc::LOCK_EX | libc::LOCK_NB) == 0 }
    }

    /// One write() per report: [report ID 0][packet].
    pub fn write(&mut self, report: &[u8]) -> io::Result<()> {
        self.f.write_all(report)
    }

    /// Ok(None) on timeout. Nothing stays pending here (poll before read), unlike Windows.
    pub fn read(&mut self, buf: &mut [u8], timeout_ms: u32) -> io::Result<Option<usize>> {
        let mut p = libc::pollfd { fd: self.f.as_raw_fd(), events: libc::POLLIN, revents: 0 };
        let r = unsafe { libc::poll(&mut p, 1, timeout_ms as i32) };
        if r < 0 { return Err(io::Error::last_os_error()); }
        if r == 0 { return Ok(None); }
        // hidraw returns the report without the report-ID byte when the device has no numbered reports
        Ok(Some(self.f.read(buf)?))
    }
}
