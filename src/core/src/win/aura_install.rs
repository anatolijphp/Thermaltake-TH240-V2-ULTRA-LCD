//! Registration of the Aura HAL plugin (ASUS LightingService), port of AuraInstall in Aura.cs. The plugin is the native
//! COM server src/aura-hal: TTLcdAura.dll (32-bit, loaded by the 32-bit LightingService) and TTLcdAura64.dll (64-bit,
//! for 64-bit hosts), copied from next to ttlcd.exe into %ProgramData%\ttlcd\aura and registered in both registry
//! views. Signatures are used by the CLI and Studio; keep them.
use super::reg::{self, HKLM, VIEW32, VIEW64};
use std::io;
use std::time::Duration;

const CLSID: &str = "{C0C96C60-30E3-4021-9904-966EC4FA0F0F}";
const PROGID: &str = "TTLcd.AuraHal.1";
const CATEGORY: &str = "CLSID\\{109DC3E4-B9FF-4AF3-9008-AB13705D4E5F}\\Instance\\{E9BBD754-6CF4-492E-BA89-782177A2771B}\\Instance\\{C0C96C60-30E3-4021-9904-966EC4FA0F0F}";
const DLL32: &str = "TTLcdAura.dll";
const DLL64: &str = "TTLcdAura64.dll";
const LIGHTING: &str = "LightingService";

fn dir() -> std::path::PathBuf {
    super::program_data().join("ttlcd").join("aura")
}

fn category_key() -> String {
    format!("SOFTWARE\\Classes\\{CATEGORY}")
}

fn clsid_key() -> String {
    format!("SOFTWARE\\Classes\\CLSID\\{CLSID}")
}

fn tf(b: bool) -> &'static str {
    if b { "True" } else { "False" }
}

/// `ttlcd aura install|uninstall|status` (install/uninstall elevate themselves).
pub fn run(action: &str) -> io::Result<u8> {
    match action.to_lowercase().as_str() {
        "status" => {
            println!("plugin registered: {}", tf(installed()));
            match crate::aura::Reader::default().read() {
                Some(s) => println!("effect {}, {} LEDs, seq {}, updated {}", s.effect, s.led_count, s.seq, local_hms(s.updated_ticks)),
                None => println!("no data from Armoury Crate yet"),
            }
            Ok(0)
        }
        "install" => Ok(if super::elevated("aura install", "ttlcd-aura.log")? { install()? } else { 0 }),
        "uninstall" => Ok(if super::elevated("aura uninstall", "ttlcd-aura.log")? { uninstall() } else { 0 }),
        _ => {
            println!("ttlcd aura install|uninstall|status");
            Ok(1)
        }
    }
}

/// The plugin is registered (32-bit registry view).
pub fn installed() -> bool {
    reg::key_exists(HKLM, &category_key(), VIEW32)
}

fn install() -> io::Result<u8> {
    let exe_dir = std::env::current_exe()?.parent().map(|d| d.to_path_buf()).unwrap_or_default();
    let src32 = exe_dir.join(DLL32);
    let src64 = exe_dir.join(DLL64);
    if !src32.is_file() {
        return Err(io::Error::new(io::ErrorKind::NotFound, format!("{DLL32} missing next to ttlcd.exe ({})", src32.display())));
    }
    let dir = dir();
    std::fs::create_dir_all(&dir)?;
    lighting(false); // releases the plugin DLL
    let dll32 = dir.join(DLL32);
    std::fs::copy(&src32, &dll32)?;
    let dll64 = if src64.is_file() {
        let d = dir.join(DLL64);
        std::fs::copy(&src64, &d)?;
        Some(d)
    } else {
        println!("note: {DLL64} missing next to ttlcd.exe; registered for 32-bit hosts only");
        let _ = std::fs::remove_file(dir.join(DLL64));
        None
    };
    let mut ok = true;
    for (view, dll) in [(VIEW32, Some(dll32)), (VIEW64, dll64)] {
        let clsid = clsid_key();
        // clears what an older (.NET) registration left under InprocServer32 (mscoree, Assembly, CodeBase, ...)
        reg::delete_tree(HKLM, &clsid, view);
        let Some(dll) = dll else {
            reg::delete_tree(HKLM, &category_key(), view);
            continue;
        };
        let ip = format!("{clsid}\\InprocServer32");
        ok &= reg::set_string_view(HKLM, &clsid, "", "TT LCD Aura HAL", view);
        ok &= reg::set_string_view(HKLM, &ip, "", &dll.to_string_lossy(), view);
        ok &= reg::set_string_view(HKLM, &ip, "ThreadingModel", "Both", view);
        ok &= reg::set_string_view(HKLM, &format!("{clsid}\\ProgID"), "", PROGID, view);
        let c = category_key();
        for (name, value) in [
            ("Description", "Thermaltake CL-W383 LCD (TT LCD Studio)"),
            ("Name", "TT LCD Studio"),
            ("Manufacturer", "TT LCD Studio"),
            ("DeviceModel", "CL-W383 LCD"),
            ("DeviceType", "External"),
            ("Version", "1.0.0.0"),
            ("SpecVersion", "1.00.00"),
        ] {
            ok &= reg::set_string_view(HKLM, &c, name, value, view);
        }
        ok &= reg::set_dword_view(HKLM, &c, "Pluging", 0, view);
    }
    lighting(true);
    if !ok {
        println!("error: some registry values could not be written");
        return Ok(2);
    }
    println!("Aura plugin installed; ASUS LightingService restarted.");
    println!("Open Armoury Crate > Aura Sync: the cooler appears as \"TT LCD CL-W383\". Select the Aura Sync mode in TT LCD Studio.");
    Ok(0)
}

fn uninstall() -> u8 {
    lighting(false);
    for view in [VIEW32, VIEW64] {
        reg::delete_tree(HKLM, &category_key(), view);
        reg::delete_tree(HKLM, &clsid_key(), view);
    }
    let d = dir();
    if d.exists() {
        if let Err(e) = std::fs::remove_dir_all(&d) {
            println!("note: {e}");
        }
    }
    lighting(true);
    println!("Aura plugin removed; ASUS LightingService restarted. Registered: {}", tf(installed()));
    0
}

/// Stops (start = false) or starts ASUS LightingService, waiting up to 30 s; a stop releases the plugin DLL.
fn lighting(start: bool) {
    if let Err(e) = super::sensors::start_stop_service(LIGHTING, start, Duration::from_secs(30)) {
        println!("{LIGHTING}: {e}");
    }
}

/// "HH:mm:ss" local time of a .NET UTC tick count (the block's update time).
fn local_hms(ticks: i64) -> String {
    if ticks <= 0 {
        return "--:--:--".into();
    }
    let age = ((crate::util::dotnet_ticks_now() - ticks) / 10_000_000).max(0);
    let now = crate::render::local_now();
    let secs = (now.hour as i64 * 3600 + now.minute as i64 * 60 + now.second as i64 - age).rem_euclid(86_400);
    format!("{:02}:{:02}:{:02}", secs / 3600, secs / 60 % 60, secs % 60)
}
