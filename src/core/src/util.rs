//! Small helpers shared by the modules.
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};

/// .NET DateTime ticks (100 ns since 0001-01-01 UTC): the timestamps in the Aura and sensor shared blocks.
pub fn dotnet_ticks_now() -> i64 {
    let d = SystemTime::now().duration_since(UNIX_EPOCH).unwrap_or_default();
    621_355_968_000_000_000 + (d.as_nanos() / 100) as i64
}

/// Milliseconds since the process started (monotonic).
pub fn now_ms() -> i64 {
    static START: std::sync::OnceLock<Instant> = std::sync::OnceLock::new();
    START.get_or_init(Instant::now).elapsed().as_millis() as i64
}

/// Sleeps in small steps until `deadline` or `stop()` returns true.
pub fn sleep_until(deadline: Instant, stop: impl Fn() -> bool) {
    while !stop() {
        let now = Instant::now();
        if now >= deadline {
            return;
        }
        std::thread::sleep((deadline - now).min(Duration::from_millis(50)));
    }
}

/// "eldenring.exe, Cyberpunk2077" -> {"eldenring", "cyberpunk2077"}
pub fn parse_names(list: &str) -> Vec<String> {
    let mut r: Vec<String> = Vec::new();
    for part in list.split([',', ';', '\n', '\r']) {
        let mut n = part.trim().to_lowercase();
        if n.ends_with(".exe") {
            n.truncate(n.len() - 4);
        }
        if !n.is_empty() && !r.contains(&n) {
            r.push(n);
        }
    }
    r
}

/// Modification time of a file as a cache key part (0 when it can't be read).
pub fn mtime_key(path: &str) -> u128 {
    std::fs::metadata(path)
        .and_then(|m| m.modified())
        .ok()
        .and_then(|t| t.duration_since(UNIX_EPOCH).ok())
        .map(|d| d.as_nanos())
        .unwrap_or(0)
}

/// FNV-1a, for cache file names.
pub fn fnv64(s: &str) -> u64 {
    let mut h: u64 = 0xcbf29ce484222325;
    for b in s.bytes() {
        h ^= b as u64;
        h = h.wrapping_mul(0x100000001b3);
    }
    h
}

/// "1.5 s" / "12 s"
pub fn secs(ms: i64) -> String {
    let s = ms as f64 / 1000.0;
    if s < 10.0 {
        let t = format!("{:.1}", s);
        format!("{} s", t.trim_end_matches(".0"))
    } else {
        format!("{} s", s.round())
    }
}
