// TT LCD Studio installer (port of installer/Setup.cs): one exe that carries the application files as an appended
// payload (src/pack.rs, added by ttlcd-pack). Per-user install (no admin) to %LOCALAPPDATA%\Programs\TT LCD Studio,
// Start menu / desktop shortcuts, "start with Windows", an Apps & features entry and an uninstaller (Uninstall.exe,
// the bare setup stub). Optional components that need administrator rights are installed through ttlcd.exe (it asks
// via UAC): the CPU temperature service, the Armoury Crate Aura plugin and the virtual 480x480 display driver.
//
//   TTLcdStudio-Setup.exe                     interactive install
//   TTLcdStudio-Setup.exe /S [/D=<dir>]       silent install (shortcuts + start with Windows, no admin components)
//   Uninstall.exe /uninstall [/S] [/all]      uninstall (from the install folder; Apps & features calls it). /all also
//                                             removes the shared Aura plugin and virtual display (asked when interactive)
//   Uninstall.exe /uninstall-run <dir> ...    internal: the uninstaller's copy in %TEMP% doing the work
#[cfg(windows)]
mod win;

#[cfg(windows)]
fn main() {
    std::process::exit(win::main());
}

#[cfg(not(windows))]
fn main() {
    eprintln!("TTLcdStudio-Setup is the Windows installer; on Linux install the ttlcd / ttlcd-studio .deb instead.");
    std::process::exit(2);
}
