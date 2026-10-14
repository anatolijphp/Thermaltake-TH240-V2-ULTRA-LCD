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
