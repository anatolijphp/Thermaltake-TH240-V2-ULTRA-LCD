//! PC readings: total CPU load, RAM use and CPU temperature.
//! CPU temperature on Linux comes from the CPU sensor driver's hwmon node (k10temp / zenpower / coretemp). On Windows
//! it comes from the `ttlcd hwmon` helper or the TTLcdSensors service (TOWEI's XYDataCenter.dll, see win/sensors.rs),
//! which publishes it in the shared block "Global\TTLcdSensors":
//!   0 magic "TSEN" | 4 cpuTempC (f32) | 8 gpuTempC | 12 cpuLoad | 16 updated (.NET ticks) | 24 intervalMs (Studio).
use std::sync::Mutex;
use std::time::{Duration, Instant};

pub const MAP_SIZE: usize = 32;
pub const MAP_MAGIC: u32 = 0x4E45_5354; // "TSEN"
pub const INTERVAL_OFFSET: usize = 24;

struct Load {
    last_idle: u64,
    last_total: u64,
    last: i32,
    at: Option<Instant>,
}

static LOAD: Mutex<Load> = Mutex::new(Load { last_idle: 0, last_total: 0, last: 0, at: None });

/// Total CPU load in percent (refreshed at most every 0.9 s).
pub fn cpu_percent() -> i32 {
    let mut l = LOAD.lock().unwrap();
    if l.at.is_some_and(|a| a.elapsed() < Duration::from_millis(900)) {
        return l.last;
    }
    let Some((idle, total)) = cpu_times() else { return l.last };
    let (dt, di) = (total.wrapping_sub(l.last_total), idle.wrapping_sub(l.last_idle));
    if l.last_total != 0 && dt > 0 {
        l.last = (100.0 * dt.saturating_sub(di) as f64 / dt as f64).round() as i32;
    }
    l.last_idle = idle;
    l.last_total = total;
    l.at = Some(Instant::now());
    l.last
}

#[cfg(target_os = "linux")]
fn cpu_times() -> Option<(u64, u64)> {
    // /proc/stat "cpu  user nice system idle iowait irq softirq steal ..." (jiffies)
    let s = std::fs::read_to_string("/proc/stat").ok()?;
    let f: Vec<u64> = s.lines().next()?.split_whitespace().skip(1).take(8).filter_map(|v| v.parse().ok()).collect();
    if f.len() < 4 {
        return None;
    }
    let total = f.iter().sum();
    let idle = f[3] + f.get(4).copied().unwrap_or(0);
    Some((idle, total))
}

#[cfg(windows)]
fn cpu_times() -> Option<(u64, u64)> {
    use windows_sys::Win32::Foundation::FILETIME;
    let ft = |f: FILETIME| (f.dwHighDateTime as u64) << 32 | f.dwLowDateTime as u64;
    unsafe {
        let (mut i, mut k, mut u) = (std::mem::zeroed(), std::mem::zeroed(), std::mem::zeroed());
        if windows_sys::Win32::System::Threading::GetSystemTimes(&mut i, &mut k, &mut u) == 0 {
            return None;
        }
        Some((ft(i), ft(k) + ft(u))) // kernel includes idle
    }
}

#[cfg(not(any(windows, target_os = "linux")))]
fn cpu_times() -> Option<(u64, u64)> {
    None
}

/// Physical RAM in use, percent.
pub fn ram_percent() -> i32 {
    #[cfg(target_os = "linux")]
    {
        let Ok(s) = std::fs::read_to_string("/proc/meminfo") else { return 0 };
        let mut total = 0u64;
        let mut avail = 0u64;
        for line in s.lines() {
            let mut it = line.split([' ', ':']).filter(|x| !x.is_empty());
            match (it.next(), it.next().and_then(|v| v.parse::<u64>().ok())) {
                (Some("MemTotal"), Some(v)) => total = v,
                (Some("MemAvailable"), Some(v)) => avail = v,
                _ => {}
            }
        }
        if total > 0 { (100.0 * (total - avail) as f64 / total as f64).round() as i32 } else { 0 }
    }
    #[cfg(windows)]
    unsafe {
        use windows_sys::Win32::System::SystemInformation::{GlobalMemoryStatusEx, MEMORYSTATUSEX};
        let mut m: MEMORYSTATUSEX = std::mem::zeroed();
        m.dwLength = std::mem::size_of::<MEMORYSTATUSEX>() as u32;
        if GlobalMemoryStatusEx(&mut m) != 0 { m.dwMemoryLoad as i32 } else { 0 }
    }
    #[cfg(not(any(windows, target_os = "linux")))]
    0
}

struct Temp {
    last: i32,
    at: Option<Instant>,
    #[cfg(target_os = "linux")]
    file: Option<std::path::PathBuf>,
    #[cfg(windows)]
    view: Option<crate::win::SharedView>,
    #[cfg(windows)]
    next_try: Option<Instant>,
}

static TEMP: Mutex<Temp> = Mutex::new(Temp {
    last: -1,
    at: None,
    #[cfg(target_os = "linux")]
    file: None,
    #[cfg(windows)]
    view: None,
    #[cfg(windows)]
    next_try: None,
});

/// CPU temperature in °C, or -1 when unknown (refreshed at most every 1.5 s).
pub fn cpu_temp_c() -> i32 {
    let mut t = TEMP.lock().unwrap();
    if t.at.is_some_and(|a| a.elapsed() < Duration::from_millis(1500)) {
        return t.last;
    }
    t.at = Some(Instant::now());
    t.last = read_temp(&mut t);
    t.last
}

#[cfg(target_os = "linux")]
fn read_temp(t: &mut Temp) -> i32 {
    if t.file.is_none() {
        if let Ok(dir) = std::fs::read_dir("/sys/class/hwmon") {
            for d in dir.flatten() {
                let name = std::fs::read_to_string(d.path().join("name")).unwrap_or_default();
                if !matches!(name.trim(), "k10temp" | "zenpower" | "coretemp" | "cpu_thermal") {
                    continue;
                }
                let f = d.path().join("temp1_input");
                if f.exists() {
                    t.file = Some(f);
                    break;
                }
            }
        }
    }
    let Some(f) = &t.file else { return -1 };
    match std::fs::read_to_string(f).ok().and_then(|s| s.trim().parse::<i64>().ok()) {
        Some(milli) => {
            let c = (milli as f64 / 1000.0).round() as i32;
            if c > 0 && c < 150 { c } else { -1 }
        }
        None => {
            t.file = None;
            -1
        }
    }
}

#[cfg(windows)]
fn read_temp(t: &mut Temp) -> i32 {
    if t.view.is_none() {
        if t.next_try.is_some_and(|n| Instant::now() < n) {
            return -1;
        }
        t.next_try = Some(Instant::now() + Duration::from_secs(2));
        t.view = ["Global\\TTLcdSensors", "Local\\TTLcdSensors"].iter().find_map(|n| crate::win::SharedView::open(n, MAP_SIZE, false));
    }
    let Some(v) = &t.view else { return -1 };
    if v.read_i32(0) as u32 != MAP_MAGIC {
        return -1;
    }
    let ticks = v.read_i64(16);
    let iv = v.read_i32(INTERVAL_OFFSET);
    let max_age = (3.0 * if iv > 0 { iv } else { 1500 } as f64 / 1000.0).max(10.0);
    if (crate::util::dotnet_ticks_now() - ticks) as f64 / 1e7 > max_age {
        return -1; // stale: helper stopped
    }
    let c = f32::from_bits(v.read_i32(4) as u32);
    if c > 0.0 && c < 150.0 { c.round_ties_even() as i32 } else { -1 }
}

#[cfg(not(any(windows, target_os = "linux")))]
fn read_temp(_: &mut Temp) -> i32 {
    -1
}

/// Windows: the helper's last update time (.NET ticks), 0 when unknown (for the dashboard's "every n s" note).
pub fn helper_updated_ticks() -> i64 {
    #[cfg(windows)]
    {
        let t = TEMP.lock().unwrap();
        if let Some(v) = &t.view {
            if v.read_i32(0) as u32 == MAP_MAGIC {
                return v.read_i64(16);
            }
        }
    }
    0
}

/// Windows: ask the helper to poll every `ms` (slower while a game runs; 0 = its default). The map grants Everyone
/// read/write.
pub fn request_interval(ms: i32) {
    #[cfg(windows)]
    for n in ["Global\\TTLcdSensors", "Local\\TTLcdSensors"] {
        if let Some(v) = crate::win::SharedView::open(n, MAP_SIZE, true) {
            v.write_i32(INTERVAL_OFFSET, ms);
            return;
        }
    }
    #[cfg(not(windows))]
    let _ = ms;
}

// ---------------- motherboard fans (the Pump mode's speed) ----------------

/// A fan the system reports. `id` is "<chip>/fan<N>" (stable across boots, unlike hwmonN), `label` what to show.
#[derive(Clone, Debug, PartialEq)]
pub struct Fan {
    pub id: String,
    pub label: String,
    pub rpm: u32,
}

/// hwmon drivers whose fans are not the motherboard's (graphics cards).
const NOT_BOARD: [&str; 5] = ["amdgpu", "radeon", "nouveau", "i915", "xe"];

/// The motherboard's fans with their speed now. Linux: hwmon (e.g. nct67xx Super I/O, asus-ec-sensors). Windows:
/// none yet (no fan source without a driver).
pub fn fans() -> Vec<Fan> {
    #[cfg(target_os = "linux")]
    return fans_in(std::path::Path::new("/sys/class/hwmon"));
    #[cfg(not(target_os = "linux"))]
    Vec::new()
}

#[cfg_attr(not(target_os = "linux"), allow(dead_code))]
fn fans_in(root: &std::path::Path) -> Vec<Fan> {
    let read = |p: std::path::PathBuf| std::fs::read_to_string(p).ok().map(|s| s.trim().to_string());
    let mut out = vec![];
    let Ok(dir) = std::fs::read_dir(root) else { return out };
    let mut chips: Vec<_> = dir.flatten().map(|e| e.path()).collect();
    chips.sort();
    for chip in chips {
        let Some(name) = read(chip.join("name")) else { continue };
        if NOT_BOARD.contains(&name.as_str()) {
            continue;
        }
        let mut n: Vec<u32> = std::fs::read_dir(&chip)
            .into_iter()
            .flatten()
            .flatten()
            .filter_map(|e| e.file_name().to_str()?.strip_prefix("fan")?.strip_suffix("_input")?.parse().ok())
            .collect();
        n.sort();
        for i in n {
            let Some(rpm) = read(chip.join(format!("fan{i}_input"))).and_then(|v| v.parse().ok()) else { continue };
            let label = read(chip.join(format!("fan{i}_label"))).filter(|l| !l.is_empty()).unwrap_or(format!("fan{i}"));
            out.push(Fan { id: format!("{name}/fan{i}"), label: format!("{name} {label}"), rpm });
        }
    }
    out
}

/// The CPU fan: one labelled "CPU" (boards whose driver names its headers), else fan2 on a Nuvoton nct67xx Super I/O
/// (the driver's fan2 is the CPUFANIN pin), else the first one that spins.
pub fn cpu_fan(fans: &[Fan]) -> Option<&Fan> {
    fans.iter()
        .find(|f| {
            let l = f.label.to_lowercase();
            l.contains("cpu") && !l.contains("opt")
        })
        .or_else(|| fans.iter().find(|f| f.id.starts_with("nct6") && f.id.ends_with("/fan2")))
        .or_else(|| fans.iter().find(|f| f.rpm > 0))
}

/// The Pump mode's speed source for its setting: "" = the CPU fan, "manual" = none (the set speed), else a fan id
/// (the CPU fan again when that fan is gone).
pub fn pump_source<'a>(choice: &str, fans: &'a [Fan]) -> Option<&'a Fan> {
    match choice {
        "manual" => None,
        "" => cpu_fan(fans),
        id => fans.iter().find(|f| f.id == id).or_else(|| cpu_fan(fans)),
    }
}

#[cfg(test)]
mod fan_tests {
    use super::*;

    /// A fake /sys/class/hwmon: chips by name, each with (index, rpm, label).
    fn sysfs(chips: &[(&str, &[(u32, u32, Option<&str>)])]) -> std::path::PathBuf {
        let root = std::env::temp_dir().join(format!("ttlcd-fans-{}-{}", std::process::id(), chips.len() * 7 + chips[0].1.len()));
        let _ = std::fs::remove_dir_all(&root);
        for (k, (name, fans)) in chips.iter().enumerate() {
            let d = root.join(format!("hwmon{k}"));
            std::fs::create_dir_all(&d).unwrap();
            std::fs::write(d.join("name"), format!("{name}\n")).unwrap();
            for (i, rpm, label) in fans.iter() {
                std::fs::write(d.join(format!("fan{i}_input")), format!("{rpm}\n")).unwrap();
                if let Some(l) = label {
                    std::fs::write(d.join(format!("fan{i}_label")), format!("{l}\n")).unwrap();
                }
            }
        }
        root
    }

    /// 2026-10-15: the Pump mode reads motherboard fans; without a choice it uses the CPU fan.
    #[test]
    fn motherboard_fans_and_the_cpu_fan() {
        // this machine's layout: nct6799 fan1/fan2/fan7, plus a graphics card's fan that isn't the board's
        let root = sysfs(&[("nct6799", &[(1, 867, None), (2, 1144, None), (7, 3125, None)]), ("amdgpu", &[(1, 0, None)])]);
        let f = fans_in(&root);
        assert_eq!(f.iter().map(|f| f.id.as_str()).collect::<Vec<_>>(), ["nct6799/fan1", "nct6799/fan2", "nct6799/fan7"]);
        assert_eq!(f[2], Fan { id: "nct6799/fan7".into(), label: "nct6799 fan7".into(), rpm: 3125 });
        assert_eq!(cpu_fan(&f).unwrap().id, "nct6799/fan2", "nct67xx: fan2 is CPUFANIN");
        assert_eq!(pump_source("", &f).unwrap().id, "nct6799/fan2", "no choice: the CPU fan");
        assert_eq!(pump_source("nct6799/fan7", &f).unwrap().rpm, 3125, "the chosen fan");
        assert_eq!(pump_source("nct6799/fan9", &f).unwrap().id, "nct6799/fan2", "chosen fan gone: the CPU fan");
        assert!(pump_source("manual", &f).is_none(), "manual: the set speed");
        let _ = std::fs::remove_dir_all(root);

        // labelled headers (asus-ec-sensors and the like): the one called CPU, not CPU_OPT
        let root = sysfs(&[("asusec", &[(1, 1500, Some("CPU Optional")), (2, 2900, Some("Water Pump")), (3, 1100, Some("CPU"))])]);
        let f = fans_in(&root);
        assert_eq!(cpu_fan(&f).unwrap().label, "asusec CPU");
        let _ = std::fs::remove_dir_all(root);

        // nothing to go by: the first fan that spins; no fans: nothing
        let root = sysfs(&[("it87", &[(1, 0, None), (3, 900, None)])]);
        assert_eq!(cpu_fan(&fans_in(&root)).unwrap().id, "it87/fan3");
        let _ = std::fs::remove_dir_all(root);
        assert!(cpu_fan(&[]).is_none());
    }
}
