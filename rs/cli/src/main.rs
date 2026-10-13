// ttlcd-rs: command-line proof of concept for the Rust port (device info, colour, brightness).
use std::{env, process, thread, time::Duration};
use ttlcd_core::{hid, lcd, Lcd};

fn usage() -> ! {
    eprintln!("usage: ttlcd-rs list | info | color <RRGGBB> [seconds] | brightness <0-100>");
    process::exit(2);
}

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();
    if let Err(e) = run(&args) {
        eprintln!("error: {e}");
        process::exit(1);
    }
}

fn run(args: &[String]) -> std::io::Result<()> {
    match args.first().map(String::as_str) {
        Some("list") => {
            for (v, p) in [(lcd::VID, lcd::PID), (lcd::ALT_VID, lcd::ALT_PID)] {
                for d in hid::enumerate(v, p) {
                    println!("{:04X}:{:04X} MI_{:02} {}", d.vid, d.pid, d.interface, d.path);
                }
            }
        }
        Some("info") => {
            let mut l = Lcd::open()?;
            println!("serial      {}", l.serial()?);
            println!("type        {}", l.device_type()?);
            println!("rotation    {}°", l.rotation()?);
            println!("coolant     {:.2} °C", l.coolant_c()?);
            println!("firmware    {}", if l.swap_red_blue { "LX800 (1A2C:151D)" } else { "Thermaltake (264A:233C)" });
        }
        Some("color") => {
            let hex = args.get(1).unwrap_or_else(|| usage());
            let v = u32::from_str_radix(hex.trim_start_matches('#'), 16).unwrap_or_else(|_| usage());
            let secs: u64 = args.get(2).and_then(|s| s.parse().ok()).unwrap_or(5);
            let mut l = Lcd::open()?;
            let jpeg = l.jpeg_rgb(&lcd::solid((v >> 16) as u8, (v >> 8) as u8, v as u8), 90)?;
            // keep-alive: the LX800 firmware falls back to its own screen when frames stop for a few seconds
            for _ in 0..secs * 2 {
                l.show_jpeg(&jpeg)?;
                thread::sleep(Duration::from_millis(500));
            }
        }
        Some("brightness") => {
            let v: u8 = args.get(1).and_then(|s| s.parse().ok()).unwrap_or_else(|| usage());
            Lcd::open()?.set_brightness(v)?;
        }
        _ => usage(),
    }
    Ok(())
}
