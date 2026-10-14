//! Platform pieces of the Studio window behind one interface.
//! Ownership: this file + linux.rs = UI (Linux) work; windows.rs = UI (Windows) work.
//! To add a function: add it here, implement it in linux.rs, and append a minimal stub at the END of windows.rs.
#[cfg(target_os = "linux")]
mod linux;
#[cfg(target_os = "linux")]
pub use linux::*;
#[cfg(windows)]
mod windows;
#[cfg(windows)]
pub use windows::*;

// Expected functions (both files implement all of them):
//   pub fn single_instance() -> Option<InstanceGuard>   // None: another Studio runs and was asked to show itself
//   pub fn listen_show(show: Box<dyn Fn() + Send>)      // a later launch asks this one to show its window
//   pub fn before_start() -> Option<String>             // Linux: stop the ttlcd user service, return a note
//   pub fn after_exit()
//   pub fn screenshot(w: &tao::window::Window, path: &std::path::Path) -> std::io::Result<()>
//   pub fn ui_base_url() -> &'static str                // custom-protocol base: "ttlcd://localhost/" / "http://ttlcd.localhost/"
