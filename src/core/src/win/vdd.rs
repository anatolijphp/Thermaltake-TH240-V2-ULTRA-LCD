//! Installs/uninstalls the bundled MttVDD IddCx virtual display (driver\ next to ttlcd.exe) configured as a single
//! 480x480 monitor, so Wallpaper Engine can render to it and the engine can mirror it to the LCD. Port of
//! VirtualDisplay.cs.
use super::{elevated, program_data, reg, run_tool, system_dir, wide};
use std::io;
use std::ptr::null_mut;
use windows_sys::core::GUID;
use windows_sys::Win32::Devices::DeviceAndDriverInstallation::*;
use windows_sys::Win32::Foundation::GetLastError;

const HW_ID: &str = "Root\\MttVDD";
const REG_KEY: &str = "SOFTWARE\\MikeTheTech\\VirtualDisplayDriver";
const DISPLAY_CLASS: GUID = GUID::from_u128(0x4d36e968_e325_11ce_bfc1_08002be10318);

fn settings_dir() -> std::path::PathBuf {
    program_data().join("ttlcd")
}

pub fn run(action: &str) -> io::Result<u8> {
    match action.to_ascii_lowercase().as_str() {
        "status" => status(),
        "install" => {
            if elevated("display install", "ttlcd-display.log")? {
                install()
            } else {
                Ok(0)
            }
        }
        "uninstall" => {
            if elevated("display uninstall", "ttlcd-display.log")? {
                uninstall()
            } else {
                Ok(0)
            }
        }
        _ => {
            println!("ttlcd display install|uninstall|status");
            Ok(1)
        }
    }
}

fn fail(what: &str) -> io::Error {
    io::Error::other(format!("{} failed, error 0x{:X}", what, unsafe { GetLastError() }))
}

fn status() -> io::Result<u8> {
    println!("virtual display devices: {}", count_devices(false)?);
    println!("driver package: {}", find_oem_inf().unwrap_or_else(|| "not installed".into()));
    match crate::capture::find_by_size(480, 480) {
        Some(m) => println!("480x480 monitor: #{m}"),
        None => println!("480x480 monitor: none"),
    }
    Ok(0)
}

fn install() -> io::Result<u8> {
    let exe = std::env::current_exe()?;
    let drv = exe.parent().unwrap_or(std::path::Path::new(".")).join("driver").join("MttVDD.inf");
    if !drv.exists() {
        return Err(io::Error::other(format!("bundled driver missing: {}", drv.display())));
    }
    if count_devices(false)? > 0 {
        println!("already installed");
        return Ok(0);
    }
    std::fs::create_dir_all(settings_dir())?;
    std::fs::write(settings_dir().join("vdd_settings.xml"), SETTINGS_XML)?;
    reg::set_string(reg::HKLM, REG_KEY, "VDDPATH", &settings_dir().to_string_lossy());
    unsafe {
        let set = SetupDiCreateDeviceInfoList(&DISPLAY_CLASS, null_mut());
        let mut d: SP_DEVINFO_DATA = std::mem::zeroed();
        d.cbSize = std::mem::size_of::<SP_DEVINFO_DATA>() as u32;
        let r = (|| {
            if SetupDiCreateDeviceInfoW(set, wide("Display").as_ptr(), &DISPLAY_CLASS, std::ptr::null(), null_mut(), DICD_GENERATE_ID, &mut d) == 0 {
                return Err(fail("SetupDiCreateDeviceInfo"));
            }
            let ids: Vec<u16> = HW_ID.encode_utf16().chain([0, 0]).collect();
            if SetupDiSetDeviceRegistryPropertyW(set, &mut d, SPDRP_HARDWAREID, ids.as_ptr() as *const u8, (ids.len() * 2) as u32) == 0 {
                return Err(fail("SetupDiSetDeviceRegistryProperty"));
            }
            if SetupDiCallClassInstaller(DIF_REGISTERDEVICE, set, &mut d) == 0 {
                return Err(fail("DIF_REGISTERDEVICE"));
            }
            Ok(())
        })();
        SetupDiDestroyDeviceInfoList(set);
        r?;
        let mut reboot = 0;
        if UpdateDriverForPlugAndPlayDevicesW(null_mut(), wide(HW_ID).as_ptr(), wide(&drv.to_string_lossy()).as_ptr(), INSTALLFLAG_FORCE, &mut reboot) == 0 {
            let err = GetLastError();
            let _ = uninstall();
            return Err(io::Error::other(format!("driver install failed, error 0x{err:X} (rolled back)")));
        }
        println!("installed virtual 480x480 display{}", if reboot != 0 { " (reboot required)" } else { "" });
    }
    println!("Assign a Wallpaper Engine wallpaper to it; the Wallpaper Engine mode mirrors it to the cooler.");
    Ok(0)
}

fn uninstall() -> io::Result<u8> {
    println!("removed {} virtual display device(s)", count_devices(true)?);
    if let Some(oem) = find_oem_inf() {
        run_tool(&pnputil(), &["/delete-driver", &oem, "/uninstall", "/force"]);
        println!("deleted driver package {oem}");
    }
    reg::delete_tree(reg::HKLM, REG_KEY, 0);
    if reg::is_empty(reg::HKLM, "SOFTWARE\\MikeTheTech") {
        reg::delete_tree(reg::HKLM, "SOFTWARE\\MikeTheTech", 0);
    }
    let _ = std::fs::remove_dir_all(settings_dir());
    println!("leftovers: devices={} package={}", count_devices(false)?, find_oem_inf().unwrap_or_else(|| "none".into()));
    Ok(0)
}

/// Counts (and optionally removes) devices whose hardware ID is Root\MttVDD.
fn count_devices(remove: bool) -> io::Result<usize> {
    let mut n = 0;
    unsafe {
        let set = SetupDiGetClassDevsW(std::ptr::null(), std::ptr::null(), null_mut(), DIGCF_ALLCLASSES);
        let mut d: SP_DEVINFO_DATA = std::mem::zeroed();
        d.cbSize = std::mem::size_of::<SP_DEVINFO_DATA>() as u32;
        let mut buf = [0u8; 2048];
        let mut i = 0;
        while SetupDiEnumDeviceInfo(set, i, &mut d) != 0 {
            i += 1;
            let (mut ty, mut req) = (0u32, 0u32);
            if SetupDiGetDeviceRegistryPropertyW(set, &d, SPDRP_HARDWAREID, &mut ty, buf.as_mut_ptr(), buf.len() as u32, &mut req) == 0 {
                continue;
            }
            let ids: Vec<u16> = buf[..req as usize].chunks_exact(2).map(|c| u16::from_le_bytes([c[0], c[1]])).collect();
            let ids = String::from_utf16_lossy(&ids);
            if !ids.to_lowercase().contains(&HW_ID.to_lowercase()) {
                continue;
            }
            n += 1;
            if remove && SetupDiCallClassInstaller(DIF_REMOVE, set, &mut d) == 0 {
                SetupDiDestroyDeviceInfoList(set);
                return Err(fail("DIF_REMOVE"));
            }
        }
        SetupDiDestroyDeviceInfoList(set);
    }
    Ok(n)
}

fn pnputil() -> String {
    system_dir().join("pnputil.exe").to_string_lossy().into_owned()
}

fn find_oem_inf() -> Option<String> {
    let (out, _) = run_tool(&pnputil(), &["/enum-drivers"]);
    let mut current: Option<String> = None;
    for line in out.lines() {
        let l = line.to_lowercase();
        if let Some(k) = l.find("oem") {
            let rest = &l[k..];
            if let Some(end) = rest.find(".inf") {
                let name = &rest[..end + 4];
                if name[3..name.len() - 4].chars().all(|c| c.is_ascii_digit()) && l.contains(':') && !l.contains("mttvdd") {
                    current = Some(name.to_owned());
                    continue;
                }
            }
        }
        if current.is_some() && l.contains("mttvdd.inf") {
            return current;
        }
    }
    None
}

const SETTINGS_XML: &str = "<?xml version='1.0' encoding='utf-8'?>
<vdd_settings>
    <monitors><count>1</count></monitors>
    <gpu><friendlyname>default</friendlyname></gpu>
    <global><g_refresh_rate>60</g_refresh_rate></global>
    <resolutions>
        <resolution><width>480</width><height>480</height><refresh_rate>60</refresh_rate></resolution>
    </resolutions>
    <options>
        <CustomEdid>false</CustomEdid>
        <PreventSpoof>false</PreventSpoof>
        <EdidCeaOverride>false</EdidCeaOverride>
        <HardwareCursor>true</HardwareCursor>
        <SDR10bit>false</SDR10bit>
        <HDRPlus>false</HDRPlus>
        <logging>false</logging>
        <debuglogging>false</debuglogging>
    </options>
</vdd_settings>
";
