//! Linux: single instance via 127.0.0.1:47913 ("ttlcd show"), ttlcd user-service handover, X11 screenshot.
pub struct InstanceGuard;
pub fn single_instance() -> Option<InstanceGuard> { Some(InstanceGuard) }
pub fn listen_show(_show: Box<dyn Fn() + Send>) {}
pub fn before_start() -> Option<String> { None }
pub fn after_exit() {}
pub fn screenshot(_w: &tao::window::Window, _path: &std::path::Path) -> std::io::Result<()> { Err(std::io::Error::other("todo")) }
pub fn ui_base_url() -> &'static str { "ttlcd://localhost/" }
