//! Studio behaviour without the window toolkit (port of StudioCore + MainUi in App.cs): settings changes with a
//! debounced save, the reading lists and their drag-and-drop lines, the text style editor, the idle picture, the
//! video editor and the status texts. The page talks to it in JSON; what goes back is queued as script in `out`.
use crate::UserEvent;
use serde_json::{json, Value};
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering::SeqCst};
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use tao::event_loop::EventLoopProxy;
use tao::window::Window;
use ttlcd_core::config::{self, Config};
use ttlcd_core::idle::{self, IdleEditor};
use ttlcd_core::model::{self, DashState, LineDrag, ListKind};
use ttlcd_core::render;
use ttlcd_core::{capture, platform, video, Engine, Mode};

/// What the custom protocol serves besides the page itself (shared with its handler thread).
pub struct Blobs {
    pub engine: Engine,
    /// frames rendered without the engine's threads (TTLCD_NO_DEVICE=1): (seq, jpeg)
    pub fallback: Mutex<Option<(u64, Arc<Vec<u8>>)>>,
    pub idle: Mutex<Option<Arc<Vec<u8>>>>,
    pub thumb: Mutex<Option<Arc<Vec<u8>>>>,
    pub quit: AtomicBool,
    pub no_device: bool,
}

impl Blobs {
    /// The newest frame for the preview: (seq, jpeg).
    pub fn frame(&self) -> Option<(u64, Arc<Vec<u8>>)> {
        if self.no_device {
            return self.fallback.lock().unwrap().clone();
        }
        self.engine.latest().map(|f| (f.seq, f.jpeg.clone()))
    }
}

const IMAGE_EXT: [&str; 6] = ["jpg", "jpeg", "png", "bmp", "webp", "tif"];

fn engine_label(m: Mode) -> &'static str {
    ttlcd_core::engine::label(m)
}

/// A command-line job (install/uninstall) running for a status label.
#[derive(Default)]
struct Busy {
    text: String,
    on: bool,
}

pub struct Studio {
    pub engine: Engine,
    pub blobs: Arc<Blobs>,
    proxy: EventLoopProxy<UserEvent>,
    /// script for the page, flushed by the event loop
    pub out: Vec<String>,
    pub no_save: bool,
    /// `--check`: a UI check script runs in the page; it reports with "check" / "checkDone" and may simulate file
    /// drags ("testDrag"), which only the OS can start otherwise
    pub testing: bool,
    pub check_failed: u32,
    pub check_done: bool,
    save_at: Option<Instant>,
    /// state / status changed by settings messages: sent once per event-loop tick (40 ms), not once per message,
    /// so dragging a slider doesn't rebuild the page 60 times a second
    dirty_state: bool,
    dirty_status: bool,
    /// mode before a file dragged over the window picked one (put back if the drag leaves without a drop)
    drag_prev: Option<Mode>,
    /// page shown before a file dragged over the window switched to Display modes (put back if the drag leaves)
    drag_prev_page: Option<String>,
    sel: Option<(ListKind, i32)>,
    drags: [Option<LineDrag>; 2],
    pub page: String,
    pub visible: bool,
    pub ready: bool,
    shown_seq: u64,
    dash: DashState,
    gauge_width: f64,
    hw_rot_shown: i64,
    pub aura_ready: bool,
    pub note: Option<(String, Instant)>,
    // idle picture
    idle: Arc<Mutex<IdleEditor>>,
    idle_view: bool,
    idle_status: String,
    idle_progress: i32,
    idle_seq: Option<u64>,
    idle_counter: u64,
    idle_encode_at: Option<Instant>,
    // video editor (Windows)
    video_duration: f64,
    thumb_gen: Arc<AtomicU64>,
    thumb_seq: u64,
    // command-line jobs
    vd: Busy,
    aura: Busy,
    svc: Busy,
    #[cfg(windows)]
    aura_reader: ttlcd_core::aura::Reader,
    pub has_tray: bool,
    /// the window is full screen (shown on the title-bar button)
    pub full: bool,
}

fn kind_of(s: &str) -> Option<ListKind> {
    match s {
        "Clock" => Some(ListKind::Clock),
        "Overlay" => Some(ListKind::Overlay),
        _ => None,
    }
}

fn kidx(k: ListKind) -> usize {
    if k.overlay() { 1 } else { 0 }
}

fn js_call(f: &str, v: &Value) -> String {
    format!("window.ttlcd&&ttlcd.{f}({v})")
}

impl Studio {
    pub fn new(engine: Engine, blobs: Arc<Blobs>, proxy: EventLoopProxy<UserEvent>) -> Studio {
        let mut s = Studio {
            engine,
            blobs,
            proxy,
            out: Vec::new(),
            no_save: false,
            testing: false,
            check_failed: 0,
            check_done: false,
            save_at: None,
            dirty_state: false,
            dirty_status: false,
            drag_prev: None,
            drag_prev_page: None,
            sel: None,
            drags: [None, None],
            page: "dashboard".into(),
            visible: false,
            ready: false,
            shown_seq: 0,
            dash: DashState::default(),
            gauge_width: 560.0,
            hw_rot_shown: -2,
            aura_ready: ttlcd_core::aura::linux_service_installed(),
            note: None,
            idle: Arc::new(Mutex::new(IdleEditor::new())),
            idle_view: false,
            idle_status: String::new(),
            idle_progress: -1,
            idle_seq: None,
            idle_counter: 0,
            idle_encode_at: None,
            video_duration: 0.0,
            thumb_gen: Arc::new(AtomicU64::new(0)),
            thumb_seq: 0,
            vd: Busy::default(),
            aura: Busy::default(),
            svc: Busy::default(),
            #[cfg(windows)]
            aura_reader: ttlcd_core::aura::Reader::default(),
            has_tray: false,
            full: false,
        };
        // first run: the current live rotation is taken as "upright on this mount" for the preview
        // "Keep going" is no longer offered: switching detection off does the same
        if s.engine.config().GameAction == ttlcd_core::config::GameAction::None {
            s.change(|c| c.GameAction = ttlcd_core::config::GameAction::LowerFps);
        }
        if s.engine.config().PreviewMount < 0 {
            let r0 = s.engine.config().Rotation;
            s.change(|c| c.PreviewMount = r0);
        }
        s
    }

    fn cfg(&self) -> Arc<Config> {
        self.engine.config()
    }

    /// Settings are immutable snapshots: edit a copy, hand it to the engine, save shortly after the last change.
    pub fn change(&mut self, edit: impl FnOnce(&mut Config)) {
        let mut c = (*self.engine.config()).clone();
        edit(&mut c);
        self.engine.set_config(c);
        if !self.no_save {
            self.save_at = Some(Instant::now() + Duration::from_millis(700));
        }
    }

    pub fn save_now(&mut self) {
        self.save_at = None;
        if !self.no_save {
            let _ = self.engine.config().save();
        }
    }

    fn js(&mut self, f: &str, v: Value) {
        self.out.push(js_call(f, &v));
    }

    fn js2(&mut self, f: &str, a: Value, b: Value) {
        self.out.push(format!("window.ttlcd&&ttlcd.{f}({a},{b})"));
    }

    // ================= messages from the page =================

    pub fn handle(&mut self, msg: &Value, window: &Window) {
        let t = msg["t"].as_str().unwrap_or("");
        let s = |k: &str| msg[k].as_str().unwrap_or("").to_string();
        let f = |k: &str| msg[k].as_f64().unwrap_or(0.0);
        match t {
            "ready" => {
                self.ready = true;
                let fonts = ttlcd_core::render::text::fonts().families();
                self.js("fonts", json!(fonts));
                self.push_all();
                if let Some(v) = self.video_path_ok() {
                    self.load_video_info(v);
                }
            }
            "nav" => {
                self.page = s("page");
                self.push_status();
                if self.page == "dashboard" {
                    self.push_dash();
                }
                if self.page == "device" || self.page == "settings" {
                    self.push_state();
                }
            }
            "gw" => {
                self.gauge_width = f("w");
                self.push_dash();
            }
            "set" => {
                let k = s("k");
                let v = msg["v"].clone();
                let mut ok = false;
                self.change(|c| ok = c.set_json(&k, &v));
                if !ok {
                    return;
                }
                match k.as_str() {
                    // sleep applies to the cooler at once and is saved immediately
                    "DeviceSleep" => self.save_now(),
                    "Mode" if self.cfg().Mode == Mode::Video => self.request_thumb(None),
                    _ => {}
                }
                self.dirty_state = true;
                self.dirty_status = true;
                if k.starts_with("Dash") {
                    self.push_dash();
                }
            }
            "style" => {
                let Some((kind, id)) = self.sel else { return };
                let k = s("k");
                let v = msg["v"].clone();
                self.change(|c| {
                    let mut st = kind.style(c, id);
                    let ok = match k.as_str() {
                        "LabelMode" => {
                            st.Label = v != "none";
                            st.LabelAbove = v == "above";
                            true
                        }
                        _ => st.set_json(&k, &v),
                    };
                    if ok {
                        c.set_style(kind.overlay(), st);
                    }
                });
                self.dirty_state = true;
            }
            "rot" => {
                let user = msg["v"].as_i64().unwrap_or(0) as i32;
                self.change(|c| c.Rotation = model::rotation_for(c, user)); // shown relative to upright
                self.push_state();
                self.push_idle();
            }
            "rotate" => {
                // the one Rotation setting, +90° clockwise per click; the engine turns the cooler's own rotation too
                let user = model::next_user_rotation(&self.cfg());
                self.change(|c| c.Rotation = model::rotation_for(c, user));
                self.push_state();
                self.push_idle();
            }
            "toggle" => {
                let (Some(kind), id) = (kind_of(&s("list")), msg["id"].as_i64().unwrap_or(-9) as i32) else { return };
                self.change(|c| {
                    let v = !kind.get(c, id);
                    kind.set(c, id, v)
                });
                self.push_state();
            }
            "select" => {
                let (Some(kind), id) = (kind_of(&s("list")), msg["id"].as_i64().unwrap_or(-9) as i32) else { return };
                if kind.def(id).is_some() {
                    // a second click on the selected reading deselects it (and hides its text style)
                    self.sel = if self.sel == Some((kind, id)) { None } else { Some((kind, id)) };
                    self.push_state();
                }
            }
            "removeBlank" => {
                let Some(kind) = kind_of(&s("list")) else { return };
                let i = msg["index"].as_u64().unwrap_or(u64::MAX) as usize;
                self.change(|c| kind.remove_blank_line(c, i));
                self.push_state();
            }
            "dragBegin" => {
                let (Some(kind), id) = (kind_of(&s("list")), msg["id"].as_i64().unwrap_or(-9) as i32) else { return };
                if id != model::EMPTY_LINE_ID && kind.def(id).is_none() {
                    return;
                }
                // a real drag starts: the selection goes away
                if self.sel.take().is_some() {
                    self.push_state();
                }
                let d = LineDrag::begin(kind, &self.cfg(), id);
                self.js2("drag", json!(kind), json!(d));
                self.drags[kidx(kind)] = Some(d);
            }
            "dragTarget" => {
                let Some(kind) = kind_of(&s("list")) else { return };
                let (t_new, t_line, t_index) = (msg["tNew"].as_bool().unwrap_or(true), msg["tLine"].as_u64().unwrap_or(0) as usize, msg["tIndex"].as_u64().unwrap_or(0) as usize);
                let Some(d) = self.drags[kidx(kind)].as_mut() else { return };
                if !t_new && t_line >= d.base.len() {
                    return;
                }
                if d.target(t_new, t_line.min(d.base.len()), t_index) {
                    let v = json!(d);
                    self.js2("drag", json!(kind), v);
                }
            }
            "dragEnd" => {
                let Some(kind) = kind_of(&s("list")) else { return };
                let Some(d) = self.drags[kidx(kind)].take() else { return };
                if msg["drop"].as_bool().unwrap_or(false) {
                    self.change(|c| kind.commit(c, d.display.clone()));
                }
                self.js2("drag", json!(kind), Value::Null);
                self.push_state();
            }
            "region" => {
                let g = |k: &str| msg[k].as_i64().map(|v| v as i32);
                if let (Some(x), Some(y), Some(w), Some(h)) = (g("x"), g("y"), g("w"), g("h")) {
                    if w > 0 && h > 0 {
                        self.change(|c| {
                            c.RegionX = x;
                            c.RegionY = y;
                            c.RegionW = w;
                            c.RegionH = h;
                        });
                    }
                }
                self.push_state();
            }
            "autostart" => {
                platform::set_autostart(msg["v"].as_bool().unwrap_or(false));
                self.push_state();
            }
            "game" => {
                self.engine.set_force_game(msg["v"].as_bool().unwrap_or(false));
                self.push_status();
            }
            "pause" => {
                self.engine.set_paused(!self.engine.paused());
                self.push_status();
            }
            "browse" => self.browse(&s("what"), window),
            "video" => self.apply_video(f("start"), f("length"), f("zoom"), f("panx"), f("pany"), f("fps")),
            "thumb" => self.request_thumb(Some((f("start"), f("zoom"), f("panx"), f("pany")))),
            "cli" => self.run_cli(&s("args"), &s("what")),
            "clearCache" => {
                video::clear_cache();
                self.push_status();
            }
            "openFolder" => platform::open_folder(&config::folder()),
            "idle" => self.idle_msg(msg, window),
            "jserr" => eprintln!("page script error: {}", s("m")),
            "check" if self.testing => {
                let ok = msg["ok"].as_bool().unwrap_or(false);
                if ok {
                    println!("ok   {}", s("name"));
                } else {
                    println!("FAIL {}: {}", s("name"), s("detail"));
                    self.check_failed += 1;
                }
            }
            "checkDone" if self.testing => self.check_done = true,
            "testDrag" if self.testing => {
                let paths: Vec<String> = msg["paths"].as_array().map(|a| a.iter().filter_map(|p| p.as_str().map(String::from)).collect()).unwrap_or_default();
                match s("a").as_str() {
                    "enter" => self.drag_enter(paths),
                    "leave" => self.drag_leave(),
                    "drop" => self.drag_drop(paths),
                    _ => {}
                }
            }
            _ => {}
        }
    }

    // ================= periodic work (from the event loop) =================
    /// Live preview: tells the page about a new frame (~25 times a second while the window is shown).
    /// Sends the state and status that settings messages marked as changed (once per tick).
    pub fn flush_dirty(&mut self) {
        if std::mem::take(&mut self.dirty_state) {
            self.push_state();
        }
        if std::mem::take(&mut self.dirty_status) {
            self.push_status();
        }
    }

    pub fn tick_frame(&mut self) {
        if !self.visible || !self.ready {
            return;
        }
        if let Some((seq, _)) = self.blobs.frame() {
            if seq != self.shown_seq {
                self.shown_seq = seq;
                self.out.push(format!("window.ttlcd&&ttlcd.frame({seq})"));
            }
        }
    }

    pub fn tick_save(&mut self, now: Instant) {
        if self.save_at.is_some_and(|t| now >= t) {
            self.save_now();
        }
        if self.idle_encode_at.is_some_and(|t| now >= t) {
            self.idle_encode_at = None;
            let editing = {
                let mut ed = self.idle.lock().unwrap();
                ed.encode();
                ed.editing()
            };
            if editing {
                self.show_encoded_idle();
            }
        }
    }

    /// Every 500 ms: status texts, and the cooler-rotation calibration on first connect.
    pub fn tick_status(&mut self) {
        if let Some((_, at)) = &self.note {
            if at.elapsed() > Duration::from_secs(30) {
                self.note = None;
            }
        }
        // a cooler with a measured calibration (model::KNOWN_MOUNTS) gets it once
        if let Some(key) = self.engine.device_key() {
            let mut c = (*self.cfg()).clone();
            if model::apply_known_mount(&mut c, &key) {
                self.change(|n| *n = c);
            }
        }
        // first connect: the cooler's current rotation is taken as the one that matches today's live rotation
        let hw = self.engine.hw_rotation();
        if hw >= 0 && self.cfg().HwUpright < 0 {
            self.change(|c| c.HwUpright = hw);
        }
        let key = hw as i64 * 1000 + self.cfg().Rotation as i64;
        if key != self.hw_rot_shown {
            self.hw_rot_shown = key;
            if self.ready {
                self.push_idle();
            }
        }
        if self.visible && self.ready {
            self.push_status();
        }
    }

    pub fn tick_dash(&mut self) {
        if self.visible && self.ready && self.page == "dashboard" {
            self.push_dash();
        }
    }

    /// Every 3 s: Aura needs armory-crated on Linux; picks it up when installed meanwhile.
    pub fn tick_aura(&mut self) {
        let r = ttlcd_core::aura::linux_service_installed();
        if r != self.aura_ready {
            self.aura_ready = r;
            if self.ready {
                self.push_state();
            }
        }
    }

    pub fn set_visible(&mut self, v: bool) {
        self.visible = v;
        self.update_suspend();
        if v && self.ready {
            self.shown_seq = 0;
            self.push_status();
            self.push_dash();
            self.push_idle();
        }
    }

    // ================= state for the page =================
    pub fn push_all(&mut self) {
        self.push_state();
        self.push_status();
        self.push_dash();
        self.push_idle();
    }

    pub fn push_state(&mut self) {
        if !self.ready {
            return;
        }
        let c = self.cfg();
        let sel_visible = self.sel.is_some_and(|(k, _)| k.shown_in(c.Mode));
        let panels: serde_json::Map<String, Value> = model::panels(&c, sel_visible).into_iter().map(|(k, v)| (k.to_string(), json!(v))).collect();
        let list = |k: ListKind| {
            let defs: Vec<Value> = k.defs().iter().map(|d| json!({"id": d.id, "name": d.name})).collect();
            let on: serde_json::Map<String, Value> = k.defs().iter().map(|d| (d.id.to_string(), json!(k.get(&c, d.id)))).collect();
            json!({"lines": k.lines(&c), "defs": defs, "on": on})
        };
        let sel = self.sel.map(|(k, id)| {
            let name = k.def(id).map(|d| d.name).unwrap_or("");
            json!({
                "list": k, "id": id,
                "title": format!("TEXT STYLE · {}{}", name.to_uppercase(), if k.overlay() { " (OVERLAY)" } else { "" }),
                "style": k.style(&c, id),
                "temp": model::is_temperature(id), "reading": model::is_reading(id),
            })
        });
        let modes: Vec<&str> = Mode::ALL.iter().filter(|m| model::supports(**m)).map(|m| m.name()).collect();
        let monitors: Vec<String> = capture::monitors()
            .iter()
            .enumerate()
            .map(|(i, m)| format!("Monitor {} — {}×{}{}", i + 1, m.w, m.h, if m.primary { " (primary)" } else { "" }))
            .collect();
        let vd = {
            let mon = if cfg!(windows) { capture::find_by_size(480, 480) } else { None };
            let text = if self.vd.on { self.vd.text.clone() } else { mon.map_or("○ Not installed".to_string(), |m| format!("● Installed (monitor {})", m + 1)) };
            json!({"text": text, "installed": mon.is_some(), "busy": self.vd.on})
        };
        let (aura_text, aura_inst) = self.aura_status();
        let svc_inst = platform::sensor_installed();
        let v = json!({
            "cfg": &*c,
            "modes": modes,
            "auraReady": self.aura_ready || cfg!(windows),
            "optTitle": model::options_title(c.Mode),
            "optDesc": model::mode_description(c.Mode),
            "panels": panels,
            "lists": {"Clock": list(ListKind::Clock), "Overlay": list(ListKind::Overlay)},
            "sel": sel,
            "userRot": model::user_rotation(&c),
            "mount": model::mount(&c),
            "autostart": platform::autostart(),
            "palette": model::PALETTE,
            "monitors": monitors,
            "vd": vd,
            "aura": {"text": if self.aura.on { self.aura.text.clone() } else { aura_text }, "installed": aura_inst, "busy": self.aura.on},
            "svc": {"text": if self.svc.on { self.svc.text.clone() } else { platform::sensor_status() }, "installed": svc_inst, "busy": self.svc.on},
            "plat": {
                "linux": cfg!(target_os = "linux"), "windows": cfg!(windows), "video": video::supported(),
                "sensorsManaged": platform::sensors_managed(), "gameDetection": platform::game_detection(),
            },
        });
        self.js("state", v);
    }

    fn aura_status(&mut self) -> (String, bool) {
        #[cfg(windows)]
        {
            let installed = ttlcd_core::win::aura_install::installed();
            let st = self.aura_reader.read();
            let t = if !installed {
                "○ Plugin not installed"
            } else if st.is_none() {
                "● Installed · waiting for Armoury Crate"
            } else {
                "● Installed · receiving Aura data"
            };
            (t.into(), installed)
        }
        #[cfg(not(windows))]
        (String::new(), false)
    }

    pub fn push_status(&mut self) {
        if !self.ready {
            return;
        }
        let e = self.engine.clone();
        let c = e.config();
        let mode_text = match &self.note {
            Some((n, _)) => n.clone(),
            None => model::mode_text(&e),
        };
        let settings = self.page == "settings";
        let aura = if c.Mode == Mode::Aura && cfg!(windows) { self.aura_status().0 } else { String::new() };
        let v = json!({
            "title": model::title_status(&e),
            "conn": model::conn_text(&e),
            "connected": e.connected(),
            "fps": model::fps_text(&e),
            "modeText": mode_text,
            "devStatus": model::device_status(&e),
            "serial": model::serial_text(&e),
            "devFps": format!("{:.1} fps", e.fps()),
            "navSerial": model::nav_serial(&e),
            "gameStatus": model::game_status(&e),
            "forceGame": e.force_game(),
            "paused": e.paused(),
            "videoProgress": e.video_progress(),
            "idleTemp": idle::shown_temp(e.coolant()),
            "full": self.full,
            "svc": if settings { platform::sensor_status() } else { String::new() },
            "aura": aura,
            "cacheInfo": if !settings { String::new() } else if video::supported() { model::cache_info() } else { format!("Settings: {}", config::folder().display()) },
        });
        self.js("status", v);
    }

    pub fn push_dash(&mut self) {
        if !self.ready {
            return;
        }
        let c = self.cfg();
        let d = self.dash.readings(&self.engine);
        let n = [c.DashWater, c.DashCpuTemp, c.DashCpuLoad, c.DashRam].iter().filter(|x| **x).count();
        let mut v = serde_json::to_value(&d).unwrap_or(Value::Null);
        v["cols"] = json!(model::gauge_columns(n, self.gauge_width));
        self.js("dash", v);
    }

    // ================= file dialogs =================
    fn pick(filter_name: &str, exts: &[&str], current: &str, window: &Window) -> Option<String> {
        let mut d = rfd::FileDialog::new().add_filter(filter_name, exts).set_parent(window);
        let cur = std::path::Path::new(current);
        if !current.is_empty() && cur.exists() {
            if let Some(dir) = cur.parent() {
                d = d.set_directory(dir);
            }
            if let Some(n) = cur.file_name() {
                d = d.set_file_name(n.to_string_lossy());
            }
        }
        d.pick_file().map(|p| p.to_string_lossy().into_owned())
    }

    fn browse(&mut self, what: &str, window: &Window) {
        let c = self.cfg();
        match what {
            "image" => {
                if let Some(p) = Self::pick("Images", &IMAGE_EXT, &c.ImagePath, window) {
                    self.set_media(Mode::Image, p);
                }
            }
            "gif" => {
                if let Some(p) = Self::pick("GIF animations", &["gif"], &c.GifPath, window) {
                    self.set_media(Mode::Gif, p);
                }
            }
            "video" => {
                if let Some(p) = Self::pick("Videos", &idle::VIDEO_EXT, &c.VideoPath, window) {
                    self.set_media(Mode::Video, p);
                }
            }
            _ => {}
        }
        self.push_state();
    }

    /// The file a picture / GIF / video mode shows.
    fn set_media(&mut self, mode: Mode, p: String) {
        match mode {
            Mode::Image => self.change(|c| c.ImagePath = p),
            Mode::Gif => self.change(|c| c.GifPath = p),
            Mode::Video => {
                let p2 = p.clone();
                self.change(|c| {
                    c.VideoPath = p2;
                    c.VideoStart = 0.0;
                    c.VideoZoom = 1.0;
                    c.VideoPanX = 0.0;
                    c.VideoPanY = 0.0;
                });
                self.load_video_info(p);
            }
            _ => {}
        }
    }

    // ================= files dragged into the window =================
    /// The mode that shows a file, by its extension (only modes this system has).
    fn mode_for_file(path: &str) -> Option<Mode> {
        let ext = path.rsplit('.').next().unwrap_or("").to_ascii_lowercase();
        let m = if ext == "gif" {
            Mode::Gif
        } else if IMAGE_EXT.contains(&ext.as_str()) {
            Mode::Image
        } else if idle::VIDEO_EXT.contains(&ext.as_str()) {
            Mode::Video
        } else {
            return None;
        };
        model::supports(m).then_some(m)
    }

    /// A file is dragged over the window: the matching mode is selected at once and Display modes is shown (both put
    /// back if the drag leaves), the page greys out all but the preview and shows a drop icon on it. In the Idle view
    /// the file becomes the idle picture instead.
    pub fn drag_enter(&mut self, paths: Vec<String>) {
        let file = paths.first().cloned().unwrap_or_default();
        let name = std::path::Path::new(&file).file_name().map(|n| n.to_string_lossy().into_owned()).unwrap_or_default();
        let mode = Self::mode_for_file(&file);
        let idle_ok = mode.is_some() && (mode != Some(Mode::Video) || video::supported());
        let (target, ok) = if self.idle_view { ("idle", idle_ok) } else { ("mode", mode.is_some()) };
        if target == "mode" {
            if let Some(m) = mode {
                let cur = self.cfg().Mode;
                if cur != m {
                    self.drag_prev.get_or_insert(cur);
                    self.change(|c| c.Mode = m);
                    self.dirty_state = true;
                    self.dirty_status = true;
                }
                if self.page != "modes" {
                    self.drag_prev_page.get_or_insert(self.page.clone());
                    self.js("page", json!("modes"));
                }
            }
        }
        let label = match (target, mode) {
            (_, None) => "This file type can't be shown".to_string(),
            ("idle", Some(_)) if !ok => "Videos need the Windows version".to_string(),
            ("idle", _) => "Drop to use as the idle picture".to_string(),
            (_, Some(m)) => format!("Drop to show it ({})", engine_label(m)),
        };
        self.js("dnd", json!({"active": true, "ok": ok, "target": target, "label": label, "name": name}));
    }

    pub fn drag_leave(&mut self) {
        if let Some(prev) = self.drag_prev.take() {
            self.change(|c| c.Mode = prev);
            self.dirty_state = true;
            self.dirty_status = true;
        }
        if let Some(page) = self.drag_prev_page.take() {
            self.js("page", json!(page));
        }
        self.js("dnd", json!({"active": false}));
    }

    pub fn drag_drop(&mut self, paths: Vec<String>) {
        self.js("dnd", json!({"active": false}));
        let Some(file) = paths.first().cloned() else { return self.drag_leave() };
        let mode = Self::mode_for_file(&file);
        if self.idle_view {
            self.drag_prev = None;
            if mode.is_some() && (mode != Some(Mode::Video) || video::supported()) {
                self.load_idle(&file);
            }
            return;
        }
        match mode {
            Some(m) => {
                self.drag_prev = None;
                self.drag_prev_page = None;
                self.change(|c| c.Mode = m);
                self.set_media(m, file);
                self.dirty_state = true;
                self.dirty_status = true;
            }
            None => self.drag_leave(),
        }
    }

    // ================= video editor (Windows) =================
    fn video_path_ok(&self) -> Option<String> {
        let p = self.cfg().VideoPath.clone();
        (video::supported() && !p.is_empty() && std::path::Path::new(&p).exists()).then_some(p)
    }

    fn load_video_info(&mut self, path: String) {
        self.video_duration = 0.0;
        self.js("video", json!({"info": "Reading video..."}));
        let proxy = self.proxy.clone();
        std::thread::spawn(move || {
            let r = video::duration(&path).map_err(|e| e.to_string());
            let _ = proxy.send_event(UserEvent::VideoInfo(r));
        });
    }

    pub fn video_info(&mut self, r: Result<f64, String>) {
        match r {
            Err(e) => self.js("video", json!({"info": format!("Cannot open: {e}")})),
            Ok(d) => {
                self.video_duration = d;
                let c = self.cfg();
                let max_len = d.min(video::MAX_SECONDS).max(1.0);
                if c.VideoLength > max_len || c.VideoLength <= 0.0 {
                    self.change(|c| c.VideoLength = (max_len * 10.0).round() / 10.0);
                }
                self.js("video", json!({"duration": d}));
                self.push_state();
                self.request_thumb(None);
            }
        }
    }

    fn apply_video(&mut self, start: f64, mut len: f64, zoom: f64, px: f64, py: f64, fps: f64) {
        let d = self.video_duration;
        if d > 0.0 && start + len > d {
            len = (d - start).max(0.5);
        }
        let r = |v: f64, k: f64| (v * k).round() / k;
        self.change(|c| {
            c.VideoStart = r(start, 10.0);
            c.VideoLength = r(len, 10.0);
            c.VideoZoom = r(zoom, 100.0);
            c.VideoPanX = r(px, 100.0);
            c.VideoPanY = r(py, 100.0);
            c.VideoFps = fps.round() as i32;
        });
    }

    fn request_thumb(&mut self, at: Option<(f64, f64, f64, f64)>) {
        let Some(path) = self.video_path_ok() else {
            self.js("video", json!({"thumb": Value::Null}));
            return;
        };
        let c = self.cfg();
        let (t, zoom, pan_x, pan_y) = at.unwrap_or((c.VideoStart, c.VideoZoom, c.VideoPanX, c.VideoPanY));
        let gen = self.thumb_gen.fetch_add(1, SeqCst) + 1;
        let (proxy, gens) = (self.proxy.clone(), self.thumb_gen.clone());
        std::thread::spawn(move || {
            let o = video::Options { path, start: 0.0, length: 1.0, fps: 1, zoom, pan_x, pan_y, quality: 80 };
            let jpg = match video::thumbnail(&o, t) {
                Ok((Some(p), _)) => Some(render::jpeg(&p, 80, false)),
                _ => None,
            };
            if gens.load(SeqCst) == gen {
                if let Some(j) = jpg {
                    let _ = proxy.send_event(UserEvent::Thumb(j));
                }
            }
        });
    }

    pub fn thumb(&mut self, jpg: Vec<u8>) {
        *self.blobs.thumb.lock().unwrap() = Some(Arc::new(jpg));
        self.thumb_seq += 1;
        self.js("video", json!({"thumb": self.thumb_seq}));
    }

    // ================= command-line jobs =================
    fn run_cli(&mut self, args: &str, what: &str) {
        let install = args.ends_with("install") && !args.ends_with("uninstall");
        let text = match (what, install) {
            ("svc", false) => "Removing… (confirm the UAC prompt)",
            (_, true) => "Installing… (confirm the UAC prompt)",
            _ => "Removing…",
        };
        let busy = match what {
            "vd" => &mut self.vd,
            "aura" => &mut self.aura,
            "svc" => &mut self.svc,
            _ => return,
        };
        if busy.on {
            return;
        }
        busy.on = true;
        busy.text = text.into();
        let (proxy, what_s) = (self.proxy.clone(), what.to_string());
        let parts: Vec<&str> = args.split_whitespace().collect();
        platform::run_cli(&parts, move |out| {
            if what_s == "vd" {
                std::thread::sleep(Duration::from_millis(1500)); // display topology settles
            }
            let _ = proxy.send_event(UserEvent::CliDone(what_s, out));
        });
        self.push_state();
    }

    pub fn cli_done(&mut self, what: &str, output: &str) {
        match what {
            "vd" => self.vd.on = false,
            "aura" => self.aura.on = false,
            "svc" => self.svc.on = false,
            _ => {}
        }
        self.push_state();
        self.push_status();
        if output.contains("error") {
            self.js("alert", json!(output.trim()));
        }
    }

    // ================= idle picture (stored in the cooler's flash) =================
    // The Idle view shows the stored picture (cached in idle-current.jpg after an upload or a read from the cooler).
    // Choosing a new picture lets it be positioned (drag) and zoomed (wheel/slider); what is shown is the exact JPEG
    // that would be uploaded: 480x480, upright, quality lowered until it fits the firmware's limit.
    fn idle_msg(&mut self, msg: &Value, window: &Window) {
        match msg["a"].as_str().unwrap_or("") {
            "view" => self.set_idle_view(msg["v"].as_bool().unwrap_or(false)),
            "choose" => {
                let cur = self.cfg().IdlePath.clone();
                let exts = ["jpg", "jpeg", "png", "bmp", "gif", "mp4", "mov", "avi", "mkv", "webm", "wmv", "m4v"];
                if let Some(p) = Self::pick("Images, GIF or video", &exts, &cur, window) {
                    self.load_idle(&p);
                }
            }
            "read" => self.read_idle(),
            "store" => self.store_idle(),
            "cancel" => {
                self.idle.lock().unwrap().drop_pending();
                self.show_stored_idle();
            }
            "zoom" => {
                let z = msg["v"].as_f64().unwrap_or(1.0).clamp(1.0, 5.0);
                let mut ed = self.idle.lock().unwrap();
                if ed.editing() {
                    ed.set_zoom(z);
                    self.idle_encode_at.get_or_insert(Instant::now() + Duration::from_millis(50));
                }
            }
            "pan" => {
                let (dx, dy) = (msg["dx"].as_f64().unwrap_or(0.0), msg["dy"].as_f64().unwrap_or(0.0));
                let mut ed = self.idle.lock().unwrap();
                if ed.editing() {
                    ed.pan(dx, dy);
                    self.idle_encode_at.get_or_insert(Instant::now() + Duration::from_millis(50));
                }
            }
            _ => {}
        }
    }

    pub fn set_idle_view(&mut self, on: bool) {
        self.idle_view = on;
        self.update_suspend();
        if on {
            let (editing, tried) = {
                let ed = self.idle.lock().unwrap();
                (ed.editing(), ed.tried_read)
            };
            if editing {
                self.idle.lock().unwrap().encode();
                self.show_encoded_idle();
            } else {
                self.show_stored_idle();
                if !idle::cache().exists() && !tried {
                    self.read_idle(); // nothing known yet: ask the cooler once
                }
            }
        }
        self.push_idle();
        self.push_status();
    }

    /// While the Idle view is open (and the window visible) no frames go to the cooler, so its own idle screen
    /// shows on the LCD. Hiding the window or switching back to Live resumes streaming.
    fn update_suspend(&mut self) {
        let on = self.idle_view && self.visible;
        if self.engine.suspended() != on {
            if on {
                // the LCD switches at once: a last frame that looks like the cooler's idle screen, then nothing,
                // until the firmware's own idle screen (the same picture) takes over after its no-frames timeout
                self.engine.suspend_with(idle::lcd_frame(self.engine.coolant()));
            } else {
                self.engine.set_suspended(false);
            }
        }
        if on && !self.idle.lock().unwrap().busy {
            self.idle_status = "Streaming paused: the cooler shows its idle screen.".into();
        }
    }

    pub fn idle_tried_read(&mut self) {
        self.idle.lock().unwrap().tried_read = true;
    }

    fn set_idle_blob(&mut self, jpg: Option<Vec<u8>>) {
        match jpg.filter(|j| !j.is_empty()) {
            None => {
                *self.blobs.idle.lock().unwrap() = None;
                self.idle_seq = None;
            }
            Some(j) => {
                *self.blobs.idle.lock().unwrap() = Some(Arc::new(j));
                self.idle_counter += 1;
                self.idle_seq = Some(self.idle_counter);
            }
        }
    }

    fn show_stored_idle(&mut self) {
        self.idle.lock().unwrap().jpeg = None;
        self.set_idle_blob(idle::stored());
        self.push_idle();
    }

    fn show_encoded_idle(&mut self) {
        let j = self.idle.lock().unwrap().jpeg.clone();
        self.set_idle_blob(j);
        self.push_idle();
    }

    fn load_idle(&mut self, file: &str) {
        let r = self.idle.lock().unwrap().load(file);
        if let Err(e) = r {
            self.idle_status = format!("Could not open the file: {e}");
            self.push_idle();
            return;
        }
        self.idle_status = "Drag the picture to position it, scroll to zoom.".into();
        self.show_encoded_idle();
    }

    fn read_idle(&mut self) {
        if self.idle.lock().unwrap().busy {
            return;
        }
        self.idle_status = if self.engine.connected() { "Reading the idle image from the cooler…" } else { "Waiting for the cooler…" }.into();
        self.idle_progress = 0;
        let (p1, p2) = (self.proxy.clone(), self.proxy.clone());
        idle::read(
            &self.idle,
            &self.engine,
            move |p| {
                let _ = p1.send_event(UserEvent::IdleProgress(p, "Reading from the cooler".into()));
            },
            move |status| {
                let _ = p2.send_event(UserEvent::IdleDone(status, false, false));
            },
        );
        self.push_idle();
    }

    fn store_idle(&mut self) {
        {
            let ed = self.idle.lock().unwrap();
            if ed.jpeg.is_none() || ed.busy {
                return;
            }
        }
        self.idle_status = if self.engine.connected() { "Writing to the cooler… do not unplug." } else { "Waiting for the cooler…" }.into();
        self.idle_progress = 0;
        let (p1, p2) = (self.proxy.clone(), self.proxy.clone());
        idle::store(
            &self.idle,
            &self.engine,
            move |p| {
                let _ = p1.send_event(UserEvent::IdleProgress(p, idle::store_progress_text(p).into()));
            },
            move |status, ok| {
                let _ = p2.send_event(UserEvent::IdleDone(status, ok, true));
            },
        );
        self.push_idle();
    }

    pub fn idle_progress(&mut self, p: i32, what: &str) {
        let p = p.clamp(0, 100);
        self.idle_progress = p;
        self.idle_status = format!("{what}… {p}%");
        self.push_idle();
    }

    pub fn idle_done(&mut self, status: String, ok: bool, store: bool) {
        self.idle_progress = -1;
        self.idle_status = status;
        let editing = self.idle.lock().unwrap().editing();
        if store && ok {
            self.idle.lock().unwrap().drop_pending();
            self.show_stored_idle();
        } else if !store && !editing {
            self.show_stored_idle();
        }
        self.push_idle();
    }

    pub fn push_idle(&mut self) {
        if !self.ready {
            return;
        }
        let c = self.cfg();
        let ed = self.idle.lock().unwrap();
        let editing = ed.editing();
        let title = if editing {
            "New idle image"
        } else if idle::cache().exists() {
            "Current idle image"
        } else {
            "Idle image"
        };
        let v = json!({
            "view": self.idle_view,
            "editing": editing,
            "img": self.idle_seq,
            // stored picture: turned as the cooler shows it; a picture being placed stays upright for dragging
            "angle": if editing { 0 } else { model::idle_preview_angle(&c) },
            "title": title,
            "info": ed.info,
            "status": self.idle_status,
            "busy": ed.busy,
            "progress": self.idle_progress,
            "fits": ed.fits(),
            "zoom": ed.zoom,
        });
        drop(ed);
        self.js("idle", v);
    }

    // ================= screenshot helpers =================
    /// TTLCD_SHOT_SELECT="ClkItems:2": a reading selected for styling.
    pub fn preselect(&mut self, spec: &str) {
        let Some((panel, id)) = spec.split_once(':') else { return };
        let kind = if panel == ListKind::Overlay.panel() { ListKind::Overlay } else { ListKind::Clock };
        if let Ok(id) = id.trim().parse::<i32>() {
            if kind.def(id).is_some() {
                self.sel = Some((kind, id));
            }
        }
    }

    pub fn set_note(&mut self, n: String) {
        self.note = Some((n, Instant::now()));
    }
}
