//! What Studio shows and decides, without any UI toolkit (port of StudioModel.cs and Readings.cs): texts, which option
//! panels a mode needs, rotation math, dashboard readings, the reading lists and the drag-and-drop line logic.
use crate::config::{Config, ElementStyle, GameAction, Mode};
use crate::engine::{self, Engine, MANUAL_GAME};
use crate::render::readings::{self, CLOCK_IDS, OVERLAY_IDS};
use crate::{platform, sensors, util, video};
use serde::Serialize;

/// Modes this system can show. Linux has no screen capture, virtual display or video decoder.
pub fn supports(m: Mode) -> bool {
    cfg!(windows) || matches!(m, Mode::Image | Mode::Gif | Mode::Color | Mode::Clock | Mode::Aura | Mode::Off)
}

// ---- modes ----
pub fn mode_description(m: Mode) -> String {
    let linux = cfg!(target_os = "linux");
    match m {
        Mode::VirtualDisplay => "A virtual 480×480 monitor is mirrored to the cooler. In Wallpaper Engine, choose that monitor and pick a wallpaper for it.".into(),
        Mode::Monitor => "Mirrors one of your monitors, or a part of it, to the cooler.".into(),
        Mode::Image => "Shows a still picture on the cooler.".into(),
        Mode::Gif => "Plays an animated GIF on the cooler in a loop.".into(),
        Mode::Video => "Plays a clip of up to 30 seconds in a loop. Choose the part to play, then zoom and pan to frame it.".into(),
        Mode::Color => "Fills the screen with one colour.".into(),
        Mode::Clock => "Time, date and sensor readings, arranged and styled below.".into(),
        Mode::Aura if linux && !crate::aura::linux_service_installed() => "Needs armory-crate, the Linux Aura Sync service, which isn't installed. Install it (./packaging/install.sh in its folder), then run: systemctl --user enable --now armory-crated and armory-crate virtual add ttlcd. This mode turns on by itself once it's there.".into(),
        Mode::Aura if linux => "Follows Aura Sync from armory-crate: its effect and colours drive an abstract visualiser on the LCD. Add the screen there once with: armory-crate virtual add ttlcd".into(),
        Mode::Aura => "Adds the cooler screen to Armoury Crate as an Aura Sync device (\"TT LCD CL-W383\"). Its effects and colours drive an abstract visualiser on the LCD. Installing needs administrator rights and restarts the ASUS lighting service.".into(),
        Mode::Off => "The screen is blanked.".into(),
    }
}

pub fn options_title(m: Mode) -> String {
    format!("{} options", engine::label(m)).to_uppercase()
}

/// Which option panels the mode (and overlay choice) needs. selected_visible: a reading is selected for styling and
/// belongs to what this mode shows.
pub fn panels(cfg: &Config, selected_visible: bool) -> Vec<(&'static str, bool)> {
    let m = cfg.Mode;
    let overlay = m != Mode::Clock && m != Mode::Off;
    let overlay_on = cfg.OverlayTime || cfg.OverlayWaterTemp || cfg.OverlayCpuTemp;
    vec![
        ("OptVirtualDisplay", m == Mode::VirtualDisplay),
        ("OptMonitor", m == Mode::Monitor),
        ("OptImage", m == Mode::Image),
        ("OptGif", m == Mode::Gif),
        ("OptVideo", m == Mode::Video),
        ("OptOff", m == Mode::Off),
        ("OptAura", m == Mode::Aura),
        ("OptClock", m == Mode::Clock),
        ("OptScaling", matches!(m, Mode::Image | Mode::Gif | Mode::Monitor)),
        ("OptBg", matches!(m, Mode::Image | Mode::Gif | Mode::Color | Mode::Clock)),
        ("OptOverlay", overlay),
        ("OvDetails", overlay_on),
        ("OptText", selected_visible),
    ]
}

// ---- rotation ----
/// The live rotation at which the picture is upright on this mount.
pub fn mount(c: &Config) -> i32 {
    if c.PreviewMount < 0 { c.Rotation } else { c.PreviewMount }
}
/// Rotation as the user sees it: 0° = upright on this mount (the stored Rotation is the software turn that achieves
/// it, e.g. 270° here).
pub fn user_rotation(c: &Config) -> i32 {
    ((c.Rotation - mount(c)) % 360 + 360) % 360
}
/// Stored Rotation for a user rotation (0/90/180/270 relative to upright).
pub fn rotation_for(c: &Config, user: i32) -> i32 {
    (mount(c) + user) % 360
}
pub fn next_user_rotation(c: &Config) -> i32 {
    (user_rotation(c) + 90) % 360
}
/// How a cooler is recognised: "VID:PID/serial", e.g. "1A2C:151D/A247392SS000000".
pub fn device_key(usb_id: (u16, u16), serial: &str) -> String {
    format!("{:04X}:{:04X}/{}", usb_id.0, usb_id.1, serial.trim_matches(|ch: char| ch == '\0' || ch.is_whitespace()))
}

/// Calibration measured on a cooler: the live Rotation at which frames are upright (PreviewMount) and the cooler's
/// stored rotation that shows its own idle screen upright then (HwUpright).
pub struct KnownMount {
    pub device: &'static str,
    pub preview_mount: i32,
    pub hw_upright: i32,
}

pub const KNOWN_MOUNTS: &[KnownMount] = &[
    // TH240 V2 Ultra on the LX800 firmware with the TT021 panel patch (CLAUDE.md), checked on the LCD 2026-10-14
    KnownMount { device: "1A2C:151D/A247392SS000000", preview_mount: 270, hw_upright: 90 },
];

/// Applies the known calibration for `device` once (Config.CalibratedFor): the picture becomes upright and the
/// cooler's idle screen follows. False when there is none or it was applied before.
pub fn apply_known_mount(c: &mut Config, device: &str) -> bool {
    let Some(m) = KNOWN_MOUNTS.iter().find(|m| m.device.eq_ignore_ascii_case(device)) else { return false };
    if c.CalibratedFor.eq_ignore_ascii_case(device) {
        return false;
    }
    c.PreviewMount = m.preview_mount;
    c.Rotation = m.preview_mount;
    c.HwUpright = m.hw_upright;
    c.CalibratedFor = device.to_string();
    true
}

/// The cooler draws its idle picture turned like the live frames: Rotation - mount from upright.
pub fn idle_preview_angle(c: &Config) -> i32 {
    user_rotation(c)
}

// ---- status texts ----
pub fn conn_text(e: &Engine) -> &'static str {
    if e.connected() { "Cooler connected" } else { "Cooler not connected" }
}
pub fn fps_text(e: &Engine) -> String {
    if e.connected() { format!("{:.0} FPS", e.fps()) } else { String::new() }
}
pub fn mode_text(e: &Engine) -> String {
    if e.connected() { e.render_status() } else { e.device_status() }
}
pub fn device_status(e: &Engine) -> String {
    if e.connected() { "Connected".into() } else { e.device_status() }
}
pub fn serial_text(e: &Engine) -> String {
    let s = e.serial();
    if s.is_empty() { "—".into() } else { s }
}
pub fn nav_serial(e: &Engine) -> String {
    let s = e.serial();
    if s.is_empty() { String::new() } else { format!("S/N {s}") }
}
pub fn title_status(e: &Engine) -> String {
    let c = e.config();
    let game = e.game();
    format!(
        "{}{}{}{}",
        if e.connected() { "● " } else { "○ " },
        engine::label(c.Mode),
        if e.suspended() { " · cooler showing its idle screen" } else if e.paused() { " · paused" } else { "" },
        match game.as_deref() {
            Some(MANUAL_GAME) => " · game mode".to_string(),
            Some(g) => format!(" · game: {g}"),
            None => String::new(),
        }
    )
}

/// Live game-mode status line (Settings).
pub fn game_status(e: &Engine) -> String {
    let c = e.config();
    let Some(g) = e.game() else {
        if !platform::game_detection() {
            return "Automatic game detection needs Windows; game mode can be switched on by hand.".into();
        }
        if !c.GameDetect {
            return "Game detection is off.".into();
        }
        let known = crate::games::known_game_count();
        return format!("No game running.{}", if c.GameKnownList && known >= 0 { format!(" Windows knows {known} games on this PC.") } else { String::new() });
    };
    let act = match c.GameAction {
        GameAction::LowerFps => format!("streaming at {} fps", c.GameFps),
        GameAction::IdleScreen => "streaming stopped, the cooler shows its idle screen".into(),
        GameAction::None => "no change".into(),
    };
    let who = if g == MANUAL_GAME { "Game mode on (forced)".to_string() } else { format!("Game running: {g}") };
    format!("{who} · {act}, sensors every {} s.", c.GameSensorMs / 1000)
}

pub fn cache_info() -> String {
    format!("Video cache: {:.1} MB · Settings: {}", video::cache_bytes() as f64 / 1048576.0, crate::config::folder().display())
}

// ---- dashboard ----
#[derive(Serialize, Clone, Debug)]
pub struct Gauge {
    pub text: String,
    pub frac: f64,
}

#[derive(Serialize, Clone, Debug)]
pub struct Dash {
    pub coolant: Gauge,
    pub cpu_temp: Gauge,
    pub cpu_load: Gauge,
    pub ram: Gauge,
    pub note: String,
    pub cpu_temp_hint: bool,
}

/// Dashboard readings and the "Updates: ..." note, from measured intervals.
#[derive(Default)]
pub struct DashState {
    helper_ticks: i64,
    helper_period_ms: i64,
}

impl DashState {
    pub fn readings(&mut self, e: &Engine) -> Dash {
        let cfg = e.config();
        let water = e.coolant();
        let w_ok = readings::water_valid(water);
        let (ct, ram, load) = (sensors::cpu_temp_c(), sensors::ram_percent(), sensors::cpu_percent());
        Dash {
            coolant: Gauge { text: if w_ok { format!("{}°", water.round_ties_even() + 0.0) } else { "--".into() }, frac: if w_ok { (water - 20.0) / 30.0 } else { 0.0 } },
            cpu_temp: Gauge { text: if ct > 0 { format!("{ct}°") } else { "n/a".into() }, frac: if ct > 0 { (ct as f64 - 20.0) / 75.0 } else { 0.0 } },
            cpu_load: Gauge { text: format!("{load}%"), frac: load as f64 / 100.0 },
            ram: Gauge { text: format!("{ram}%"), frac: ram as f64 / 100.0 },
            note: self.note(e, &cfg, ct),
            cpu_temp_hint: ct <= 0 && platform::cpu_temp_needs_setup(),
        }
    }

    // "Coolant every 2 s · CPU temperature every 1.5 s · CPU load and RAM every 1 s"
    fn note(&mut self, e: &Engine, cfg: &Config, cpu_temp: i32) -> String {
        let mut parts = Vec::new();
        if cfg.DashWater {
            let at = e.coolant_at_ms();
            let age = if at < 0 { -1 } else { util::now_ms() - at };
            let period = e.coolant_period_ms() as i64;
            parts.push(if age < 0 {
                "Coolant: waiting for the cooler".to_string()
            } else if period > 0 && age > 3 * period + 2000 {
                format!("Coolant: no reading for {}", util::secs(age))
            } else if !readings::water_valid(e.coolant()) {
                "Coolant sensor reads open".into()
            } else {
                format!("Coolant every {}", if period > 0 { util::secs(period) } else { "…".into() })
            });
        }
        if cfg.DashCpuTemp {
            if cfg!(windows) {
                let t = sensors::helper_updated_ticks();
                if t != 0 && t != self.helper_ticks {
                    if self.helper_ticks != 0 {
                        self.helper_period_ms = (t - self.helper_ticks) / 10_000;
                    }
                    self.helper_ticks = t;
                }
                parts.push(if cpu_temp <= 0 {
                    "CPU temperature: not available".into()
                } else {
                    format!("CPU temperature every {}", if self.helper_period_ms > 0 { util::secs(self.helper_period_ms) } else { "…".into() })
                });
            } else {
                parts.push(if cpu_temp <= 0 { "CPU temperature: not available".into() } else { "CPU temperature every 1.5 s".into() });
            }
        }
        let mut fast = Vec::new();
        if cfg.DashCpuLoad {
            fast.push("CPU load");
        }
        if cfg.DashRam {
            fast.push("RAM");
        }
        if !fast.is_empty() {
            parts.push(format!("{} every 1 s", fast.join(" and ")));
        }
        if parts.is_empty() { String::new() } else { format!("Updates: {}.", parts.join(" · ")) }
    }
}

pub const PALETTE: [i32; 10] = [0x000000, 0xFFFFFF, 0xFF2B45, 0xFF7A00, 0xFFD400, 0x2BE36B, 0x00D1FF, 0x2B6BFF, 0x8A3BFF, 0xFF3BD2];

// ---- reading lists ----
#[derive(Clone, Copy, Debug)]
pub struct ItemDef {
    pub id: i32,
    pub name: &'static str,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq, Serialize)]
pub enum ListKind {
    Clock,
    Overlay,
}

pub const EMPTY_LINE_ID: i32 = -1; // the "Empty line" chip in the palette

impl ListKind {
    pub fn overlay(self) -> bool {
        self == ListKind::Overlay
    }
    pub fn panel(self) -> &'static str {
        if self.overlay() { "OvItems" } else { "ClkItems" }
    }
    pub fn defs(self) -> &'static [ItemDef] {
        const CLOCK: [ItemDef; 5] = [
            ItemDef { id: 0, name: "Time" },
            ItemDef { id: 1, name: "Date" },
            ItemDef { id: 2, name: "Coolant temperature" },
            ItemDef { id: 3, name: "CPU temperature" },
            ItemDef { id: 4, name: "CPU load" },
        ];
        const OVERLAY: [ItemDef; 3] = [ItemDef { id: 0, name: "Time" }, ItemDef { id: 2, name: "Coolant temperature" }, ItemDef { id: 3, name: "CPU temperature" }];
        if self.overlay() { &OVERLAY } else { &CLOCK }
    }
    pub fn def(self, id: i32) -> Option<ItemDef> {
        self.defs().iter().copied().find(|d| d.id == id)
    }
    pub fn get(self, c: &Config, id: i32) -> bool {
        match (self, id) {
            (ListKind::Clock, 0) => c.ShowTime,
            (ListKind::Clock, 1) => c.ShowDate,
            (ListKind::Clock, 2) => c.ShowWaterTemp,
            (ListKind::Clock, 3) => c.ShowCpuTemp,
            (ListKind::Clock, 4) => c.ShowCpuLoad,
            (ListKind::Overlay, 0) => c.OverlayTime,
            (ListKind::Overlay, 2) => c.OverlayWaterTemp,
            (ListKind::Overlay, 3) => c.OverlayCpuTemp,
            _ => false,
        }
    }
    pub fn set(self, c: &mut Config, id: i32, v: bool) {
        match (self, id) {
            (ListKind::Clock, 0) => c.ShowTime = v,
            (ListKind::Clock, 1) => c.ShowDate = v,
            (ListKind::Clock, 2) => c.ShowWaterTemp = v,
            (ListKind::Clock, 3) => c.ShowCpuTemp = v,
            (ListKind::Clock, 4) => c.ShowCpuLoad = v,
            (ListKind::Overlay, 0) => c.OverlayTime = v,
            (ListKind::Overlay, 2) => c.OverlayWaterTemp = v,
            (ListKind::Overlay, 3) => c.OverlayCpuTemp = v,
            _ => {}
        }
    }
    pub fn lines(self, c: &Config) -> Vec<Vec<i32>> {
        if self.overlay() {
            readings::lines(&c.OverlayOrder, c.OverlayLayout, c.OverlayBreaks, &OVERLAY_IDS)
        } else {
            readings::lines(&c.ClockOrder, c.ClockLayout, c.ClockBreaks, &CLOCK_IDS)
        }
    }
    pub fn set_order(self, c: &mut Config, order: String) {
        if self.overlay() { c.OverlayOrder = order } else { c.ClockOrder = order }
    }
    /// Whether a reading selected in this list is on screen in the mode (its style editor is useful).
    pub fn shown_in(self, m: Mode) -> bool {
        if self.overlay() { m != Mode::Clock && m != Mode::Off } else { m == Mode::Clock }
    }
    pub fn style(self, c: &Config, id: i32) -> ElementStyle {
        c.style(self.overlay(), id)
    }
    /// Commit edited lines (empty lines at the ends dropped).
    pub fn commit(self, c: &mut Config, mut lines: Vec<Vec<i32>>) {
        readings::trim_empty_ends(&mut lines);
        self.set_order(c, readings::lines_to_string(&lines));
    }
    pub fn remove_blank_line(self, c: &mut Config, index: usize) {
        let mut l = self.lines(c);
        if index < l.len() && l[index].is_empty() {
            l.remove(index);
        }
        self.set_order(c, readings::lines_to_string(&l));
    }
}

pub fn is_temperature(id: i32) -> bool {
    id == readings::ID_WATER || id == readings::ID_CPU_TEMP
}
pub fn is_reading(id: i32) -> bool {
    is_temperature(id) || id == readings::ID_LOAD
}

/// Drag state of one list: the lines without the dragged reading (base), where it would land (target) and the lines
/// shown meanwhile (display, with the reading at the target).
#[derive(Clone, Debug, Serialize)]
pub struct LineDrag {
    pub id: i32,
    pub base: Vec<Vec<i32>>,
    pub display: Vec<Vec<i32>>,
    pub t_new: bool,
    pub t_line: usize,
    pub t_index: usize,
    pub new_row_idx: i32, // display row that is the dragged item's own new line, -1 = none
}

impl LineDrag {
    /// Starts at the reading's own place.
    pub fn begin(kind: ListKind, cfg: &Config, id: i32) -> LineDrag {
        let mut base = kind.lines(cfg);
        let (mut t_new, mut t_line, mut t_index) = (true, base.len(), 0);
        if id != EMPTY_LINE_ID {
            for i in 0..base.len() {
                if let Some(k) = base[i].iter().position(|x| *x == id) {
                    base[i].remove(k);
                    if base[i].is_empty() {
                        base.remove(i); // it was alone: its line goes
                        t_new = true;
                        t_line = i;
                    } else {
                        t_new = false;
                        t_line = i;
                        t_index = k;
                    }
                    break;
                }
            }
        }
        let mut d = LineDrag { id, base, display: Vec::new(), t_new, t_line, t_index, new_row_idx: -1 };
        d.apply();
        d
    }

    /// display = base lines with the dragged reading inserted at the target (or a blank line for "Empty line").
    pub fn apply(&mut self) {
        let mut d = self.base.clone();
        if self.t_new {
            let at = self.t_line.min(d.len());
            d.insert(at, if self.id == EMPTY_LINE_ID { vec![] } else { vec![self.id] });
            self.new_row_idx = at as i32;
        } else {
            let l = &mut d[self.t_line];
            l.insert(self.t_index.min(l.len()), self.id);
            self.new_row_idx = -1;
        }
        self.display = d;
    }

    /// Display row -> base line (rows after the preview's own new line shift by one).
    pub fn to_base(&self, row: usize) -> usize {
        if self.new_row_idx >= 0 && row as i32 > self.new_row_idx { row - 1 } else { row }
    }

    /// Sets the target; false when it didn't change.
    pub fn target(&mut self, t_new: bool, t_line: usize, t_index: usize) -> bool {
        if t_new == self.t_new && t_line == self.t_line && (t_new || t_index == self.t_index) {
            return false;
        }
        self.t_new = t_new;
        self.t_line = t_line;
        self.t_index = t_index;
        self.apply();
        true
    }
}

/// Gauge grid: as many columns as fit (~200 px each), balanced so every row is full (4 -> 2x2 or 4x1).
pub fn gauge_columns(count: usize, width: f64) -> usize {
    if count == 0 {
        return 1;
    }
    let fit = ((width / 200.0) as usize).clamp(1, count);
    let rows = count.div_ceil(fit);
    count.div_ceil(rows)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn drag_into_line() {
        let mut c = Config::default();
        c.OverlayOrder = "0|2|3|".into();
        let mut d = LineDrag::begin(ListKind::Overlay, &c, 3);
        assert_eq!(d.base, vec![vec![0], vec![2]]);
        assert!(d.target(false, 0, 1));
        assert_eq!(d.display, vec![vec![0, 3], vec![2]]);
        ListKind::Overlay.commit(&mut c, d.display.clone());
        assert_eq!(c.OverlayOrder, "0,3|2|");
    }

    #[test]
    fn rotation_math() {
        let mut c = Config::default();
        c.Rotation = 270;
        c.PreviewMount = 270;
        assert_eq!(user_rotation(&c), 0);
        assert_eq!(rotation_for(&c, 90), 0);
        c.HwUpright = 90;
        c.Rotation = 0; // user 90
        assert_eq!(engine::hw_for(&c), 0);
    }

    #[test]
    fn known_mount() {
        let key = device_key((0x1A2C, 0x151D), "A247392SS000000\0");
        assert_eq!(key, "1A2C:151D/A247392SS000000");
        let mut c = Config::default();
        c.Rotation = 0;
        c.PreviewMount = 0;
        c.HwUpright = 90;
        assert!(apply_known_mount(&mut c, &key));
        assert_eq!((c.Rotation, c.PreviewMount, c.HwUpright), (270, 270, 90));
        assert_eq!(user_rotation(&c), 0);
        assert_eq!(engine::hw_for(&c), 90);
        // once: a later turn by the user is kept
        c.Rotation = 0;
        assert!(!apply_known_mount(&mut c, &key));
        assert_eq!(c.Rotation, 0);
        assert!(!apply_known_mount(&mut Config::default(), "264A:233C/OTHER"));
    }
}
