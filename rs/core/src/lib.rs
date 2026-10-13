//! TT LCD core: the cooler protocol (see docs/protocol.md) and its HID transport, shared by the CLI and Studio.
pub mod hid;
pub mod lcd;

pub use lcd::Lcd;
