//! CPU temperature helper (`ttlcd hwmon`) and the TTLcdSensors Windows service, port of Sensors.cs.
//! The temperature is read exactly as the vendor app (TOWEI) does: through its bundled HWiNFO-based XYDataCenter.dll
//! (MXNSDK_* API), which needs admin rights for the sensor driver. The helper (elevated console, or the service as
//! LocalSystem) loads that DLL, reads cpu_temperature (struct offset 0x188) and publishes it to "Global\TTLcdSensors"
//! (layout in crate::sensors), which Studio and `ttlcd run` read as a normal user. Signatures are used by the CLI,
//! Studio and platform.rs; keep them.
use super::{run_tool, wide};
use crate::sensors::{INTERVAL_OFFSET, MAP_MAGIC, MAP_SIZE};
use std::ffi::c_void;
use std::io;
use std::ptr::{null, null_mut};
use std::sync::atomic::{AtomicBool, AtomicPtr, Ordering};
use std::time::{Duration, Instant};
use windows_sys::Win32::Foundation::{CloseHandle, NO_ERROR};
use windows_sys::Win32::System::Services::*;

pub const SERVICE_NAME: &str = "TTLcdSensors";
const MAP_NAME: &str = "Global\\TTLcdSensors";
const MAP_NAME_LOCAL: &str = "Local\\TTLcdSensors";
/// Everyone: read/write (Studio writes the polling interval); SYSTEM and Administrators: full.
const MAP_SDDL: &str = "D:(A;;GRGW;;;WD)(A;;GA;;;SY)(A;;GA;;;BA)";
const DLL: &str = "XYDataCenter.dll";
const DLL_DIRS: [&str; 2] = ["C:\\Program Files\\T'OWEI\\plugin\\x64", "C:\\Program Files (x86)\\T'OWEI\\plugin\\x64"];
const CPU_TEMP_OFFSET: usize = 0x188; // cpu_temperature (f32) in MXNSDK_GetHWInfo's struct
const DEFAULT_INTERVAL_MS: i32 = 1500;

fn tf(b: bool) -> &'static str {
    if b { "True" } else { "False" }
}

// ---------------- helper (elevated): `ttlcd hwmon` ----------------

type InitFn = unsafe extern "C" fn() -> i32;
type ReadyFn = unsafe extern "C" fn() -> i32;
type GetFn = unsafe extern "C" fn(*mut c_void) -> i32;

/// `ttlcd hwmon`: publish until stop() returns true (the service stops it; the console helper runs until Ctrl+C).
pub fn run_helper(stop: &dyn Fn() -> bool) -> io::Result<u8> {
    use windows_sys::Win32::System::LibraryLoader::{GetProcAddress, LoadLibraryW, SetDllDirectoryW};
    let Some(dir) = DLL_DIRS.iter().find(|d| std::path::Path::new(d).join(DLL).is_file()) else {
        eprintln!("XYDataCenter.dll not found (TOWEI not installed)");
        return Ok(1);
    };
    let (init, ready, get) = unsafe {
        SetDllDirectoryW(wide(dir).as_ptr()); // its own dependencies sit next to it
        let h = LoadLibraryW(wide(&format!("{dir}\\{DLL}")).as_ptr());
        if h.is_null() {
            eprintln!("failed to load {DLL}");
            return Ok(1);
        }
        let f = |name: &std::ffi::CStr| GetProcAddress(h, name.as_ptr() as *const u8);
        match (f(c"MXNSDK_Init"), f(c"MXNSDK_IsReady"), f(c"MXNSDK_GetHWInfo")) {
            (Some(i), Some(r), Some(g)) => (std::mem::transmute::<_, InitFn>(i), std::mem::transmute::<_, ReadyFn>(r), std::mem::transmute::<_, GetFn>(g)),
            _ => {
                eprintln!("{DLL}: MXNSDK functions missing");
                return Ok(1);
            }
        }
    };
    unsafe { init() };
    for _ in 0..60 {
        if stop() || unsafe { ready() } != 0 {
            break;
        }
        std::thread::sleep(Duration::from_millis(500));
    }

    let (map, name) = match super::SharedView::create(MAP_NAME, MAP_SIZE, Some(MAP_SDDL)) {
        Some(v) => (v, MAP_NAME),
        None => match super::SharedView::create(MAP_NAME_LOCAL, MAP_SIZE, None) {
            Some(v) => (v, MAP_NAME_LOCAL),
            None => return Err(io::Error::other("cannot create the shared memory block for the temperature")),
        },
    };
    map.write_i32(0, MAP_MAGIC as i32);
    let mut buf = vec![0u64; 65536 / 8]; // the DLL's info struct (8-byte aligned)
    println!("ttlcd hwmon: publishing CPU temperature to {name}");
    while !stop() {
        if unsafe { get(buf.as_mut_ptr() as *mut c_void) } != 0 {
            let b = unsafe { std::slice::from_raw_parts(buf.as_ptr() as *const u8, 65536) };
            let cpu = f32::from_le_bytes(b[CPU_TEMP_OFFSET..CPU_TEMP_OFFSET + 4].try_into().unwrap());
            map.write_i32(4, cpu.to_bits() as i32);
            map.write_i64(16, crate::util::dotnet_ticks_now());
        }
        let iv = map.read_i32(INTERVAL_OFFSET); // Studio may ask for slower polling while a game runs
        let wait = if (500..=120_000).contains(&iv) { iv } else { DEFAULT_INTERVAL_MS };
        crate::util::sleep_until(Instant::now() + Duration::from_millis(wait as u64), stop);
    }
    Ok(0)
}

// ---------------- service control (also used for ASUS LightingService) ----------------

struct Sc(SC_HANDLE);

impl Drop for Sc {
    fn drop(&mut self) {
        if !self.0.is_null() {
            unsafe { CloseServiceHandle(self.0) };
        }
    }
}

fn open_service(name: &str, access: u32) -> io::Result<(Sc, Sc)> {
    unsafe {
        let m = Sc(OpenSCManagerW(null(), null(), SC_MANAGER_CONNECT));
        if m.0.is_null() {
            return Err(io::Error::last_os_error());
        }
        let s = Sc(OpenServiceW(m.0, wide(name).as_ptr(), access));
        if s.0.is_null() {
            return Err(io::Error::last_os_error());
        }
        Ok((m, s))
    }
}

/// The service's state (SERVICE_RUNNING, SERVICE_STOPPED, ...), None when it isn't installed (or can't be queried).
pub(crate) fn service_state(name: &str) -> Option<u32> {
    let (_m, s) = open_service(name, SERVICE_QUERY_STATUS).ok()?;
    let mut st: SERVICE_STATUS = unsafe { std::mem::zeroed() };
    (unsafe { QueryServiceStatus(s.0, &mut st) } != 0).then_some(st.dwCurrentState)
}

/// Starts (or stops) a service unless it already is, and waits up to `timeout` for it to get there.
pub(crate) fn start_stop_service(name: &str, start: bool, timeout: Duration) -> io::Result<()> {
    let (_m, s) = open_service(name, SERVICE_QUERY_STATUS | SERVICE_START | SERVICE_STOP)?;
    let want = if start { SERVICE_RUNNING } else { SERVICE_STOPPED };
    let query = || {
        let mut st: SERVICE_STATUS = unsafe { std::mem::zeroed() };
        if unsafe { QueryServiceStatus(s.0, &mut st) } == 0 {
            return Err(io::Error::last_os_error());
        }
        Ok(st.dwCurrentState)
    };
    let state = query()?;
    if state == want {
        return Ok(());
    }
    unsafe {
        if start {
            // a service still stopping can't be started yet: wait for it first
            let until = Instant::now() + timeout;
            while query()? == SERVICE_STOP_PENDING && Instant::now() < until {
                std::thread::sleep(Duration::from_millis(250));
            }
            if StartServiceW(s.0, 0, null()) == 0 {
                let e = io::Error::last_os_error();
                if e.raw_os_error() != Some(1056) {
                    return Err(e); // 1056 = already running
                }
            }
        } else if state != SERVICE_STOP_PENDING {
            let mut st: SERVICE_STATUS = std::mem::zeroed();
            if ControlService(s.0, SERVICE_CONTROL_STOP, &mut st) == 0 {
                let e = io::Error::last_os_error();
                if e.raw_os_error() != Some(1062) {
                    return Err(e); // 1062 = not started
                }
            }
        }
    }
    let until = Instant::now() + timeout;
    loop {
        if query()? == want {
            return Ok(());
        }
        if Instant::now() >= until {
            return Err(io::Error::new(io::ErrorKind::TimedOut, format!("{name} did not {} within {} s", if start { "start" } else { "stop" }, timeout.as_secs())));
        }
        std::thread::sleep(Duration::from_millis(250));
    }
}

pub fn service_installed() -> bool {
    service_state(SERVICE_NAME).is_some()
}

pub fn service_running() -> bool {
    service_state(SERVICE_NAME) == Some(SERVICE_RUNNING)
}

// ---------------- `ttlcd service install|uninstall|status` ----------------

/// `ttlcd service install|uninstall|status` (install/uninstall elevate themselves).
pub fn run_service_cmd(action: &str) -> io::Result<u8> {
    match action.to_lowercase().as_str() {
        "status" => {
            println!("service installed: {}, running: {}", tf(service_installed()), tf(service_running()));
            Ok(0)
        }
        "install" => Ok(if super::elevated("service install", "ttlcd-service.log")? { install()? } else { 0 }),
        "uninstall" => Ok(if super::elevated("service uninstall", "ttlcd-service.log")? { uninstall() } else { 0 }),
        _ => {
            println!("ttlcd service install|uninstall|status");
            Ok(1)
        }
    }
}

fn sc(args: &[&str]) -> i32 {
    let (out, code) = run_tool("sc.exe", args);
    print!("{out}");
    code
}

fn install() -> io::Result<u8> {
    let exe = std::env::current_exe()?;
    if service_installed() {
        uninstall();
    }
    // a console helper started earlier would publish twice: stop it (other ttlcd.exe processes)
    kill_other_ttlcd();
    let bin = format!("\"{}\" sensor-service", exe.display());
    let rc = sc(&["create", SERVICE_NAME, "binPath=", &bin, "start=", "auto", "DisplayName=", "TT LCD sensors"]);
    if rc != 0 {
        println!("error: could not create the service ({rc})");
        return Ok(2);
    }
    sc(&["description", SERVICE_NAME, "Publishes the CPU temperature for TT LCD Studio (uses TOWEI's sensor driver)."]);
    sc(&["failure", SERVICE_NAME, "reset=", "86400", "actions=", "restart/5000"]);
    sc(&["start", SERVICE_NAME]);
    println!("installed and started the TT LCD sensors service");
    Ok(0)
}

fn uninstall() -> u8 {
    sc(&["stop", SERVICE_NAME]);
    for _ in 0..20 {
        if !service_running() {
            break;
        }
        std::thread::sleep(Duration::from_millis(250));
    }
    let rc = sc(&["delete", SERVICE_NAME]);
    if rc == 0 {
        println!("removed the TT LCD sensors service");
        0
    } else {
        println!("error: could not remove the service ({rc})");
        2
    }
}

fn kill_other_ttlcd() {
    use windows_sys::Win32::System::Diagnostics::ToolHelp::*;
    use windows_sys::Win32::System::Threading::{GetCurrentProcessId, OpenProcess, TerminateProcess, PROCESS_TERMINATE};
    unsafe {
        let snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if snap.is_null() || snap == windows_sys::Win32::Foundation::INVALID_HANDLE_VALUE {
            return;
        }
        let me = GetCurrentProcessId();
        let mut e: PROCESSENTRY32W = std::mem::zeroed();
        e.dwSize = std::mem::size_of::<PROCESSENTRY32W>() as u32;
        let mut ok = Process32FirstW(snap, &mut e) != 0;
        while ok {
            if e.th32ProcessID != me && super::from_wide(&e.szExeFile).eq_ignore_ascii_case("ttlcd.exe") {
                let h = OpenProcess(PROCESS_TERMINATE, 0, e.th32ProcessID);
                if !h.is_null() {
                    TerminateProcess(h, 1);
                    CloseHandle(h);
                }
            }
            ok = Process32NextW(snap, &mut e) != 0;
        }
        CloseHandle(snap);
    }
}

// ---------------- the service itself ----------------

static STOP: AtomicBool = AtomicBool::new(false);
static STATUS: AtomicPtr<c_void> = AtomicPtr::new(null_mut());

fn report(state: u32, exit_code: u32, specific: u32, wait_ms: u32) {
    let h = STATUS.load(Ordering::SeqCst);
    if h.is_null() {
        return;
    }
    let st = SERVICE_STATUS {
        dwServiceType: SERVICE_WIN32_OWN_PROCESS,
        dwCurrentState: state,
        dwControlsAccepted: if state == SERVICE_RUNNING { SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN } else { 0 },
        dwWin32ExitCode: exit_code,
        dwServiceSpecificExitCode: specific,
        dwCheckPoint: 0,
        dwWaitHint: wait_ms,
    };
    unsafe { SetServiceStatus(h, &st) };
}

unsafe extern "system" fn control_handler(control: u32, _event: u32, _data: *mut c_void, _ctx: *mut c_void) -> u32 {
    match control {
        SERVICE_CONTROL_STOP | SERVICE_CONTROL_SHUTDOWN => {
            STOP.store(true, Ordering::SeqCst);
            report(SERVICE_STOP_PENDING, NO_ERROR, 0, 3000);
            NO_ERROR
        }
        SERVICE_CONTROL_INTERROGATE => NO_ERROR,
        _ => windows_sys::Win32::Foundation::ERROR_CALL_NOT_IMPLEMENTED,
    }
}

unsafe extern "system" fn service_proc(_argc: u32, _argv: *mut windows_sys::core::PWSTR) {
    let name = wide(SERVICE_NAME);
    let h = RegisterServiceCtrlHandlerExW(name.as_ptr(), Some(control_handler), null());
    if h.is_null() {
        return;
    }
    STATUS.store(h, Ordering::SeqCst);
    STOP.store(false, Ordering::SeqCst);
    report(SERVICE_RUNNING, NO_ERROR, 0, 0);
    let rc = std::panic::catch_unwind(|| run_helper(&|| STOP.load(Ordering::SeqCst)));
    match rc {
        Ok(Ok(0)) => report(SERVICE_STOPPED, NO_ERROR, 0, 0),
        // TOWEI missing or its DLL unusable: stop with a service-specific error (shown by `sc query`)
        Ok(Ok(code)) => report(SERVICE_STOPPED, windows_sys::Win32::Foundation::ERROR_SERVICE_SPECIFIC_ERROR, code as u32, 0),
        Ok(Err(_)) | Err(_) => report(SERVICE_STOPPED, windows_sys::Win32::Foundation::ERROR_SERVICE_SPECIFIC_ERROR, 2, 0),
    }
}

/// `ttlcd sensor-service`: entry point when started by the service control manager. Returns when the service stops.
pub fn service_main() -> io::Result<u8> {
    let mut name = wide(SERVICE_NAME);
    let table = [
        SERVICE_TABLE_ENTRYW { lpServiceName: name.as_mut_ptr(), lpServiceProc: Some(service_proc) },
        SERVICE_TABLE_ENTRYW { lpServiceName: null_mut(), lpServiceProc: None },
    ];
    if unsafe { StartServiceCtrlDispatcherW(table.as_ptr()) } == 0 {
        let e = io::Error::last_os_error();
        if e.raw_os_error() == Some(1063) {
            // ERROR_FAILED_SERVICE_CONTROLLER_CONNECT: started from a console, not by Windows
            println!("ttlcd sensor-service is started by Windows; use `ttlcd service install` (or `ttlcd hwmon` in a console)");
            return Ok(1);
        }
        return Err(e);
    }
    Ok(0)
}
