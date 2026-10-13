//! Linux transport: SCSI generic (/dev/sgN) with the SG_IO ioctl. The recovery loader and the writer show up as
//! USB mass storage (usb-storage). Found through /sys/bus/scsi/devices/H:C:T:L/{vendor,model} and the USB
//! idVendor/idProduct further up the sysfs tree; opened as /dev/sgN, or as /dev/sdX when the sg driver isn't loaded.
//! Sending vendor CDBs through SG_IO needs CAP_SYS_RAWIO (run as root). Untested on hardware so far.

use crate::{Dev, Pt, Res};
use std::fs::{File, OpenOptions};
use std::os::fd::AsRawFd;
use std::os::unix::fs::OpenOptionsExt;
use std::path::Path;

pub fn clock() -> String {
    unsafe {
        let t = libc::time(std::ptr::null_mut());
        let mut tm: libc::tm = std::mem::zeroed();
        libc::localtime_r(&t, &mut tm);
        format!("{:02}:{:02}:{:02}", tm.tm_hour, tm.tm_min, tm.tm_sec)
    }
}

fn read_trim(p: &Path) -> String {
    std::fs::read_to_string(p).map(|s| s.trim().to_string()).unwrap_or_default()
}

/// (node, description) for every SCSI device: "/dev/sg2", "W55FA93 DEV TOOL vid_0416&pid_9396". The node is the
/// sg one when the sg driver is loaded, else the block one (/dev/sdX also takes SG_IO, even with no medium).
fn list() -> Vec<(String, String)> {
    let mut v = Vec::new();
    let Ok(rd) = std::fs::read_dir("/sys/bus/scsi/devices") else { return v };
    for e in rd.flatten() {
        let dev = e.path();
        if !dev.join("vendor").is_file() {
            continue; // hosts and targets; only H:C:T:L devices have vendor/model
        }
        let first = |sub: &str| {
            std::fs::read_dir(dev.join(sub)).ok()?.flatten().next().map(|e| e.file_name().to_string_lossy().into_owned())
        };
        let Some(node) = first("scsi_generic").or_else(|| first("block")) else { continue };
        let mut desc = format!("{} {}", read_trim(&dev.join("vendor")), read_trim(&dev.join("model")));
        // walk up from the SCSI device to the USB device that owns it
        if let Ok(mut p) = std::fs::canonicalize(&dev) {
            while p.pop() && p != Path::new("/sys") {
                if p.join("idVendor").is_file() {
                    desc += &format!(" vid_{}&pid_{}", read_trim(&p.join("idVendor")), read_trim(&p.join("idProduct")));
                    break;
                }
            }
        }
        v.push((format!("/dev/{node}"), desc));
    }
    v.sort();
    v
}

pub fn devices() -> Vec<String> {
    list().into_iter().map(|(n, d)| format!("{n}: {d}")).collect()
}

pub fn find_loader() -> Option<String> {
    list().into_iter().find(|(_, d)| crate::is_loader(d)).map(|(n, _)| n)
}

pub fn find_writer() -> Option<String> {
    list().into_iter().find(|(_, d)| crate::is_writer(d)).map(|(n, _)| n)
}

pub struct Sg(File);

pub fn open(path: &str) -> Res<Sg> {
    let node = path.split(':').next().unwrap_or(path);
    OpenOptions::new()
        .read(true)
        .write(true)
        .custom_flags(libc::O_NONBLOCK)
        .open(node)
        .map(Sg)
        .map_err(|e| format!("open {node} failed ({e})"))
}

// <scsi/sg.h>
#[repr(C)]
struct SgIoHdr {
    interface_id: i32,
    dxfer_direction: i32,
    cmd_len: u8,
    mx_sb_len: u8,
    iovec_count: u16,
    dxfer_len: u32,
    dxferp: *mut libc::c_void,
    cmdp: *const u8,
    sbp: *mut u8,
    timeout: u32,
    flags: u32,
    pack_id: i32,
    usr_ptr: *mut libc::c_void,
    status: u8,
    masked_status: u8,
    msg_status: u8,
    sb_len_wr: u8,
    host_status: u16,
    driver_status: u16,
    resid: i32,
    duration: u32,
    info: u32,
}
const SG_IO: libc::c_ulong = 0x2285;
const SG_DXFER_NONE: i32 = -1;
const SG_DXFER_TO_DEV: i32 = -2;
const SG_DXFER_FROM_DEV: i32 = -3;

impl Dev for Sg {
    fn pt(&mut self, cdb: &[u8], data: &mut [u8], data_in: bool, timeout_s: u32) -> Pt {
        let mut c = [0u8; 16];
        c[..cdb.len()].copy_from_slice(cdb);
        let mut sense = [0u8; 32];
        let mut h = SgIoHdr {
            interface_id: b'S' as i32,
            dxfer_direction: if data.is_empty() {
                SG_DXFER_NONE
            } else if data_in {
                SG_DXFER_FROM_DEV
            } else {
                SG_DXFER_TO_DEV
            },
            cmd_len: 10, // every request in this protocol is a 10-byte CDB, as in TurboWriter
            mx_sb_len: 0x18,
            iovec_count: 0,
            dxfer_len: data.len() as u32,
            dxferp: if data.is_empty() { std::ptr::null_mut() } else { data.as_mut_ptr().cast() },
            cmdp: c.as_ptr(),
            sbp: sense.as_mut_ptr(),
            timeout: timeout_s.saturating_mul(1000),
            flags: 0,
            pack_id: 0,
            usr_ptr: std::ptr::null_mut(),
            status: 0,
            masked_status: 0,
            msg_status: 0,
            sb_len_wr: 0,
            host_status: 0,
            driver_status: 0,
            resid: 0,
            duration: 0,
            info: 0,
        };
        let rc = unsafe { libc::ioctl(self.0.as_raw_fd(), SG_IO as _, &mut h as *mut SgIoHdr) };
        if rc < 0 {
            let e = std::io::Error::last_os_error();
            return Pt { ok: false, err: e.to_string(), status: 0, sense };
        }
        // Like DeviceIoControl on Windows: a CHECK CONDITION still counts as delivered (driver_status then only
        // carries DRIVER_SENSE, 0x08); a transport error (host_status) or driver error/timeout doesn't.
        if h.host_status != 0 || (h.driver_status & 0x07) != 0 {
            return Pt {
                ok: false,
                err: format!("host status 0x{:X}, driver status 0x{:X}", h.host_status, h.driver_status),
                status: h.status,
                sense,
            };
        }
        Pt { ok: true, err: String::new(), status: h.status, sense }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn layout() {
        // struct sg_io_hdr is 88 bytes on 64-bit Linux
        assert_eq!(std::mem::size_of::<super::SgIoHdr>(), if cfg!(target_pointer_width = "64") { 88 } else { 64 });
    }
}
