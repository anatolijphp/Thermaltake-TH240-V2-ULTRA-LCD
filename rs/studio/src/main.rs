// TT LCD Studio (Rust proof of concept): a native window whose UI is HTML drawn by the OS's own web engine
// (WebView2 on Windows, WebKitGTK on Linux), talking to a device thread that owns the cooler.
use std::sync::mpsc::{self, RecvTimeoutError};
use std::thread;
use std::time::{Duration, Instant};
use tao::event::{Event, WindowEvent};
use tao::event_loop::{ControlFlow, EventLoopBuilder, EventLoopProxy};
use tao::window::WindowBuilder;
use ttlcd_core::{lcd, Lcd};
use wry::WebViewBuilder;

mod os;

enum Cmd {
    Color(u32),
    Brightness(u8),
    Quit,
}

/// JSON string literal (the only escaping the status messages need).
fn js(s: &str) -> String {
    let mut o = String::from("\"");
    for c in s.chars() {
        match c {
            '"' => o.push_str("\\\""),
            '\\' => o.push_str("\\\\"),
            c if (c as u32) < 0x20 => o.push_str(&format!("\\u{:04x}", c as u32)),
            c => o.push(c),
        }
    }
    o.push('"');
    o
}

/// The cooler lives on this thread: it opens (and reopens) the device, streams the current frame with a 500 ms
/// keep-alive (the LX800 firmware falls back to its own screen otherwise) and polls the coolant temperature.
fn device_thread(rx: mpsc::Receiver<Cmd>, ui: EventLoopProxy<String>) {
    let status = |s: String| {
        let _ = ui.send_event(s);
    };
    let mut lcd: Option<Lcd> = None;
    let mut color = 0x2080ffu32;
    let mut jpeg: Option<Vec<u8>> = None;
    let mut last_temp = Instant::now() - Duration::from_secs(10);
    let mut frames = 0u32;
    let mut fps_t = Instant::now();
    loop {
        if lcd.is_none() {
            match Lcd::open() {
                Ok(mut l) => {
                    let serial = l.serial().unwrap_or_default();
                    let rot = l.rotation().unwrap_or(0);
                    let fw = if l.swap_red_blue { "LX800 rescue (1A2C:151D)" } else { "Thermaltake (264A:233C)" };
                    status(format!(
                        "onStatus({{connected:true,serial:{},rotation:{rot},firmware:{}}})",
                        js(&serial),
                        js(fw)
                    ));
                    jpeg = None;
                    lcd = Some(l);
                }
                Err(e) => {
                    status(format!("onStatus({{connected:false,message:{}}})", js(&e.to_string())));
                    match rx.recv_timeout(Duration::from_secs(2)) {
                        Ok(Cmd::Quit) | Err(RecvTimeoutError::Disconnected) => return,
                        Ok(Cmd::Color(c)) => color = c,
                        _ => {}
                    }
                    continue;
                }
            }
        }
        let l = lcd.as_mut().unwrap();
        let r = (|| -> std::io::Result<bool> {
            match rx.recv_timeout(Duration::from_millis(500)) {
                Ok(Cmd::Quit) | Err(RecvTimeoutError::Disconnected) => return Ok(false),
                Ok(Cmd::Color(c)) => {
                    color = c;
                    jpeg = None;
                }
                Ok(Cmd::Brightness(b)) => l.set_brightness(b)?,
                Err(RecvTimeoutError::Timeout) => {}
            }
            if jpeg.is_none() {
                jpeg = Some(l.jpeg_rgb(&lcd::solid((color >> 16) as u8, (color >> 8) as u8, color as u8), 90)?);
            }
            l.show_jpeg(jpeg.as_ref().unwrap())?;
            frames += 1;
            if last_temp.elapsed() >= Duration::from_secs(2) {
                // a query without a reply is not a lost connection: the cooler has been seen to stop answering
                // queries for a while while frames still work (CLAUDE.md, 2026-10-09)
                let t = match l.coolant_c() {
                    Ok(t) => format!("{t:.1}"),
                    Err(e) if e.kind() == std::io::ErrorKind::TimedOut => "null".into(),
                    Err(e) => return Err(e),
                };
                let fps = frames as f64 / fps_t.elapsed().as_secs_f64();
                frames = 0;
                fps_t = Instant::now();
                last_temp = Instant::now();
                status(format!("onReading({{coolant:{t},fps:{fps:.1}}})"));
            }
            Ok(true)
        })();
        match r {
            Ok(true) => {}
            Ok(false) => return, // dropping `lcd` here releases the device on its own thread
            Err(e) => {
                status(format!("onStatus({{connected:false,message:{}}})", js(&format!("Connection lost: {e}"))));
                lcd = None;
            }
        }
    }
}

fn main() -> wry::Result<()> {
    let event_loop = EventLoopBuilder::<String>::with_user_event().build();
    let window = WindowBuilder::new()
        .with_title("TT LCD Studio")
        .with_inner_size(tao::dpi::LogicalSize::new(760.0, 520.0))
        .with_min_inner_size(tao::dpi::LogicalSize::new(600.0, 440.0))
        .build(&event_loop)
        .expect("window");

    let (tx, rx) = mpsc::channel();
    let proxy = event_loop.create_proxy();
    let dev = thread::spawn(move || device_thread(rx, proxy));

    let ipc_tx = tx.clone();
    let builder = WebViewBuilder::new()
        .with_html(include_str!("../ui/index.html"))
        .with_ipc_handler(move |req: wry::http::Request<String>| {
            // messages from the page: "color:RRGGBB", "brightness:N"
            let (k, v) = req.body().split_once(':').unwrap_or(("", ""));
            let cmd = match k {
                "color" => u32::from_str_radix(v, 16).ok().map(Cmd::Color),
                "brightness" => v.parse().ok().map(Cmd::Brightness),
                _ => None,
            };
            if let Some(c) = cmd {
                let _ = ipc_tx.send(c);
            }
        });

    #[cfg(target_os = "linux")]
    let webview = {
        use tao::platform::unix::WindowExtUnix;
        use wry::WebViewBuilderExtUnix;
        builder.build_gtk(window.default_vbox().expect("gtk vbox"))?
    };
    #[cfg(not(target_os = "linux"))]
    let webview = builder.build(&window)?;

    let mut dev = Some(dev);
    event_loop.run(move |event, _, flow| {
        *flow = ControlFlow::Wait;
        match event {
            Event::UserEvent(script) => {
                let _ = webview.evaluate_script(&script);
            }
            Event::WindowEvent { event: WindowEvent::CloseRequested, .. } => {
                let _ = tx.send(Cmd::Quit);
                if let Some(d) = dev.take() {
                    let _ = d.join();
                }
                *flow = ControlFlow::Exit;
            }
            _ => {}
        }
    });
}
