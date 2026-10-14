// ttlcd: command-line tool and headless screen service for the Thermaltake CL-W383 cooler LCD.
use std::process::ExitCode;
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread;
use std::time::{Duration, Instant};
use ttlcd_core::config::{self, Config, Mode};
use ttlcd_core::{capture, engine, hid, lcd, media, model, render, store, Engine, Lcd};

static STOP: AtomicBool = AtomicBool::new(false);

const USAGE: &str = "ttlcd - Thermaltake CL-W383 LCD tool
  ttlcd probe                         list HID interfaces (read-only)
  ttlcd info                          serial, type, rotation
  ttlcd temp                          coolant temperature from the cooler's sensor
  ttlcd brightness <0-100>
  ttlcd rotate <0|90|180|270>         hardware rotation
  ttlcd color <RRGGBB>                fill screen with a colour
  ttlcd image <file> [-q 85]          show jpg/png/bmp
  ttlcd gif <file> [-q 85]            loop an animated GIF (Ctrl+C to stop)
  ttlcd idle <file> [-rot 0] [-q 85]  store the idle (standby) screen in the cooler: picture/GIF/video frame
  ttlcd files [folder]                list files stored in the cooler (read-only)
  ttlcd monitors                      list monitors
  ttlcd games                         what game detection sees right now
  ttlcd stream [-m N] [-r x,y,w,h] [-fps 30] [-q 80]
                                      mirror a monitor (default: the 480x480 one) or region
  ttlcd display install|uninstall|status   virtual 480x480 monitor for Wallpaper Engine (admin)
  ttlcd aura install|uninstall|status      Armoury Crate / Aura Sync plugin (admin)
  ttlcd service install|uninstall|status   CPU temperature service, starts with Windows (admin)
  ttlcd hwmon                              CPU temperature helper in this console (admin)
  ttlcd run                           drive the screen from the settings (service; Ctrl+C to stop)
  ttlcd config                        show the settings (%APPDATA%\\ttlcd, ~/.config/ttlcd on Linux)
  ttlcd set <name> <value>            change a setting, e.g. set Mode Clock, set GifPath /path/a.gif
  ttlcd render <out.jpg> [mode]       render one frame of the current mode to a file (no cooler needed)
  ttlcd version";

type R = Result<u8, String>;

fn main() -> ExitCode {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let cmd = args.first().map(|s| s.to_lowercase()).unwrap_or_else(|| "help".into());
    on_ctrl_c();
    match run(&cmd, &args) {
        Ok(code) => ExitCode::from(code),
        Err(e) => {
            eprintln!("error: {e}");
            ExitCode::from(2)
        }
    }
}

fn run(cmd: &str, a: &[String]) -> R {
    match cmd {
        "probe" => probe(),
        "run" => service_run(),
        "config" => show_config(),
        "set" => set(arg(a, 1)?, arg(a, 2)?),
        "render" => render_file(arg(a, 1)?, a.get(2).map(String::as_str)),
        "version" | "--version" => {
            println!("ttlcd {}", ttlcd_core::VERSION);
            Ok(0)
        }
        "info" => {
            let l = open()?;
            println!("serial={} type={} rotation={}", l.serial().map_err(s)?, l.device_type().map_err(s)?, l.rotation().map_err(s)?);
            Ok(0)
        }
        "temp" => {
            let l = open()?;
            // the first control reply after opening can take ~10 s
            for i in 0.. {
                match l.coolant_c() {
                    Ok(t) => {
                        println!("coolant={t:.2} C");
                        return Ok(0);
                    }
                    Err(e) if e.kind() == std::io::ErrorKind::TimedOut && i < 15 => continue,
                    Err(e) => return Err(e.to_string()),
                }
            }
            Ok(0)
        }
        "brightness" => {
            let v: i32 = num(arg(a, 1)?)?;
            open()?.set_brightness(v.clamp(0, 100) as u8).map_err(s)?;
            Ok(0)
        }
        "rotate" => {
            open()?.set_rotation(num(arg(a, 1)?)?).map_err(s)?;
            Ok(0)
        }
        "color" => {
            let hex = arg(a, 1)?;
            let v = i32::from_str_radix(hex.trim_start_matches('#'), 16).map_err(|_| format!("not a colour: {hex}"))?;
            let l = open()?;
            l.show_jpeg(&render::jpeg(&render::blank(v), 90, l.swap_red_blue)).map_err(s)?;
            Ok(0)
        }
        "image" => {
            let pic = media::load_picture(arg(a, 1)?).ok_or("cannot read that picture")?;
            let l = open()?;
            l.show_jpeg(&render::jpeg(&render::compose(Some(&pic), false, 0), opt(a, "-q", 85), l.swap_red_blue)).map_err(s)?;
            Ok(0)
        }
        "gif" => gif(arg(a, 1)?, opt(a, "-q", 85)),
        "idle" => idle(arg(a, 1)?, opt(a, "-rot", 0), opt(a, "-q", 85)),
        "boot" => Err("storing a boot animation is disabled: uploads erased the original firmware's flash once, and the LX800 boot upload was never verified".into()),
        "files" => {
            for f in open()?.list_files(a.get(1).map(String::as_str).unwrap_or("")).map_err(s)? {
                println!("{f}");
            }
            Ok(0)
        }
        "monitors" => {
            let m = capture::monitors();
            if m.is_empty() {
                println!("no monitors (screen capture needs the Windows version)");
            }
            for (i, m) in m.iter().enumerate() {
                println!("{}: {} {}x{} at {},{}{}", i, m.name, m.w, m.h, m.x, m.y, if m.primary { " (primary)" } else { "" });
            }
            Ok(0)
        }
        "stream" => stream(a),
        "games" => {
            // what game detection sees now (with the saved detection settings)
            let c = Config::load();
            match ttlcd_core::games::detect(&c) {
                Some(g) => println!("game running: {g}"),
                None => println!("no game detected"),
            }
            Ok(0)
        }
        #[cfg(windows)]
        "display" => ttlcd_core::win::vdd::run(arg(a, 1)?).map_err(s),
        #[cfg(windows)]
        "aura" => ttlcd_core::win::aura_install::run(arg(a, 1)?).map_err(s),
        #[cfg(windows)]
        "service" => ttlcd_core::win::sensors::run_service_cmd(arg(a, 1)?).map_err(s),
        #[cfg(windows)]
        "hwmon" => ttlcd_core::win::sensors::run_helper(&|| STOP.load(Ordering::SeqCst)).map_err(s),
        #[cfg(windows)]
        "sensor-service" => ttlcd_core::win::sensors::service_main().map_err(s), // started by Windows
        #[cfg(not(windows))]
        "display" | "service" | "hwmon" => Err("this command needs the Windows version".into()),
        #[cfg(not(windows))]
        "aura" => {
            // Linux: Aura Sync comes from armory-crated; there is nothing to install here
            let snap = ttlcd_core::aura::Reader::default().read();
            println!("armory-crated installed: {}", ttlcd_core::aura::linux_service_installed());
            println!("{}", match snap {
                Some(s) => format!("effect {}, {} LEDs, seq {}", s.effect, s.led_count, s.seq),
                None => "no data from armory-crated yet (armory-crate virtual add ttlcd)".into(),
            });
            Ok(0)
        }
        _ => {
            println!("{USAGE}");
            Ok(1)
        }
    }
}

fn s(e: impl std::fmt::Display) -> String {
    e.to_string()
}

fn arg(a: &[String], i: usize) -> Result<&str, String> {
    a.get(i).map(String::as_str).ok_or_else(|| "missing argument; run 'ttlcd' for usage".to_string())
}

fn num<T: std::str::FromStr>(v: &str) -> Result<T, String> {
    v.trim().parse().map_err(|_| format!("not a number: {v}"))
}

fn opt(a: &[String], name: &str, def: i32) -> i32 {
    a.windows(2).find(|w| w[0] == name).and_then(|w| w[1].parse().ok()).unwrap_or(def)
}

fn opt_s<'a>(a: &'a [String], name: &str) -> Option<&'a str> {
    a.windows(2).find(|w| w[0] == name).map(|w| w[1].as_str())
}

fn open() -> Result<Lcd, String> {
    Lcd::open().map_err(s)
}

fn probe() -> R {
    let mut devs = hid::enumerate(lcd::VID, lcd::PID);
    devs.extend(hid::enumerate(lcd::ALT_VID, lcd::ALT_PID)); // LX800 firmware
    if devs.is_empty() {
        println!("No cooler LCD found (264A:233C or 1A2C:151D).");
        return Ok(1);
    }
    for d in devs {
        println!("{:04X}:{:04X} MI_{:02}\n  {}", d.vid, d.pid, d.interface, d.path);
    }
    Ok(0)
}

/// Modes this system can't show fall back to the clock (without changing the saved setting).
fn effective(mut c: Config) -> Config {
    if !model::supports(c.Mode) {
        println!("mode {} needs Windows; showing the clock (ttlcd set Mode Clock|Image|Gif|Color|Off)", c.Mode);
        c.Mode = Mode::Clock;
    }
    c
}

/// Headless: what Studio does, without the window. Settings are re-read when the file changes (ttlcd set, Studio).
fn service_run() -> R {
    let path = config::file_path();
    let engine = Engine::new(effective(Config::load()));
    engine.start();
    println!("driving the cooler screen ({}); settings: {}", engine::label(engine.config().Mode), path.display());
    let stamp = |p: &std::path::Path| std::fs::metadata(p).and_then(|m| m.modified()).ok();
    let mut last = stamp(&path);
    let mut shown = String::new();
    while !STOP.load(Ordering::SeqCst) {
        thread::sleep(Duration::from_secs(1));
        let t = stamp(&path);
        if t != last {
            last = t;
            engine.set_config(effective(Config::load()));
            println!("settings reloaded");
        }
        // a cooler with a measured calibration (model::KNOWN_MOUNTS) gets it once, saved like Studio does
        if let Some(key) = engine.device_key().filter(|k| model::apply_known_mount(&mut (*engine.config()).clone(), k)) {
            let mut c = Config::load();
            model::apply_known_mount(&mut c, &key);
            if c.save().is_ok() {
                last = stamp(&path);
            }
            engine.set_config(effective(c));
            println!("calibration for {key} applied");
        }
        let st = engine.status();
        if st != shown {
            println!("{st}");
            shown = st;
        }
    }
    engine.stop();
    Ok(0)
}

fn show_config() -> R {
    let c = Config::load();
    println!("# {}", config::file_path().display());
    for (name, v) in c.listing() {
        println!("{name} = {v}");
    }
    Ok(0)
}

fn set(name: &str, value: &str) -> R {
    let mut c = Config::load();
    let (name, shown) = c.set_cli(name, value)?;
    c.save().map_err(s)?;
    println!("{name} = {shown}");
    if cfg!(target_os = "linux") && name == "Mode" && c.Mode == Mode::Aura && !ttlcd_core::aura::linux_service_installed() {
        println!("Note: armory-crate (armory-crated) isn't installed; the screen waits for it until it is.");
    }
    Ok(0)
}

fn render_file(file: &str, mode: Option<&str>) -> R {
    let mut c = Config::load();
    if let Some(m) = mode {
        c.Mode = Mode::parse(m).ok_or_else(|| format!("unknown mode {m}"))?;
    }
    let label = engine::label(c.Mode);
    let e = Engine::new(effective(c));
    let jpg = e.render_once();
    std::fs::write(file, &jpg).map_err(s)?;
    println!("{}: {} frame, {} KB", file, label, jpg.len() / 1024);
    Ok(0)
}

fn percent(p: i32) {
    use std::io::Write;
    print!("\r{p:3}%");
    let _ = std::io::stdout().flush();
}

fn idle(path: &str, rot: i32, q: i32) -> R {
    let jpg = store::standby_jpeg(path, rot, q).map_err(s)?;
    println!("idle picture: {} KB", jpg.len() / 1024);
    let l = open()?;
    let ok = store::write_standby(&l, &jpg, &mut percent).map_err(s)?;
    println!("{}", if ok { "\rstored and verified (read-back matches)" } else { "\rstored but read-back did NOT match" });
    Ok(if ok { 0 } else { 3 })
}

fn gif(path: &str, q: i32) -> R {
    let l = open()?;
    let frames: Vec<(Vec<u8>, i32)> = media::gif_panel_frames(path, false, 0)
        .ok_or("cannot read that GIF")?
        .into_iter()
        .map(|(p, d)| (render::jpeg(&p, q, l.swap_red_blue), d))
        .collect();
    println!("{} frames, Ctrl+C to stop", frames.len());
    let start = Instant::now();
    let mut next = 0i64;
    while !STOP.load(Ordering::SeqCst) {
        for (j, d) in &frames {
            if STOP.load(Ordering::SeqCst) {
                break;
            }
            l.show_jpeg(j).map_err(s)?;
            next += *d as i64;
            let wait = next - start.elapsed().as_millis() as i64;
            if wait > 0 {
                thread::sleep(Duration::from_millis(wait as u64));
            } else if wait < -500 {
                next = start.elapsed().as_millis() as i64; // fell behind, resync
            }
        }
    }
    Ok(0)
}

fn stream(a: &[String]) -> R {
    let (fps, q) = (opt(a, "-fps", 30), opt(a, "-q", 80));
    let mon = match opt_s(a, "-m") {
        Some(m) => num(m)?,
        None => capture::find_by_size(480, 480).ok_or("no 480x480 monitor found; use -m N (see 'ttlcd monitors') or 'ttlcd display install'")?,
    };
    let sub = match opt_s(a, "-r") {
        Some(r) => {
            let p: Vec<i32> = r.split(',').filter_map(|v| v.trim().parse().ok()).collect();
            if p.len() != 4 {
                return Err("-r needs x,y,w,h".into());
            }
            Some(capture::RectI { x: p[0], y: p[1], w: p[2], h: p[3] })
        }
        None => None,
    };
    let rect = capture::monitor_rect(mon, sub).ok_or_else(|| format!("no monitor {mon}"))?;
    println!("streaming monitor {} {:?} at up to {} fps, Ctrl+C to stop", mon, rect, fps);
    let frame_ms = 1000 / fps.max(1) as u64;
    let mut src = capture::ScreenSource::new(rect).map_err(s)?;
    let l = open()?;
    let (mut frames, mut report) = (0u32, Instant::now());
    while !STOP.load(Ordering::SeqCst) {
        let t0 = Instant::now();
        let shot = src.grab().map_err(s)?;
        let p = if rect.w == 480 && rect.h == 480 { render::to_pixmap(&shot).unwrap() } else { render::compose(Some(&shot), false, 0) };
        let jpg = render::jpeg(&p, q, l.swap_red_blue);
        l.show_jpeg(&jpg).map_err(s)?;
        frames += 1;
        if report.elapsed() >= Duration::from_secs(5) {
            println!("{:.1} fps, {} KB/frame", frames as f64 / report.elapsed().as_secs_f64(), jpg.len() / 1024);
            frames = 0;
            report = Instant::now();
        }
        if let Some(w) = Duration::from_millis(frame_ms).checked_sub(t0.elapsed()) {
            thread::sleep(w);
        }
    }
    Ok(0)
}

fn on_ctrl_c() {
    #[cfg(unix)]
    unsafe {
        extern "C" fn h(_: libc::c_int) {
            STOP.store(true, Ordering::SeqCst);
        }
        libc::signal(libc::SIGINT, h as *const () as libc::sighandler_t);
        libc::signal(libc::SIGTERM, h as *const () as libc::sighandler_t);
    }
    #[cfg(windows)]
    unsafe {
        unsafe extern "system" fn h(_: u32) -> i32 {
            STOP.store(true, Ordering::SeqCst);
            1
        }
        windows_sys::Win32::System::Console::SetConsoleCtrlHandler(Some(h), 1);
    }
}
