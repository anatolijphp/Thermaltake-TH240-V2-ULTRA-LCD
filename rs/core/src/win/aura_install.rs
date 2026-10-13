//! Registration of the Aura HAL plugin TTLcdAura.dll (ASUS LightingService), port of AuraInstall in Aura.cs.
//! Owned by the integrations work. Signatures are used by the CLI and Studio; keep them.
use std::io;

/// `ttlcd aura install|uninstall|status` (install/uninstall elevate themselves).
pub fn run(action: &str) -> io::Result<u8> {
    let _ = action;
    Err(io::Error::other("not ported yet"))
}

/// The plugin is registered (32-bit registry view).
pub fn installed() -> bool {
    false
}
