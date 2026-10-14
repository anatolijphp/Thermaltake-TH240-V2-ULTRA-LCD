//! The screen engine (port of Engine.cs). Threads:
//!  - renderer: produces frames for the configured mode (always runs, so the Studio preview works without the cooler)
//!  - sender:   owns the USB connection and pushes the newest frame, dropping stale ones, with a 500 ms keep-alive
//!  - control:  per connection: brightness, sleep, serial, rotation, coolant temperature, device jobs (uploads)
//!  - games:    game detection every 3 s (Windows)
use crate::aura;
use crate::capture::{self, RectI, ScreenSource};
use crate::config::{Config, GameAction, Mode};
use crate::lcd::Lcd;
use crate::media;
use crate::render::{self, readings, Picture};
use crate::sensors;
use crate::video::{self, Clip};
use std::collections::VecDeque;
use std::io;
use std::sync::atomic::{AtomicBool, AtomicI32, AtomicI64, AtomicU64, Ordering::SeqCst};
use std::sync::{Arc, Condvar, Mutex, RwLock};
use std::thread::{self, JoinHandle};
use std::time::{Duration, Instant};
use tiny_skia::Pixmap;

pub struct Frame {
    pub jpeg: Arc<Vec<u8>>,       // what is sent to a panel with normal colour order (already rotated)
    pub rgba: Option<Arc<Pixmap>>, // the same frame's pixels (to encode red/blue swapped for the LX800 board)
    pub rotation: i32,            // rotation applied, so a preview can undo it
    pub seq: u64,
    pub quality: i32,
}

/// Work on the control thread while live frames pause (idle picture upload / read-back).
pub struct DeviceJob {
    pub name: String,
    pub work: Box<dyn FnOnce(&Lcd, &mut dyn FnMut(i32)) -> io::Result<()> + Send>,
    pub progress: Box<dyn FnMut(i32) + Send>,
    pub done: Box<dyn FnOnce(Option<io::Error>) + Send>,
}

pub const MANUAL_GAME: &str = "manual";

#[derive(Default)]
struct Event {
    m: Mutex<bool>,
    c: Condvar,
}

impl Event {
    fn set(&self) {
        *self.m.lock().unwrap() = true;
        self.c.notify_all();
    }
    /// Waits until set (auto-reset) or the timeout passes.
    fn wait(&self, d: Duration) {
        let g = self.m.lock().unwrap();
        let (mut g, _) = self.c.wait_timeout_while(g, d, |s| !*s).unwrap();
        *g = false;
    }
}

#[derive(Default)]
struct Texts {
    render_status: String,
    device_status: String,
    serial: String,
    game: Option<String>,
}

#[derive(Default)]
struct VideoSlot {
    key: Option<String>,
    failed: Option<String>,
    clip: Option<Arc<Clip>>,
    loading: bool,
}

struct Shared {
    cfg: RwLock<Arc<Config>>,
    paused: AtomicBool,
    quit: AtomicBool,
    suspended: AtomicBool,
    force_game: AtomicBool,
    connected: AtomicBool,
    hold_frames: AtomicBool,
    latest: Mutex<Option<Arc<Frame>>>,
    new_frame: Event,
    game_wake: Event,
    seq: AtomicU64,
    texts: Mutex<Texts>,
    fps: AtomicU64,            // f64 bits: frames actually sent per second
    video_progress: AtomicU64, // f64 bits: 0..1 while a video is being prepared, -1 otherwise
    coolant: AtomicI32,        // pump sensor in hundredths of °C, i32::MIN when unread
    coolant_at: AtomicI64,     // util::now_ms of the last reading, -1 = none
    coolant_period: AtomicI32, // ms between the last two readings
    hw_rotation: AtomicI32,    // the cooler's stored rotation in degrees, -1 until read after connecting
    jobs: Mutex<VecDeque<DeviceJob>>,
    session: AtomicU64, // current connection; the control thread runs while it matches
    video: Mutex<VideoSlot>,
    threads: Mutex<Vec<JoinHandle<()>>>,
}

#[derive(Clone)]
pub struct Engine(Arc<Shared>);

fn set_f64(a: &AtomicU64, v: f64) {
    a.store(v.to_bits(), SeqCst);
}
fn get_f64(a: &AtomicU64) -> f64 {
    f64::from_bits(a.load(SeqCst))
}

pub fn label(m: Mode) -> &'static str {
    match m {
        Mode::VirtualDisplay => "Virtual display",
        Mode::Monitor => "Screen mirror",
        Mode::Image => "Image",
        Mode::Gif => "GIF",
        Mode::Video => "Video",
        Mode::Color => "Colour",
        Mode::Clock => "Clock",
        Mode::Aura => "Aura Sync",
        Mode::Off => "Off",
    }
}

/// The cooler's rotation that matches live rotation R. Its +90° turns counter-clockwise while ours turns clockwise
/// (tested 2026-10-10), so: hw = HwUpright - (R - PreviewMount). -1 = not calibrated yet.
pub fn hw_for(c: &Config) -> i32 {
    if c.HwUpright < 0 || c.PreviewMount < 0 {
        return -1;
    }
    ((c.HwUpright - (c.Rotation - c.PreviewMount)) % 360 + 360) % 360
}

impl Engine {
    pub fn new(c: Config) -> Engine {
        Engine(Arc::new(Shared {
            cfg: RwLock::new(Arc::new(c)),
            paused: AtomicBool::new(false),
            quit: AtomicBool::new(false),
            suspended: AtomicBool::new(false),
            force_game: AtomicBool::new(false),
            connected: AtomicBool::new(false),
            hold_frames: AtomicBool::new(false),
            latest: Mutex::new(None),
            new_frame: Event::default(),
            game_wake: Event::default(),
            seq: AtomicU64::new(0),
            texts: Mutex::new(Texts { render_status: "Starting".into(), device_status: "Connecting...".into(), ..Default::default() }),
            fps: AtomicU64::new(0f64.to_bits()),
            video_progress: AtomicU64::new((-1f64).to_bits()),
            coolant: AtomicI32::new(i32::MIN),
            coolant_at: AtomicI64::new(-1),
            coolant_period: AtomicI32::new(0),
            hw_rotation: AtomicI32::new(-1),
            jobs: Mutex::new(VecDeque::new()),
            session: AtomicU64::new(0),
            video: Mutex::new(VideoSlot::default()),
            threads: Mutex::new(Vec::new()),
        }))
    }

    // ---- state ----
    pub fn config(&self) -> Arc<Config> {
        self.0.cfg.read().unwrap().clone()
    }
    pub fn set_config(&self, c: Config) {
        *self.0.cfg.write().unwrap() = Arc::new(c);
        self.0.new_frame.set();
    }
    pub fn paused(&self) -> bool {
        self.0.paused.load(SeqCst)
    }
    pub fn set_paused(&self, v: bool) {
        self.0.paused.store(v, SeqCst);
    }
    /// Stop sending frames altogether (not even black ones), so the firmware falls back to its own idle screen.
    pub fn suspended(&self) -> bool {
        self.0.suspended.load(SeqCst)
    }
    pub fn set_suspended(&self, v: bool) {
        self.0.suspended.store(v, SeqCst);
        if v {
            set_f64(&self.0.fps, 0.0);
        }
        self.0.new_frame.set();
    }
    /// Force game mode on (for games detection misses). Not saved: it ends when the app restarts.
    pub fn force_game(&self) -> bool {
        self.0.force_game.load(SeqCst)
    }
    pub fn set_force_game(&self, v: bool) {
        self.0.force_game.store(v, SeqCst);
        self.0.game_wake.set();
    }
    pub fn connected(&self) -> bool {
        self.0.connected.load(SeqCst)
    }
    pub fn fps(&self) -> f64 {
        get_f64(&self.0.fps)
    }
    pub fn video_progress(&self) -> f64 {
        get_f64(&self.0.video_progress)
    }
    pub fn latest(&self) -> Option<Arc<Frame>> {
        self.0.latest.lock().unwrap().clone()
    }
    pub fn serial(&self) -> String {
        self.0.texts.lock().unwrap().serial.clone()
    }
    pub fn render_status(&self) -> String {
        self.0.texts.lock().unwrap().render_status.clone()
    }
    pub fn device_status(&self) -> String {
        self.0.texts.lock().unwrap().device_status.clone()
    }
    pub fn status(&self) -> String {
        if self.connected() { self.render_status() } else { self.device_status() }
    }
    /// Name of the running game, MANUAL_GAME when forced, None otherwise.
    pub fn game(&self) -> Option<String> {
        self.0.texts.lock().unwrap().game.clone()
    }
    pub fn gaming(&self) -> bool {
        self.game().is_some() && self.config().GameAction != GameAction::None
    }
    fn game_idle(&self, c: &Config) -> bool {
        self.game().is_some() && c.GameAction == GameAction::IdleScreen
    }
    /// Coolant temperature in °C, NaN when unread.
    pub fn coolant(&self) -> f64 {
        match self.0.coolant.load(SeqCst) {
            i32::MIN => f64::NAN,
            v => v as f64 / 100.0,
        }
    }
    pub fn coolant_at_ms(&self) -> i64 {
        self.0.coolant_at.load(SeqCst)
    }
    pub fn coolant_period_ms(&self) -> i32 {
        self.0.coolant_period.load(SeqCst)
    }
    pub fn hw_rotation(&self) -> i32 {
        self.0.hw_rotation.load(SeqCst)
    }
    pub fn jobs_pending(&self) -> bool {
        !self.0.jobs.lock().unwrap().is_empty() || self.0.hold_frames.load(SeqCst)
    }
    /// Runs once the cooler is connected; live frames pause while it runs.
    pub fn enqueue(&self, job: DeviceJob) {
        self.0.jobs.lock().unwrap().push_back(job);
    }

    fn quit(&self) -> bool {
        self.0.quit.load(SeqCst)
    }

    pub fn start(&self) {
        #[cfg(windows)]
        unsafe {
            windows_sys::Win32::Media::timeBeginPeriod(1); // default 15.6 ms sleep granularity caps pacing below 30 fps
        }
        let mut t = self.0.threads.lock().unwrap();
        let e = self.clone();
        t.push(thread::Builder::new().name("lcd-render".into()).spawn(move || e.render_loop()).unwrap());
        let e = self.clone();
        t.push(thread::Builder::new().name("lcd-send".into()).spawn(move || e.send_loop()).unwrap());
        let e = self.clone();
        t.push(thread::Builder::new().name("lcd-games".into()).spawn(move || e.game_loop()).unwrap());
    }

    pub fn stop(&self) {
        self.0.quit.store(true, SeqCst);
        self.0.new_frame.set();
        self.0.game_wake.set();
        let threads: Vec<_> = self.0.threads.lock().unwrap().drain(..).collect();
        for t in threads {
            let _ = t.join();
        }
        #[cfg(windows)]
        unsafe {
            windows_sys::Win32::Media::timeEndPeriod(1);
        }
    }

    // ================= game mode =================
    fn game_loop(&self) {
        let mut sensor_ms = -1;
        while !self.quit() {
            let c = self.config();
            let mut g = None;
            if c.GameDetect {
                g = crate::games::detect(&c);
            }
            if g.is_none() && self.force_game() {
                g = Some(MANUAL_GAME.to_owned());
            }
            let gaming = g.is_some();
            self.0.texts.lock().unwrap().game = g;
            // slow the CPU-temperature helper down while gaming (0 = its default)
            let want = if gaming { c.GameSensorMs.max(1000) } else { 0 };
            if want != sensor_ms && cfg!(windows) {
                sensors::request_interval(want);
                sensor_ms = want;
            }
            self.0.new_frame.set();
            self.0.game_wake.wait(Duration::from_secs(3)); // next check, or at once when forced on/off
        }
    }

    // ================= renderer =================
    fn publish(&self, jpeg: Vec<u8>, rgba: Option<Pixmap>, rotation: i32, quality: i32) {
        let seq = self.0.seq.fetch_add(1, SeqCst) + 1;
        *self.0.latest.lock().unwrap() = Some(Arc::new(Frame { jpeg: Arc::new(jpeg), rgba: rgba.map(Arc::new), rotation, seq, quality }));
        self.0.new_frame.set();
    }

    fn set_render_status(&self, s: String) {
        self.0.texts.lock().unwrap().render_status = s;
    }

    fn values(&self) -> readings::Values {
        readings::Values { water: self.coolant(), cpu_temp: sensors::cpu_temp_c(), cpu_load: sensors::cpu_percent() }
    }

    /// One frame of the current mode as a JPEG, without a cooler (`ttlcd render`; CI checks the build with it).
    pub fn render_once(&self) -> Vec<u8> {
        let c = self.config();
        if c.Mode == Mode::Off {
            return render::black_jpeg();
        }
        let mut st = RenderState::default();
        let p = self.produce(&c, &mut st);
        let mut frame = p.frame;
        if c.Mode != Mode::Clock && (c.OverlayTime || c.OverlayCpuTemp || c.OverlayWaterTemp) {
            readings::draw_overlay(&mut frame, &c, &self.values());
        }
        render::jpeg(&render::rotate(&frame, c.Rotation), c.Quality, false)
    }

    fn render_loop(&self) {
        let mut st = RenderState::default();
        let mut blank = false;
        while !self.quit() {
            let c = self.config();
            if self.paused() {
                // freeze: render nothing new; the sender keeps re-sending the last frame (keep-alive)
                self.set_render_status("Paused".into());
                thread::sleep(Duration::from_millis(100));
                continue;
            }
            if c.Mode == Mode::Off {
                if !blank {
                    let b = render::blank(0);
                    self.publish(render::jpeg(&b, 50, false), Some(b), 0, 50);
                    blank = true;
                }
                self.set_render_status("Off".into());
                thread::sleep(Duration::from_millis(100));
                continue;
            }
            if self.game_idle(&c) {
                // game running: render nothing; the sender stops too, so the cooler shows its idle screen
                self.set_render_status(format!("Game running ({}): cooler shows its idle screen", self.game().unwrap_or_default()));
                thread::sleep(Duration::from_millis(250));
                continue;
            }
            blank = false;
            let slow = self.game().is_some() && c.GameAction == GameAction::LowerFps;
            let t0 = Instant::now();
            let p = self.produce(&c, &mut st);
            let mut delay = p.delay;
            let overlay = c.Mode != Mode::Clock && (c.OverlayTime || c.OverlayCpuTemp || c.OverlayWaterTemp);
            match (&p.direct, overlay, c.Rotation % 360 == 0) {
                (Some(j), false, true) => self.publish(j.to_vec(), None, 0, c.Quality), // pre-encoded video frame
                _ => {
                    let mut frame = p.frame;
                    if overlay {
                        readings::draw_overlay(&mut frame, &c, &self.values());
                    }
                    let frame = render::rotate(&frame, c.Rotation);
                    self.publish(render::jpeg(&frame, c.Quality, false), Some(frame), c.Rotation, c.Quality);
                }
            }
            if slow {
                delay = delay.max(1000 / c.GameFps.max(1)); // game running: lower frame rate
            }
            let mut status = p.note.unwrap_or_else(|| {
                format!("{}{}", label(c.Mode), if delay < 1000 { format!(" · {:.0} fps", self.fps()) } else { String::new() })
            });
            if slow {
                status += &format!(" · game mode ({})", self.game().unwrap_or_default());
            }
            self.set_render_status(status);
            let deadline = t0 + Duration::from_millis(delay.max(1) as u64);
            // a settings change ends the wait at once
            let cur = self.0.cfg.read().unwrap().clone();
            crate::util::sleep_until(deadline, || self.quit() || !Arc::ptr_eq(&cur, &self.0.cfg.read().unwrap()));
        }
    }

    fn produce(&self, c: &Config, st: &mut RenderState) -> Produced {
        let stream_delay = 1000 / c.Fps.clamp(1, 30); // screen capture, Aura, still modes
        let msg = |text: &str, note: &str| Produced { frame: render::message(text), delay: 1000, note: Some(note.into()), direct: None };
        match c.Mode {
            Mode::VirtualDisplay => {
                let Some(mon) = capture::find_by_size(480, 480) else {
                    return msg("Virtual display not installed.\nInstall it on the Device page.", "Virtual display not installed");
                };
                match capture::monitor_rect(mon, None) {
                    Some(r) => st.capture(r, c, stream_delay),
                    None => msg("Virtual display not installed.", "Virtual display not installed"),
                }
            }
            Mode::Monitor => {
                let sub = c.UseRegion.then(|| RectI { x: c.RegionX, y: c.RegionY, w: c.RegionW.max(1), h: c.RegionH.max(1) });
                let n = capture::monitors().len();
                match (n > 0).then(|| capture::monitor_rect((c.MonitorIndex.max(0) as usize).min(n - 1), sub)).flatten() {
                    Some(r) => st.capture(r, c, stream_delay),
                    None => msg("Screen capture needs the Windows version", "No monitor"),
                }
            }
            Mode::Image => match st.image(&c.ImagePath, c.FitContain, c.ColorRgb) {
                Some(f) => Produced { frame: f, delay: stream_delay, note: None, direct: None },
                None => msg("No image selected", "Choose an image"),
            },
            Mode::Gif => {
                let Some(frames) = st.gif(c) else { return msg("No GIF selected", "Choose a GIF") };
                if frames.is_empty() {
                    return msg("No GIF selected", "Choose a GIF");
                }
                if st.gif_idx >= frames.len() {
                    st.gif_idx = 0;
                }
                let (f, d) = frames[st.gif_idx].clone();
                st.gif_idx += 1;
                // never faster than the Fps cap, but a slower GIF keeps its own pace
                Produced { frame: f, delay: d.max(stream_delay), note: None, direct: None }
            }
            Mode::Video => {
                let Some(clip) = self.load_video(c) else {
                    if c.VideoPath.is_empty() {
                        return msg("No video selected", "Choose a video");
                    }
                    let p = self.video_progress();
                    let name = std::path::Path::new(&c.VideoPath).file_name().map(|n| n.to_string_lossy().into_owned()).unwrap_or_default();
                    let (text, note) = if p >= 0.0 {
                        (format!("Preparing video\n{:.0} %", p * 100.0), format!("Preparing video {:.0} %", p * 100.0))
                    } else if !video::supported() {
                        ("Videos need the Windows version".into(), "Videos need the Windows version".into())
                    } else {
                        (format!("Video could not be loaded:\n{name}"), "Video could not be loaded".into())
                    };
                    return Produced { frame: render::message(&text), delay: 200, note: Some(note), direct: None }; // retry-poll
                };
                if st.video_idx >= clip.frames.len() {
                    st.video_idx = 0;
                }
                let j = Arc::new(clip.frames[st.video_idx].clone());
                st.video_idx += 1;
                let frame = render::decode(&j).map(|p| render::compose(Some(&p), false, 0)).unwrap_or_else(|| render::blank(0));
                // never faster than the Fps cap, but a slower clip keeps its own pace
                Produced { frame, delay: clip.frame_ms.max(stream_delay), note: None, direct: Some(j) }
            }
            Mode::Aura => {
                let snap = st.aura.read();
                let (frame, status) = st.vis.render(snap.as_ref());
                Produced { frame, delay: stream_delay, note: Some(format!("{} · {:.0} fps", status, self.fps())), direct: None }
            }
            Mode::Clock => {
                let mut b = render::blank(c.ColorRgb);
                readings::draw_clock(&mut b, c, &self.values());
                Produced { frame: b, delay: stream_delay, note: None, direct: None }
            }
            Mode::Color => Produced { frame: render::blank(c.ColorRgb), delay: stream_delay, note: None, direct: None },
            Mode::Off => Produced { frame: render::blank(0), delay: 100, note: None, direct: None },
        }
    }

    /// The clip when ready; otherwise starts preparing it in the background and returns None.
    fn load_video(&self, c: &Config) -> Option<Arc<Clip>> {
        if c.VideoPath.is_empty() || !std::path::Path::new(&c.VideoPath).exists() || !video::supported() {
            return None;
        }
        let o = video::Options::from_config(c);
        let key = o.key();
        let mut v = self.0.video.lock().unwrap();
        if v.key.as_deref() == Some(&key) {
            return v.clip.clone();
        }
        if v.failed.as_deref() == Some(&key) || v.loading {
            return None; // a previous load still runs; it re-checks when done
        }
        v.key = None;
        v.clip = None;
        v.loading = true;
        drop(v);
        set_f64(&self.0.video_progress, 0.0);
        let e = self.clone();
        thread::Builder::new()
            .name("lcd-video".into())
            .spawn(move || {
                let still = |e: &Engine| video::Options::from_config(&e.config()).key() == key;
                let r = video::load(&o, &mut |p| {
                    set_f64(&e.0.video_progress, p);
                    !e.quit() && still(&e)
                });
                let mut v = e.0.video.lock().unwrap();
                v.loading = false;
                match r {
                    Ok(Some(clip)) if !clip.frames.is_empty() => {
                        v.clip = Some(Arc::new(clip));
                        v.key = Some(key.clone());
                        set_f64(&e.0.video_progress, -1.0);
                    }
                    _ if still(&e) => {
                        v.failed = Some(key.clone());
                        set_f64(&e.0.video_progress, -1.0);
                    }
                    _ => set_f64(&e.0.video_progress, 0.0), // superseded; the next frame starts the new load
                }
            })
            .ok();
        None
    }

    // ================= sender =================
    fn send_loop(&self) {
        let mut lcd: Option<Arc<Lcd>> = None;
        let mut sent_seq = 0u64;
        let mut swapped: Option<(u64, Vec<u8>)> = None;
        let start = Instant::now();
        let ms = || start.elapsed().as_millis() as i64;
        let (mut fps_t, mut last_sent, mut fps_n) = (0i64, 0i64, 0);
        while !self.quit() {
            if lcd.is_none() {
                match Lcd::open() {
                    Ok(l) => {
                        let l = Arc::new(l);
                        self.0.texts.lock().unwrap().device_status = "Connecting to cooler...".into();
                        self.start_control(l.clone());
                        sent_seq = 0;
                        lcd = Some(l);
                    }
                    Err(e) => {
                        self.0.connected.store(false, SeqCst);
                        self.0.texts.lock().unwrap().device_status = format!("Cooler not found: {e}");
                        for _ in 0..20 {
                            if self.quit() {
                                break;
                            }
                            thread::sleep(Duration::from_millis(100));
                        }
                        continue;
                    }
                }
            }
            self.0.new_frame.wait(Duration::from_millis(250));
            let f = self.latest();
            let c = self.config();
            // Repeat the current frame as a keep-alive: the LX800 firmware falls back to its built-in temperature
            // screen when no frame arrives for a few seconds (static images change rarely).
            let keep_alive = f.as_ref().is_some_and(|f| f.seq == sent_seq) && ms() - last_sent >= 500;
            let idle = self.game_idle(&c);
            let Some(f) = f else { continue };
            if (f.seq == sent_seq && !keep_alive) || self.0.hold_frames.load(SeqCst) || self.suspended() || idle {
                if idle {
                    set_f64(&self.0.fps, 0.0);
                }
                continue;
            }
            let l = lcd.as_ref().unwrap();
            let data: &[u8] = if l.swap_red_blue {
                if swapped.as_ref().map(|s| s.0) != Some(f.seq) {
                    let j = match &f.rgba {
                        Some(p) => render::jpeg(p, f.quality, true),
                        None => render::recompress(&f.jpeg, f.quality, true).unwrap_or_else(|| f.jpeg.to_vec()),
                    };
                    swapped = Some((f.seq, j));
                }
                &swapped.as_ref().unwrap().1
            } else {
                &f.jpeg
            };
            match l.show_jpeg(data) {
                // the first transfer after opening stalls ~5 s (device wake-up)
                Ok(()) => {
                    sent_seq = f.seq;
                    last_sent = ms();
                    self.0.connected.store(true, SeqCst);
                    fps_n += 1;
                    let now = ms();
                    if now - fps_t >= 1000 {
                        set_f64(&self.0.fps, fps_n as f64 * 1000.0 / (now - fps_t) as f64);
                        fps_n = 0;
                        fps_t = now;
                    }
                }
                Err(e) => {
                    self.0.session.fetch_add(1, SeqCst); // ends the control thread
                    self.0.connected.store(false, SeqCst);
                    set_f64(&self.0.fps, 0.0);
                    self.0.texts.lock().unwrap().device_status = format!("Connection lost: {e}");
                    lcd = None;
                }
            }
        }
        if let Some(l) = lcd {
            if self.config().BlankOnExit {
                let _ = l.show_jpeg(&render::black_jpeg());
            }
            self.0.session.fetch_add(1, SeqCst);
        }
    }

    /// Control commands (brightness, serial, uploads) on their own thread: the cooler stalls the first control
    /// transfer after opening for ~10 s, which must not block frame streaming on the LCD interface.
    fn start_control(&self, lcd: Arc<Lcd>) {
        let session = self.0.session.fetch_add(1, SeqCst) + 1;
        self.0.hw_rotation.store(-1, SeqCst); // re-read from the cooler on every connect
        let e = self.clone();
        let _ = thread::Builder::new().name("lcd-control".into()).spawn(move || {
            let (mut sent, mut sleep_sent) = (-1, -1);
            let mut got_serial = false;
            // Config.Rotation as of reading the cooler's stored rotation this connection. Only a later CHANGE to
            // Rotation pushes a write: finding that the stored value disagrees with our calibration must not (it was
            // very likely changed on purpose: ttlcd rotate, a previous session, a remount).
            let mut rotation_at_connect = i32::MIN;
            let mut last_temp: Option<Instant> = None;
            while !e.quit() && e.0.session.load(SeqCst) == session {
                let c = e.config();
                let r = (|| -> io::Result<()> {
                    if c.Brightness != sent {
                        lcd.set_brightness(c.Brightness.clamp(0, 100) as u8)?;
                        sent = c.Brightness;
                    }
                    let want_sleep = c.DeviceSleep as i32;
                    if want_sleep != sleep_sent {
                        lcd.set_sleep(c.DeviceSleep)?;
                        sleep_sent = want_sleep;
                    }
                    // queries time out while the cooler is still waking up; keep trying instead of giving up
                    if !got_serial {
                        if let Ok(s) = lcd.serial() {
                            e.0.texts.lock().unwrap().serial = s;
                            got_serial = true;
                        }
                    }
                    if e.hw_rotation() < 0 {
                        if let Ok(r) = lcd.rotation() {
                            e.0.hw_rotation.store(r as i32, SeqCst);
                            rotation_at_connect = c.Rotation;
                        }
                    }
                    let hw = hw_for(&c);
                    // flash write: only when Rotation changed since we connected, not merely on mismatch
                    if hw >= 0 && e.hw_rotation() >= 0 && c.Rotation != rotation_at_connect && hw != e.hw_rotation() {
                        lcd.set_rotation(hw)?;
                        e.0.hw_rotation.store(hw, SeqCst);
                        rotation_at_connect = c.Rotation;
                    }
                    // water temperature every 2 s, or at the game-mode interval while a game runs
                    let every = if e.gaming() { c.GameSensorMs.max(2000) } else { 2000 } as u64;
                    if last_temp.is_none_or(|t| t.elapsed() >= Duration::from_millis(every)) {
                        if let Ok(t) = lcd.coolant_c() {
                            e.0.coolant.store((t * 100.0).round() as i32, SeqCst);
                            let now = crate::util::now_ms();
                            let at = e.0.coolant_at.swap(now, SeqCst);
                            if at >= 0 {
                                e.0.coolant_period.store((now - at) as i32, SeqCst);
                            }
                        }
                        last_temp = Some(Instant::now());
                    }
                    Ok(())
                })();
                if r.is_err() {
                    return; // session closed; the sender reconnects
                }
                let job = e.0.jobs.lock().unwrap().pop_front();
                if let Some(job) = job {
                    e.run_job(&lcd, job);
                }
                thread::sleep(Duration::from_millis(100));
            }
        });
    }

    fn run_job(&self, lcd: &Lcd, job: DeviceJob) {
        self.0.hold_frames.store(true, SeqCst);
        thread::sleep(Duration::from_millis(200)); // let an in-flight frame finish
        let DeviceJob { work, mut progress, done, .. } = job;
        let r = work(lcd, &mut |p| progress(p));
        self.0.hold_frames.store(false, SeqCst);
        self.0.new_frame.set();
        done(r.err());
    }
}

struct Produced {
    frame: Pixmap,
    delay: i32,
    note: Option<String>,
    direct: Option<Arc<Vec<u8>>>, // pre-encoded frame (video)
}

/// What the renderer keeps between frames: decoded pictures, GIF frames, the screen grabber, Aura state.
#[derive(Default)]
struct RenderState {
    img_key: String,
    img: Option<Pixmap>,
    gif_key: String,
    gif: Option<Vec<(Pixmap, i32)>>,
    gif_idx: usize,
    screen: Option<ScreenSource>,
    video_idx: usize,
    aura: aura::Reader,
    vis: aura::Visualiser,
}

impl RenderState {
    fn image(&mut self, path: &str, contain: bool, bg: i32) -> Option<Pixmap> {
        if path.is_empty() || !std::path::Path::new(path).exists() {
            return None;
        }
        let key = format!("{}|{}|{}|{}", path, crate::util::mtime_key(path), contain, bg);
        if key != self.img_key {
            let pic: Option<Picture> = media::load_picture(path);
            self.img = pic.map(|p| render::compose(Some(&p), contain, bg));
            self.img_key = key;
        }
        self.img.clone()
    }

    fn gif(&mut self, c: &Config) -> Option<Vec<(Pixmap, i32)>> {
        let path = &c.GifPath;
        if path.is_empty() || !std::path::Path::new(path).exists() {
            return None;
        }
        let key = format!("{}|{}|{}|{}", path, crate::util::mtime_key(path), c.FitContain, c.ColorRgb);
        if key != self.gif_key {
            self.gif = media::gif_panel_frames(path, c.FitContain, c.ColorRgb);
            self.gif_key = key;
            self.gif_idx = 0;
        }
        self.gif.clone()
    }

    fn capture(&mut self, r: RectI, c: &Config, delay: i32) -> Produced {
        if self.screen.as_ref().map(|s| s.rect) != Some(r) {
            self.screen = ScreenSource::new(r).ok();
        }
        let shot = self.screen.as_mut().map(|s| s.grab());
        match shot {
            Some(Ok(pic)) => {
                let frame = if r.w == 480 && r.h == 480 { render::to_pixmap(&pic).unwrap_or_else(|| render::blank(0)) } else { render::compose(Some(&pic), c.FitContain, 0) };
                Produced { frame, delay, note: None, direct: None }
            }
            Some(Err(e)) => Produced { frame: render::message(&format!("Screen capture failed:\n{e}")), delay: 500, note: Some(format!("Capture failed: {e}")), direct: None },
            None => Produced { frame: render::message("Screen capture needs the Windows version"), delay: 1000, note: Some("Screen capture unavailable".into()), direct: None },
        }
    }
}
