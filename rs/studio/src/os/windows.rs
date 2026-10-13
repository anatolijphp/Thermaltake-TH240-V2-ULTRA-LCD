//! Windows: single instance via the mutex "TTLcd.Tray.SingleInstance" + event "TTLcd.Tray.Show" (same names as the
//! C# Studio), PrintWindow screenshot, WebView2 specifics.
pub struct InstanceGuard;
pub fn single_instance() -> Option<InstanceGuard> { Some(InstanceGuard) }
pub fn listen_show(_show: Box<dyn Fn() + Send>) {}
pub fn before_start() -> Option<String> { None }
pub fn after_exit() {}
pub fn screenshot(_w: &tao::window::Window, _path: &std::path::Path) -> std::io::Result<()> { Err(std::io::Error::other("todo")) }
pub fn ui_base_url() -> &'static str { "http://ttlcd.localhost/" }
