//! HID access with no external library: hidraw on Linux, SetupAPI + overlapped ReadFile/WriteFile on Windows.
//! Both sides use the same shape: `write` takes [report ID 0][packet], `read` returns the report without the ID.

#[cfg(target_os = "linux")]
mod linux;
#[cfg(target_os = "linux")]
pub use linux::{enumerate, Dev};

#[cfg(windows)]
mod windows;
#[cfg(windows)]
pub use self::windows::{enumerate, Dev, DeviceMutex};

/// One HID interface of a USB device. `interface` is the USB interface number (MI_0N on Windows, "inputN" on Linux).
#[derive(Debug, Clone)]
pub struct DevInfo {
    pub path: String,
    pub vid: u16,
    pub pid: u16,
    pub interface: i32,
}
