// TT LCD Studio: a frameless native window whose UI is one shared web page (ui/) drawn by the OS's own web engine
// (WebView2 on Windows, WebKitGTK on Linux), over the screen engine in ttlcd-core.
//   ttlcd-studio [--tray] [--screenshot <file.png> <dashboard|modes[:Mode]|device|settings|idle>]
use serde_json::Value;
use std::borrow::Cow;
use std::path::PathBuf;
use std::sync::atomic::Ordering::SeqCst;
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use tao::dpi::{LogicalSize, PhysicalPosition};
use tao::event::{Event, StartCause, WindowEvent};
use tao::event_loop::{ControlFlow, EventLoopBuilder};
use tao::window::{Fullscreen, ResizeDirection, Window, WindowBuilder};
use ttlcd_core::{Config, Engine, Mode};
use wry::http::{header, Request, Response};
use wry::WebViewBuilder;

mod os;
mod studio;
mod tray;

use studio::{Blobs, Studio};

/// Events for the UI thread (from the page, the tray, worker threads and the engine's control thread).
pub enum UserEvent {
    Ipc(String),
    Show,
    Menu(tray_icon::menu::MenuId),
    VideoInfo(Result<f64, String>),
    Thumb(Vec<u8>),
    CliDone(String, String),
    IdleProgress(i32, String),
    IdleDone(String, bool, bool),
    /// files dragged over the window (paths on enter), left again, or dropped
    DragEnter(Vec<String>),
    DragLeave,
    DragDrop(Vec<String>),
}

// the UI bundle build.rs makes from ui/ (React components + Sass)
const INDEX: &str = include_str!(concat!(env!("OUT_DIR"), "/ui/index.html"));
const STYLE: &str = include_str!(concat!(env!("OUT_DIR"), "/ui/app.css"));
const SCRIPT: &str = include_str!(concat!(env!("OUT_DIR"), "/ui/app.js"));
const ICON_PNG: &[u8] = include_bytes!("../../../linux/ttlcd-studio.png");

struct Shot {
    file: PathBuf,
    page: String,
    at: Option<Instant>,
    deadline: Instant,
}

fn icon_rgba() -> Option<(Vec<u8>, u32, u32)> {
    let img = image::load_from_memory_with_format(ICON_PNG, image::ImageFormat::Png).ok()?.to_rgba8();
    let (w, h) = img.dimensions();
    Some((img.into_raw(), w, h))
}

/// The custom protocol: the page, its files, the live frame, the idle picture and the video thumbnail.
fn serve(blobs: &Blobs, req: &Request<Vec<u8>>) -> Response<Cow<'static, [u8]>> {
    let path = req.uri().path();
    let (mime, body): (&str, Option<Cow<'static, [u8]>>) = match path {
        "/" | "/index.html" => ("text/html; charset=utf-8", Some(Cow::Borrowed(INDEX.as_bytes()))),
        "/app.css" => ("text/css; charset=utf-8", Some(Cow::Borrowed(STYLE.as_bytes()))),
        "/app.js" => ("text/javascript; charset=utf-8", Some(Cow::Borrowed(SCRIPT.as_bytes()))),
        "/icon.png" => ("image/png", Some(Cow::Borrowed(ICON_PNG))),
        // the coolant temperature as the cooler's idle screen draws it, over the preview's idle picture
        p if p.starts_with("/idletemp/") => ("image/png", p[10..].trim_end_matches(".png").parse::<i64>().ok().map(|t| Cow::Owned(ttlcd_core::idle_text::overlay_png(t)))),
        p if p.starts_with("/frame/") => ("image/jpeg", blobs.frame().map(|(_, j)| Cow::Owned(j.to_vec()))),
        p if p.starts_with("/idle/") => ("image/jpeg", blobs.idle.lock().unwrap().as_ref().map(|j| Cow::Owned(j.to_vec()))),
        p if p.starts_with("/thumb/") => ("image/jpeg", blobs.thumb.lock().unwrap().as_ref().map(|j| Cow::Owned(j.to_vec()))),
        _ => ("text/plain", None),
    };
    let b = Response::builder().header(header::CACHE_CONTROL, "no-store").header(header::ACCESS_CONTROL_ALLOW_ORIGIN, "*");
    match body {
        Some(body) => b.header(header::CONTENT_TYPE, mime).body(body),
        None => b.status(404).header(header::CONTENT_TYPE, "text/plain").body(Cow::Borrowed(&b"not found"[..])),
    }
    .unwrap()
}

fn resize_dir(s: &str) -> Option<ResizeDirection> {
    Some(match s {
        "North" => ResizeDirection::North,
        "South" => ResizeDirection::South,
        "West" => ResizeDirection::West,
        "East" => ResizeDirection::East,
        "NorthWest" => ResizeDirection::NorthWest,
        "NorthEast" => ResizeDirection::NorthEast,
        "SouthWest" => ResizeDirection::SouthWest,
        "SouthEast" => ResizeDirection::SouthEast,
        _ => return None,
    })
}

fn main() -> wry::Result<()> {
    let args: Vec<String> = std::env::args().collect();
    let start_in_tray = args.iter().any(|a| a == "--tray");
    let shot = args.iter().position(|a| a == "--screenshot").and_then(|i| {
        let file = args.get(i + 1)?;
        Some(Shot { file: PathBuf::from(file), page: args.get(i + 2).cloned().unwrap_or_else(|| "dashboard".into()), at: None, deadline: Instant::now() + Duration::from_secs(20) })
    });

    #[cfg(target_os = "linux")]
    if shot.is_some() {
        // the window pixels are read from X: XWayland on a Wayland desktop, Xvfb in CI
        std::env::set_var("GDK_BACKEND", "x11");
    }

    // one Studio per user: a second launch brings the running one to the front
    let _instance = if shot.is_none() {
        match os::single_instance() {
            Some(g) => Some(g),
            None => return Ok(()),
        }
    } else {
        None
    };
    let note = if shot.is_none() { os::before_start() } else { None };

    // TTLCD_NO_DEVICE=1: never open the cooler (UI tests); frames for the preview are rendered here instead
    let no_device = std::env::var("TTLCD_NO_DEVICE").is_ok_and(|v| v == "1");
    let engine = Engine::new(Config::load());
    if !no_device {
        engine.start();
    }
    let blobs = Arc::new(Blobs {
        engine: engine.clone(),
        fallback: Mutex::new(None),
        idle: Mutex::new(None),
        thumb: Mutex::new(None),
        quit: Default::default(),
        no_device,
    });
    if no_device {
        let b = blobs.clone();
        std::thread::spawn(move || {
            let mut seq = 0u64;
            while !b.quit.load(SeqCst) {
                let j = b.engine.render_once();
                seq += 1;
                *b.fallback.lock().unwrap() = Some((seq, Arc::new(j)));
                std::thread::sleep(Duration::from_millis(250));
            }
        });
    }

    let event_loop = EventLoopBuilder::<UserEvent>::with_user_event().build();
    let proxy = event_loop.create_proxy();
    let icon = icon_rgba();
    let height = std::env::var("TTLCD_SHOT_HEIGHT").ok().and_then(|v| v.parse::<f64>().ok()).unwrap_or(792.0);
    let window = WindowBuilder::new()
        .with_title("TT LCD Studio")
        .with_decorations(false)
        .with_inner_size(LogicalSize::new(1240.0, height))
        .with_min_inner_size(LogicalSize::new(1100.0, 700.0f64.min(height)))
        .with_visible(false)
        .with_window_icon(icon.as_ref().and_then(|(d, w, h)| tao::window::Icon::from_rgba(d.clone(), *w, *h).ok()))
        .build(&event_loop)
        .expect("window");
    #[cfg(windows)]
    {
        os::style_window(&window);
        if let Some(i) = os::window_icon() {
            window.set_window_icon(Some(i));
        }
    }
    if let Some(m) = window.current_monitor() {
        let (ms, mp, ws) = (m.size(), m.position(), window.outer_size());
        let x = mp.x + (ms.width as i32 - ws.width as i32).max(0) / 2;
        let y = mp.y + (ms.height as i32 - ws.height as i32).max(0) / 2;
        window.set_outer_position(PhysicalPosition::new(x, y));
    }

    #[cfg(windows)]
    if os::webview2_runtime_version().is_none() {
        os::message_box("TT LCD Studio", os::no_webview2_message());
        engine.stop();
        return Ok(());
    }
    // the web engine's data (cache, storage) lives in the local app-data folder, not next to the program
    let mut web_context = wry::WebContext::new(Some(ttlcd_core::config::local_folder().join(if cfg!(windows) { "WebView2" } else { "webkit" })));
    let ipc_proxy = proxy.clone();
    let pb = blobs.clone();
    let builder = WebViewBuilder::new_with_web_context(&mut web_context)
        .with_custom_protocol("ttlcd".into(), move |_id, req| serve(&pb, &req))
        .with_url(format!("{}index.html", os::ui_base_url()))
        .with_background_color((7, 7, 7, 255))
        .with_devtools(std::env::var("TTLCD_DEVTOOLS").is_ok_and(|v| v == "1"))
        // files dragged into the window: handled here (real paths; the page only shows the feedback)
        .with_drag_drop_handler({
            let p = proxy.clone();
            move |e: wry::DragDropEvent| {
                let paths = |v: &[PathBuf]| v.iter().map(|x| x.to_string_lossy().into_owned()).collect::<Vec<_>>();
                let ev = match e {
                    wry::DragDropEvent::Enter { paths: v, .. } => Some(UserEvent::DragEnter(paths(&v))),
                    wry::DragDropEvent::Drop { paths: v, .. } => Some(UserEvent::DragDrop(paths(&v))),
                    wry::DragDropEvent::Leave => Some(UserEvent::DragLeave),
                    _ => None,
                };
                if let Some(ev) = ev {
                    let _ = p.send_event(ev);
                }
                true // the web page must not open the file itself
            }
        })
        .with_ipc_handler(move |req: Request<String>| {
            let _ = ipc_proxy.send_event(UserEvent::Ipc(req.into_body()));
        });
    #[cfg(target_os = "linux")]
    let webview = {
        use tao::platform::unix::WindowExtUnix;
        use wry::WebViewBuilderExtUnix;
        builder.build_gtk(window.default_vbox().expect("gtk vbox"))?
    };
    #[cfg(not(target_os = "linux"))]
    let webview = builder.build(&window)?;
    // dropped before exiting, so the web engine shuts its page process down cleanly (see the quit path)
    let mut webview = Some(webview);

    let mut st = Studio::new(engine.clone(), blobs.clone(), proxy.clone());
    st.no_save = shot.is_some();
    if let Some(n) = note {
        st.set_note(n);
    }
    let mut shot = shot;
    if let Some(s) = &mut shot {
        // dev aid / CI: "modes:Clock" shows that mode's options (not saved); "idle" opens the Idle view
        if let Some((p, m)) = s.page.clone().split_once(':') {
            if let Some(mode) = Mode::parse(m) {
                st.change(|c| c.Mode = mode);
            }
            s.page = p.to_string();
        }
        if let Ok(sel) = std::env::var("TTLCD_SHOT_SELECT") {
            st.preselect(&sel);
        }
    }

    let show_now = shot.is_some() || !start_in_tray;
    if show_now {
        window.set_visible(true);
        st.set_visible(true);
    }
    if shot.is_none() {
        let p = proxy.clone();
        os::listen_show(Box::new(move || {
            let _ = p.send_event(UserEvent::Show);
        }));
    }

    let mut tray: Option<tray::Tray> = None;
    let mut next_status = Instant::now();
    let mut next_dash = Instant::now();
    let mut next_aura = Instant::now() + Duration::from_secs(3);
    let mut quitting = false;

    let show = |w: &Window, st: &mut Studio| {
        w.set_visible(true);
        w.set_minimized(false);
        w.set_focus();
        st.set_visible(true);
    };

    event_loop.run(move |event, _, flow| {
        let _keep_alive = &web_context; // the web engine's context lives as long as the loop
        let now = Instant::now();
        *flow = ControlFlow::WaitUntil(now + Duration::from_millis(40));
        let mut quit = false;
        match event {
            Event::NewEvents(StartCause::Init) => {
                if shot.is_none() {
                    tray = icon.as_ref().and_then(|(d, w, h)| tray::Tray::create(d.clone(), *w, *h, proxy.clone()));
                    st.has_tray = tray.is_some();
                    if !show_now && tray.is_none() {
                        show(&window, &mut st); // --tray without a tray: open the window instead
                    }
                }
            }
            Event::NewEvents(_) => {
                st.flush_dirty();
                st.tick_frame();
                st.tick_save(now);
                if now >= next_status {
                    next_status = now + Duration::from_millis(500);
                    st.full = window.fullscreen().is_some();
                    st.tick_status();
                }
                if now >= next_dash {
                    next_dash = now + Duration::from_secs(1);
                    st.tick_dash();
                    if let Some(t) = &mut tray {
                        t.sync(&st.engine, st.aura_ready);
                    }
                }
                if now >= next_aura {
                    next_aura = now + Duration::from_secs(3);
                    st.tick_aura();
                }
                if let Some(s) = &mut shot {
                    if st.ready && s.at.is_none() {
                        if s.page == "idle" {
                            st.idle_tried_read();
                            st.set_idle_view(true);
                            s.page = "dashboard".into();
                        }
                        st.out.push(format!("ttlcd.page({})", Value::from(s.page.clone())));
                        st.push_all();
                        // dev aid: TTLCD_SHOT_JS runs in the page before the capture (exercise clicks and drags)
                        if let Ok(js) = std::env::var("TTLCD_SHOT_JS") {
                            st.out.push(format!("setTimeout(function(){{{js}}}, 600)"));
                        }
                        s.at = Some(now + Duration::from_millis(2500));
                    }
                    if s.at.is_some_and(|t| now >= t) || now >= s.deadline {
                        if let Err(e) = os::screenshot(&window, &s.file) {
                            eprintln!("screenshot failed: {e}");
                        }
                        quit = true;
                    }
                }
            }
            Event::UserEvent(ev) => match ev {
                UserEvent::Ipc(body) => {
                    let Ok(msg) = serde_json::from_str::<Value>(&body) else { return };
                    if msg["t"] == "win" {
                        match msg["a"].as_str().unwrap_or("") {
                            "drag" => {
                                let _ = window.drag_window();
                            }
                            "resize" => {
                                if let Some(d) = msg["d"].as_str().and_then(resize_dir) {
                                    let _ = window.drag_resize_window(d);
                                }
                            }
                            "min" => window.set_minimized(true),
                            "full" => {
                                let full = window.fullscreen().is_some();
                                window.set_fullscreen(if full { None } else { Some(Fullscreen::Borderless(None)) });
                                st.full = !full;
                                st.push_status();
                            }
                            "close" => {
                                if st.engine.config().CloseToTray && st.has_tray {
                                    window.set_visible(false);
                                    st.set_visible(false);
                                } else {
                                    quit = true;
                                }
                            }
                            _ => {}
                        }
                    } else {
                        st.handle(&msg, &window);
                    }
                }
                UserEvent::Show => show(&window, &mut st),
                UserEvent::Menu(id) => {
                    let action = tray.as_ref().and_then(|t| t.action(&id));
                    match action {
                        Some(tray::Action::Open) => show(&window, &mut st),
                        Some(tray::Action::Mode(m)) => {
                            st.change(|c| c.Mode = m);
                            st.push_state();
                        }
                        Some(tray::Action::Pause) => {
                            st.engine.set_paused(!st.engine.paused());
                            st.push_status();
                        }
                        Some(tray::Action::Game) => {
                            st.engine.set_force_game(!st.engine.force_game());
                            st.push_status();
                        }
                        Some(tray::Action::Exit) => quit = true,
                        None => {}
                    }
                    if let Some(t) = &mut tray {
                        t.sync(&st.engine, st.aura_ready);
                    }
                }
                UserEvent::VideoInfo(r) => st.video_info(r),
                UserEvent::Thumb(j) => st.thumb(j),
                UserEvent::CliDone(what, out) => st.cli_done(&what, &out),
                UserEvent::IdleProgress(p, what) => st.idle_progress(p, &what),
                UserEvent::IdleDone(s, ok, store) => st.idle_done(s, ok, store),
                UserEvent::DragEnter(p) => st.drag_enter(p),
                UserEvent::DragLeave => st.drag_leave(),
                UserEvent::DragDrop(p) => st.drag_drop(p),
            },
            Event::WindowEvent { event: WindowEvent::CloseRequested, .. } => {
                if st.engine.config().CloseToTray && st.has_tray && shot.is_none() {
                    window.set_visible(false);
                    st.set_visible(false);
                } else {
                    quit = true;
                }
            }
            _ => {}
        }
        for s in st.out.drain(..) {
            if let Some(w) = &webview {
                let _ = w.evaluate_script(&s);
            }
        }
        if quit && !quitting {
            // on exit: save, stop the engine (it blanks the screen when BlankOnExit), hand back to the service
            quitting = true;
            window.set_visible(false);
            st.save_now();
            st.blobs.quit.store(true, SeqCst);
            tray = None;
            st.engine.stop();
            os::after_exit();
            // close the web view first and give it a moment: exiting while WebKitGTK is still tearing down its
            // separate page process made that process abort ("corrupted double-linked list") now and then
            drop(webview.take());
            #[cfg(target_os = "linux")]
            {
                let until = Instant::now() + Duration::from_millis(300);
                while Instant::now() < until {
                    if gtk::events_pending() {
                        gtk::main_iteration_do(false);
                    } else {
                        std::thread::sleep(Duration::from_millis(10));
                    }
                }
            }
            *flow = ControlFlow::Exit;
        }
    });
}
