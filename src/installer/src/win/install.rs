//! Install and uninstall (behaviour of the 1.x installer/Setup.cs).
use super::{ask, delete_key_tree, inform, known_folder, wide, Key, APP_NAME, VERSION};
use std::fs;
use std::io::{self, Seek};
use std::os::windows::process::CommandExt;
use std::path::{Path, PathBuf};
use std::process::{Command, Stdio};
use std::time::Duration;
use ttlcd_setup::pack;
use windows_sys::Win32::Foundation::{CloseHandle, HWND, LPARAM, WAIT_OBJECT_0};
use windows_sys::Win32::System::Diagnostics::ToolHelp::{
    CreateToolhelp32Snapshot, Process32FirstW, Process32NextW, PROCESSENTRY32W, TH32CS_SNAPPROCESS,
};
use windows_sys::Win32::System::Registry::{HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY};
use windows_sys::Win32::System::Threading::{
    OpenProcess, QueryFullProcessImageNameW, TerminateProcess, WaitForSingleObject, PROCESS_NAME_WIN32,
    PROCESS_QUERY_LIMITED_INFORMATION, PROCESS_SYNCHRONIZE, PROCESS_TERMINATE,
};
use windows_sys::Win32::UI::Shell::{FOLDERID_Desktop, FOLDERID_LocalAppData, FOLDERID_Programs};
use windows_sys::Win32::UI::WindowsAndMessaging::{
    EnumWindows, GetWindowThreadProcessId, IsWindowVisible, PostMessageW, WM_CLOSE,
};

const PUBLISHER: &str = "TT LCD Studio";
const UNINSTALL_KEY: &str = r"Software\Microsoft\Windows\CurrentVersion\Uninstall\TTLcdStudio";
const RUN_KEY: &str = r"Software\Microsoft\Windows\CurrentVersion\Run";
const RUN_NAME: &str = "TTLcd"; // same value the app writes for "Start with Windows"
const STUDIO_EXE: &str = "TTLcdStudio.exe";
const CREATE_NO_WINDOW: u32 = 0x0800_0000;

#[derive(Clone, Default)]
pub struct Options {
    pub dir: PathBuf,
    pub start_menu: bool,
    pub desktop: bool,
    pub autostart: bool,
    pub launch: bool,
    pub sensor_service: bool,
    pub aura: bool,
    pub virtual_display: bool,
}

pub fn default_dir() -> PathBuf {
    let base = known_folder(&FOLDERID_LocalAppData)
        .or_else(|| std::env::var_os("LOCALAPPDATA").map(PathBuf::from))
        .unwrap_or_else(|| PathBuf::from(r"C:\"));
    base.join("Programs").join(APP_NAME)
}

fn start_menu_lnk() -> Option<PathBuf> {
    known_folder(&FOLDERID_Programs).map(|p| p.join(format!("{APP_NAME}.lnk")))
}

fn desktop_lnk() -> Option<PathBuf> {
    known_folder(&FOLDERID_Desktop).map(|p| p.join(format!("{APP_NAME}.lnk")))
}

fn lower(p: &Path) -> String {
    p.to_string_lossy().to_lowercase()
}

fn same_path(a: &Path, b: &Path) -> bool {
    lower(a) == lower(b)
}

/// The Run value points into this install folder.
fn owned_by_us(run_value: Option<&str>, dir: &Path) -> bool {
    run_value.is_some_and(|v| v.to_lowercase().contains(&lower(dir)))
}

fn io_err(what: &str, p: &Path, e: io::Error) -> String {
    format!("{what} {}: {e}", p.display())
}

// ---------------- install ----------------

pub fn install(o: &Options, status: &dyn Fn(&str)) -> Result<(), String> {
    let dir = std::path::absolute(&o.dir).map_err(|e| io_err("bad folder", &o.dir, e))?;
    fs::create_dir_all(&dir).map_err(|e| io_err("cannot create", &dir, e))?;
    status("Closing a running TT LCD Studio…");
    stop_app(&dir);
    remove_old_files(&dir);

    let self_exe = std::env::current_exe().map_err(|e| format!("cannot find the setup's own file: {e}"))?;
    let mut f = fs::File::open(&self_exe).map_err(|e| io_err("cannot read", &self_exe, e))?;
    let payload = pack::read_payload(&mut f)
        .map_err(|e| e.to_string())?
        .filter(|p| !p.entries.is_empty())
        .ok_or("this setup contains no files (broken build)")?;
    for e in &payload.entries {
        let rel: PathBuf = e.path.split('/').collect();
        let target = dir.join(&rel);
        status(&format!("Copying {}", rel.display()));
        if let Some(parent) = target.parent() {
            fs::create_dir_all(parent).map_err(|err| io_err("cannot create", parent, err))?;
        }
        let data = pack::extract(&mut f, &payload, e).map_err(|err| err.to_string())?;
        write_file(&target, &data)?;
    }

    // the uninstaller: the bare setup stub (or the whole file when it is signed: cutting it would break that)
    let uninst = dir.join("Uninstall.exe");
    if !same_path(&self_exe, &uninst) {
        let len = f.seek(io::SeekFrom::End(0)).map_err(|e| e.to_string())?;
        let signed = pack::logical_end(&mut f).map_err(|e| e.to_string())? < len;
        let all = fs::read(&self_exe).map_err(|e| io_err("cannot read", &self_exe, e))?;
        let stub = if signed { &all[..] } else { &all[..payload.start as usize] };
        write_file(&uninst, stub)?;
    }
    drop(f);

    let studio = dir.join(STUDIO_EXE);
    status("Creating shortcuts…");
    for (want, lnk) in [(o.start_menu, start_menu_lnk()), (o.desktop, desktop_lnk())] {
        let Some(lnk) = lnk else { continue };
        if want {
            shortcut(&lnk, &studio, "", &dir).map_err(|e| format!("cannot create the shortcut {}: {e}", lnk.display()))?;
        } else {
            try_delete(&lnk);
        }
    }

    let run = Key::create(HKEY_CURRENT_USER, RUN_KEY)?;
    if o.autostart {
        run.set_str(RUN_NAME, &format!("\"{}\" --tray", studio.display()))?;
    } else if owned_by_us(run.get_str(RUN_NAME).as_deref(), &dir) {
        run.delete_value(RUN_NAME);
    }
    drop(run);

    let k = Key::create(HKEY_CURRENT_USER, UNINSTALL_KEY)?;
    k.set_str("DisplayName", APP_NAME)?;
    k.set_str("DisplayVersion", VERSION)?;
    k.set_str("Publisher", PUBLISHER)?;
    k.set_str("DisplayIcon", &studio.to_string_lossy())?;
    k.set_str("InstallLocation", &dir.to_string_lossy())?;
    k.set_str("UninstallString", &format!("\"{}\" /uninstall", uninst.display()))?;
    k.set_str("QuietUninstallString", &format!("\"{}\" /uninstall /S", uninst.display()))?;
    k.set_dword("NoModify", 1)?;
    k.set_dword("NoRepair", 1)?;
    k.set_dword("EstimatedSize", (dir_size(&dir) / 1024).min(u32::MAX as u64) as u32)?;
    drop(k);

    // optional parts that need administrator rights: ttlcd.exe elevates itself (UAC prompt)
    let cli = dir.join("ttlcd.exe");
    if o.sensor_service {
        status("Installing the CPU temperature service (confirm the UAC prompt)…");
        run_cli(&cli, &["service", "install"]);
    }
    if o.aura {
        status("Installing the Aura Sync plugin (confirm the UAC prompt)…");
        run_cli(&cli, &["aura", "install"]);
    }
    if o.virtual_display {
        status("Installing the virtual display (confirm the UAC prompt)…");
        run_cli(&cli, &["display", "install"]);
    }

    if o.launch {
        status("Starting TT LCD Studio…");
        Command::new(&studio).current_dir(&dir).spawn().map_err(|e| io_err("cannot start", &studio, e))?;
    }
    status(&format!("Installed to {}", dir.display()));
    Ok(())
}

/// Writes a file; if it is in use (a running exe), moves it aside first: Windows allows renaming a running exe.
fn write_file(target: &Path, data: &[u8]) -> Result<(), String> {
    if fs::write(target, data).is_ok() {
        return Ok(());
    }
    let ticks = std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).map_or(0, |d| d.as_nanos());
    let mut aside = target.as_os_str().to_owned();
    aside.push(format!(".{ticks}.old"));
    fs::rename(target, &aside).map_err(|e| io_err("cannot replace", target, e))?;
    fs::write(target, data).map_err(|e| io_err("cannot write", target, e))
}

/// Files moved aside by an earlier install ("<name>.<ticks>.old") whose process has exited by now.
fn remove_old_files(dir: &Path) {
    fn walk(d: &Path, depth: u32) {
        let Ok(rd) = fs::read_dir(d) else { return };
        for e in rd.flatten() {
            let p = e.path();
            if p.is_dir() {
                if depth < 4 {
                    walk(&p, depth + 1);
                }
                continue;
            }
            let name = e.file_name().to_string_lossy().into_owned();
            if let Some(stem) = name.strip_suffix(".old") {
                if stem.rsplit_once('.').is_some_and(|(_, t)| !t.is_empty() && t.bytes().all(|b| b.is_ascii_digit())) {
                    let _ = fs::remove_file(&p);
                }
            }
        }
    }
    walk(dir, 0);
}

fn dir_size(d: &Path) -> u64 {
    let Ok(rd) = fs::read_dir(d) else { return 0 };
    rd.flatten()
        .map(|e| match e.metadata() {
            Ok(m) if m.is_dir() => dir_size(&e.path()),
            Ok(m) => m.len(),
            Err(_) => 0,
        })
        .sum()
}

pub fn try_delete(f: &Path) {
    if f.is_file() {
        let _ = fs::remove_file(f);
    }
}

/// Runs ttlcd.exe without a window and waits; the exit code, or -1.
pub fn run_cli(cli: &Path, args: &[&str]) -> i32 {
    if !cli.is_file() {
        return -1;
    }
    Command::new(cli)
        .args(args)
        .creation_flags(CREATE_NO_WINDOW)
        .stdin(Stdio::null())
        .output()
        .map_or(-1, |o| o.status.code().unwrap_or(-1))
}

/// Closes TT LCD Studio processes that run from `dir`: WM_CLOSE to their visible windows, then (after 3 s, or at
/// once when they only sit in the tray) terminates them.
pub fn stop_app(dir: &Path) {
    let prefix = format!("{}\\", lower(dir).trim_end_matches('\\'));
    for pid in processes_named(STUDIO_EXE) {
        unsafe {
            let h = OpenProcess(
                PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | PROCESS_SYNCHRONIZE,
                0,
                pid,
            );
            if h.is_null() {
                continue;
            }
            let mut buf = [0u16; 1024];
            let mut n = buf.len() as u32;
            let path = if QueryFullProcessImageNameW(h, PROCESS_NAME_WIN32, buf.as_mut_ptr(), &mut n) != 0 {
                String::from_utf16_lossy(&buf[..n as usize]).to_lowercase()
            } else {
                String::new()
            };
            if path.starts_with(&prefix) {
                let exited = close_windows_of(pid) && WaitForSingleObject(h, 3000) == WAIT_OBJECT_0;
                if !exited {
                    TerminateProcess(h, 1);
                    WaitForSingleObject(h, 3000);
                }
            }
            CloseHandle(h);
        }
    }
}

fn processes_named(exe: &str) -> Vec<u32> {
    let mut out = Vec::new();
    unsafe {
        let snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if snap.is_null() || snap as isize == -1 {
            return out;
        }
        let mut e: PROCESSENTRY32W = std::mem::zeroed();
        e.dwSize = std::mem::size_of::<PROCESSENTRY32W>() as u32;
        let mut ok = Process32FirstW(snap, &mut e) != 0;
        while ok {
            let n = e.szExeFile.iter().position(|&c| c == 0).unwrap_or(e.szExeFile.len());
            if String::from_utf16_lossy(&e.szExeFile[..n]).eq_ignore_ascii_case(exe) {
                out.push(e.th32ProcessID);
            }
            ok = Process32NextW(snap, &mut e) != 0;
        }
        CloseHandle(snap);
    }
    out
}

/// Posts WM_CLOSE to the process's visible top-level windows; false when it has none.
fn close_windows_of(pid: u32) -> bool {
    struct Find {
        pid: u32,
        found: bool,
    }
    unsafe extern "system" fn each(hwnd: HWND, lp: LPARAM) -> windows_sys::core::BOOL {
        let f = &mut *(lp as *mut Find);
        let mut p = 0u32;
        GetWindowThreadProcessId(hwnd, &mut p);
        if p == f.pid && IsWindowVisible(hwnd) != 0 {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            f.found = true;
        }
        1
    }
    let mut f = Find { pid, found: false };
    unsafe { EnumWindows(Some(each), &mut f as *mut Find as LPARAM) };
    f.found
}

/// A .lnk through IShellLinkW + IPersistFile (COM must be initialised on this thread).
fn shortcut(lnk: &Path, target: &Path, args: &str, work_dir: &Path) -> windows::core::Result<()> {
    use windows::core::{Interface, PCWSTR};
    use windows::Win32::System::Com::{CoCreateInstance, IPersistFile, CLSCTX_INPROC_SERVER};
    use windows::Win32::UI::Shell::{IShellLinkW, ShellLink};
    let w = |s: &dyn AsRef<std::ffi::OsStr>| wide(s);
    unsafe {
        let link: IShellLinkW = CoCreateInstance(&ShellLink, None, CLSCTX_INPROC_SERVER)?;
        let t = w(&target);
        link.SetPath(PCWSTR(t.as_ptr()))?;
        let a = w(&args);
        link.SetArguments(PCWSTR(a.as_ptr()))?;
        let d = w(&work_dir);
        link.SetWorkingDirectory(PCWSTR(d.as_ptr()))?;
        link.SetIconLocation(PCWSTR(t.as_ptr()), 0)?;
        let desc = w(&"Thermaltake cooler LCD control");
        link.SetDescription(PCWSTR(desc.as_ptr()))?;
        let file: IPersistFile = link.cast()?;
        let l = w(&lnk);
        file.Save(PCWSTR(l.as_ptr()), true)
    }
}

/// COM for the calling thread (shortcuts, the folder picker); apartment-threaded like the C# [STAThread].
pub fn com_init() {
    use windows::Win32::System::Com::{CoInitializeEx, COINIT_APARTMENTTHREADED, COINIT_DISABLE_OLE1DDE};
    unsafe {
        let _ = CoInitializeEx(None, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    }
}

// ---------------- uninstall ----------------

/// Started from the install folder: copy ourselves to %TEMP% and run from there, so the folder can be deleted.
pub fn uninstall_start(silent: bool, all: bool) -> Result<i32, String> {
    let self_exe = std::env::current_exe().map_err(|e| e.to_string())?;
    let dir = self_exe.parent().ok_or("no install folder")?.to_path_buf();
    let nonce = std::time::SystemTime::now().duration_since(std::time::UNIX_EPOCH).map_or(0, |d| d.as_nanos() as u64)
        ^ ((std::process::id() as u64) << 32);
    let tmp = std::env::temp_dir().join(format!("TTLcdStudio-uninstall-{:08x}.exe", nonce as u32));
    fs::copy(&self_exe, &tmp).map_err(|e| io_err("cannot copy the uninstaller to", &tmp, e))?;
    let mut c = Command::new(&tmp);
    c.arg("/uninstall-run").arg(&dir);
    if silent {
        c.arg("/S");
    }
    if all {
        c.arg("/all");
    }
    c.spawn().map_err(|e| io_err("cannot start", &tmp, e))?;
    Ok(0)
}

pub fn uninstall_run(dir: Option<&str>, silent: bool, remove_shared: bool) -> Result<i32, String> {
    let dir = match dir {
        Some(d) if !d.is_empty() && Path::new(d).join(STUDIO_EXE).is_file() => PathBuf::from(d),
        d => return Err(format!("TT LCD Studio was not found in {}", d.unwrap_or(""))),
    };
    if !silent && !ask("Remove TT LCD Studio from this PC?\n\nYour settings in %APPDATA%\\ttlcd are kept.") {
        self_delete();
        return Ok(1);
    }

    stop_app(&dir);
    // admin parts (each asks via UAC). The sensor service is removed only if it runs from this folder; the Aura
    // plugin and the virtual display are shared system-wide (another copy may use them), so only on request.
    let cli = dir.join("ttlcd.exe");
    if service_owned_by(&dir) {
        run_cli(&cli, &["service", "uninstall"]);
    }
    let aura = aura_installed();
    let display = display_installed(&cli);
    if (aura || display)
        && (remove_shared
            || (!silent
                && ask(&format!(
                    "Also remove {}?\n(Needs administrator rights.)",
                    match (aura, display) {
                        (true, true) => "the Aura Sync plugin and the virtual display driver",
                        (true, false) => "the Aura Sync plugin",
                        _ => "the virtual display driver",
                    }
                ))))
    {
        if aura {
            run_cli(&cli, &["aura", "uninstall"]);
        }
        if display {
            run_cli(&cli, &["display", "uninstall"]);
        }
    }

    if let Ok(run) = Key::create(HKEY_CURRENT_USER, RUN_KEY) {
        if owned_by_us(run.get_str(RUN_NAME).as_deref(), &dir) {
            run.delete_value(RUN_NAME);
        }
    }
    for lnk in [start_menu_lnk(), desktop_lnk()].into_iter().flatten() {
        try_delete(&lnk);
    }
    delete_key_tree(HKEY_CURRENT_USER, UNINSTALL_KEY);

    for _ in 0..10 {
        if !dir.exists() || fs::remove_dir_all(&dir).is_ok() {
            break;
        }
        std::thread::sleep(Duration::from_millis(500));
    }
    if !silent {
        inform(&if dir.exists() {
            format!(
                "TT LCD Studio was removed. Some files were in use and stay in\n{}\nuntil the next restart.",
                dir.display()
            )
        } else {
            "TT LCD Studio was removed.".to_string()
        });
    }
    self_delete();
    Ok(0)
}

fn service_owned_by(dir: &Path) -> bool {
    Key::open(HKEY_LOCAL_MACHINE, r"SYSTEM\CurrentControlSet\Services\TTLcdSensors", 0)
        .and_then(|k| k.get_str("ImagePath"))
        .is_some_and(|p| p.to_lowercase().contains(&lower(dir)))
}

fn aura_installed() -> bool {
    Key::open(HKEY_LOCAL_MACHINE, r"SOFTWARE\Classes\CLSID\{C0C96C60-30E3-4021-9904-966EC4FA0F0F}", KEY_WOW64_32KEY)
        .is_some()
}

fn display_installed(cli: &Path) -> bool {
    if !cli.is_file() {
        return false;
    }
    Command::new(cli)
        .args(["display", "status"])
        .creation_flags(CREATE_NO_WINDOW)
        .stdin(Stdio::null())
        .stderr(Stdio::null())
        .output()
        .map(|o| {
            let s = String::from_utf8_lossy(&o.stdout);
            s.contains("driver package:") && !s.contains("driver package: not installed")
        })
        .unwrap_or(false)
}

/// The temp copy deletes itself once it has exited.
fn self_delete() {
    let Ok(self_exe) = std::env::current_exe() else { return };
    if !lower(&self_exe).starts_with(&lower(&std::env::temp_dir())) {
        return;
    }
    let _ = Command::new("cmd.exe")
        .raw_arg(format!("/c ping -n 3 127.0.0.1 > nul & del /f /q \"{}\"", self_exe.display()))
        .creation_flags(CREATE_NO_WINDOW)
        .stdin(Stdio::null())
        .stdout(Stdio::null())
        .stderr(Stdio::null())
        .spawn();
}
