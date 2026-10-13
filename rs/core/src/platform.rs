//! What differs between Windows and Linux under the same Studio: autostart, the CPU temperature source, game
//! detection, opening folders, running the command-line tool, and the Linux `ttlcd` user service.
use std::path::PathBuf;
use std::process::{Command, Stdio};

/// The command-line tool next to the running executable.
pub fn cli_path() -> PathBuf {
    let exe = std::env::current_exe().unwrap_or_default();
    let dir = exe.parent().map(|p| p.to_path_buf()).unwrap_or_default();
    #[cfg(windows)]
    {
        dir.join("ttlcd.exe")
    }
    #[cfg(not(windows))]
    {
        let local = dir.join("ttlcd");
        if local.exists() { local } else { PathBuf::from("ttlcd") }
    }
}

/// Runs `ttlcd <args>` (on Windows it elevates itself when needed) on a worker thread; done(output).
pub fn run_cli(args: &[&str], done: impl FnOnce(String) + Send + 'static) {
    let args: Vec<String> = args.iter().map(|s| s.to_string()).collect();
    std::thread::spawn(move || {
        let mut cmd = Command::new(cli_path());
        cmd.args(&args).stdin(Stdio::null());
        #[cfg(windows)]
        {
            use std::os::windows::process::CommandExt;
            cmd.creation_flags(0x0800_0000); // CREATE_NO_WINDOW
        }
        let out = match cmd.output() {
            Ok(o) => format!("{}{}", String::from_utf8_lossy(&o.stdout), String::from_utf8_lossy(&o.stderr)),
            Err(e) => format!("error: {e}"),
        };
        done(out);
    });
}

pub fn open_folder(path: &std::path::Path) {
    #[cfg(windows)]
    let _ = Command::new("explorer.exe").arg(path).spawn();
    #[cfg(not(windows))]
    let _ = Command::new("xdg-open").arg(path).stdin(Stdio::null()).stdout(Stdio::null()).stderr(Stdio::null()).spawn();
}

/// Start with Windows (HKCU Run "TTLcd") / with the desktop session (~/.config/autostart), in the tray.
pub fn autostart() -> bool {
    #[cfg(windows)]
    {
        crate::win::reg::get_string(crate::win::reg::HKCU, RUN_KEY, "TTLcd").is_some()
    }
    #[cfg(not(windows))]
    {
        autostart_file().exists()
    }
}

#[cfg(windows)]
pub const RUN_KEY: &str = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

pub fn set_autostart(on: bool) {
    #[cfg(windows)]
    {
        use crate::win::reg;
        if on {
            let exe = std::env::current_exe().unwrap_or_default();
            reg::set_string(reg::HKCU, RUN_KEY, "TTLcd", &format!("\"{}\" --tray", exe.display()));
        } else {
            reg::delete_value(reg::HKCU, RUN_KEY, "TTLcd");
        }
    }
    #[cfg(not(windows))]
    {
        let f = autostart_file();
        if !on {
            let _ = std::fs::remove_file(&f);
            return;
        }
        if let Some(d) = f.parent() {
            let _ = std::fs::create_dir_all(d);
        }
        let _ = std::fs::write(
            &f,
            "[Desktop Entry]\nType=Application\nName=TT LCD Studio\nComment=Cooler screen (in the tray)\nExec=ttlcd-studio --tray\nIcon=ttlcd-studio\nX-GNOME-Autostart-enabled=true\n",
        );
    }
}

#[cfg(not(windows))]
fn autostart_file() -> PathBuf {
    let cfg = std::env::var_os("XDG_CONFIG_HOME")
        .filter(|v| !v.is_empty())
        .map(PathBuf::from)
        .unwrap_or_else(|| PathBuf::from(std::env::var_os("HOME").unwrap_or_default()).join(".config"));
    cfg.join("autostart/ttlcd-studio.desktop")
}

/// Automatic game detection is available (Windows).
pub fn game_detection() -> bool {
    cfg!(windows)
}

/// CPU temperature needs a one-time install (dashboard hint): the Windows sensor service.
pub fn cpu_temp_needs_setup() -> bool {
    #[cfg(windows)]
    {
        !crate::win::sensors::service_installed()
    }
    #[cfg(not(windows))]
    false
}

/// Settings can install/uninstall the CPU temperature source.
pub fn sensors_managed() -> bool {
    cfg!(windows)
}

pub fn sensor_installed() -> bool {
    #[cfg(windows)]
    {
        crate::win::sensors::service_installed()
    }
    #[cfg(not(windows))]
    {
        crate::sensors::cpu_temp_c() > 0
    }
}

pub fn sensor_status() -> String {
    #[cfg(windows)]
    {
        let inst = crate::win::sensors::service_installed();
        let run = inst && crate::win::sensors::service_running();
        if !inst { "○ Service not installed" } else if run { "● Service running" } else { "● Installed · not running" }.into()
    }
    #[cfg(not(windows))]
    {
        if crate::sensors::cpu_temp_c() > 0 {
            "● Read from the kernel's CPU sensor driver".into()
        } else {
            "○ No CPU sensor driver found (k10temp, zenpower or coretemp)".into()
        }
    }
}

/// Linux: the `ttlcd run` user service drives the cooler too; Studio takes over while it runs.
pub fn service_active() -> bool {
    #[cfg(target_os = "linux")]
    {
        Command::new("systemctl")
            .args(["--user", "is-active", "ttlcd"])
            .stderr(Stdio::null())
            .output()
            .map(|o| String::from_utf8_lossy(&o.stdout).trim() == "active")
            .unwrap_or(false)
    }
    #[cfg(not(target_os = "linux"))]
    false
}

pub fn stop_service() {
    #[cfg(target_os = "linux")]
    let _ = Command::new("systemctl").args(["--user", "stop", "ttlcd"]).stderr(Stdio::null()).status();
}

pub fn start_service() {
    #[cfg(target_os = "linux")]
    let _ = Command::new("systemctl").args(["--user", "start", "ttlcd"]).stderr(Stdio::null()).status();
}
