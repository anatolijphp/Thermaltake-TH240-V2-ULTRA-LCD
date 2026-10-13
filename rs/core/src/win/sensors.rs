//! CPU temperature helper (`ttlcd hwmon`) and the TTLcdSensors Windows service, port of Sensors.cs: loads TOWEI's
//! XYDataCenter.dll and publishes to "Global\TTLcdSensors" (layout in crate::sensors). Owned by the integrations
//! work. Signatures are used by the CLI, Studio and platform.rs; keep them.
use std::io;

/// `ttlcd service install|uninstall|status`
pub fn run_service_cmd(action: &str) -> io::Result<u8> {
    let _ = action;
    Err(io::Error::other("not ported yet"))
}

/// `ttlcd hwmon`: publish until stop() returns true.
pub fn run_helper(stop: &dyn Fn() -> bool) -> io::Result<u8> {
    let _ = stop;
    Err(io::Error::other("not ported yet"))
}

/// `ttlcd sensor-service`: entry point when started by the service control manager.
pub fn service_main() -> io::Result<u8> {
    Err(io::Error::other("not ported yet"))
}

pub fn service_installed() -> bool {
    false
}

pub fn service_running() -> bool {
    false
}
