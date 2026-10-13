//! Virtual 480x480 display (bundled MttVDD IddCx driver): install / uninstall / status. Port of VirtualDisplay.cs.
use std::io;

pub fn run(action: &str) -> io::Result<u8> {
    let _ = action;
    Err(io::Error::other("not ported yet"))
}
