//! TT LCD core: the cooler protocol and HID transport, settings, rendering, the screen engine and the Studio logic,
//! shared by the `ttlcd` command-line tool and TT LCD Studio on Windows and Linux.
pub mod aura;
pub mod capture;
pub mod config;
pub mod engine;
pub mod games;
pub mod hid;
pub mod idle;
pub mod lcd;
pub mod media;
pub mod model;
pub mod platform;
pub mod render;
pub mod sensors;
pub mod store;
pub mod util;
pub mod video;
pub mod xml;
#[cfg(windows)]
pub mod win;

pub use config::{Config, Mode};
pub use engine::Engine;
pub use lcd::Lcd;

/// Version shown in the apps and the installer (Apps & features); bump for releases.
pub const VERSION: &str = env!("CARGO_PKG_VERSION");
