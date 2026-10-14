//! Exercises TTLcdAura.dll the way ASUS LightingService does, without Armoury Crate (Windows):
//!   cargo build -p ttlcd-aura-hal --example probe --target <x86_64|i686>-pc-windows-gnullvm
//!   probe.exe [path\to\TTLcdAura.dll]     (default: the DLL in the folder above the example)
//! As a normal user the plugin can't create Global\TTLcdAuraSync and falls back to Local\TTLcdAuraSync, which this
//! probe then reads back. Exit code 0 = every check passed.

#[cfg(not(windows))]
fn main() {
    eprintln!("probe: Windows only");
}

#[cfg(windows)]
fn main() {
    std::process::exit(win::run());
}

#[cfg(windows)]
mod win {
    use std::ffi::c_void;
    use std::ptr::{null, null_mut};
    use windows_sys::core::{BSTR, GUID, HRESULT};
    use windows_sys::Win32::Foundation::{SysFreeString, SysStringLen};
    use windows_sys::Win32::System::Com::{CoInitializeEx, COINIT_MULTITHREADED};
    use windows_sys::Win32::System::LibraryLoader::{GetProcAddress, LoadLibraryW};
    use windows_sys::Win32::System::Memory::{MapViewOfFile, OpenFileMappingW, FILE_MAP_READ};

    const CLSID: GUID = GUID::from_u128(0xC0C96C60_30E3_4021_9904_966EC4FA0F0F);
    const IID_ICLASSFACTORY: GUID = GUID::from_u128(0x00000001_0000_0000_C000_000000000046);
    const IID_IMARSHAL: GUID = GUID::from_u128(0x00000003_0000_0000_C000_000000000046);
    const IID_HAL: GUID = GUID::from_u128(0xF2C8D5B4_3854_4325_8A4F_FD7C5072E3B9);
    const IID_DEVICE: GUID = GUID::from_u128(0x61711778_AB59_4026_89E8_7A63422C29C2);
    const IID_DEVICE_OPT: GUID = GUID::from_u128(0x68F0C6E1_7469_40B3_84D5_E0793F449E4D);

    type GetClassObject = unsafe extern "system" fn(*const GUID, *const GUID, *mut *mut c_void) -> HRESULT;
    type CanUnload = unsafe extern "system" fn() -> HRESULT;

    #[repr(C)]
    struct Unknown {
        qi: unsafe extern "system" fn(*mut c_void, *const GUID, *mut *mut c_void) -> HRESULT,
        add_ref: unsafe extern "system" fn(*mut c_void) -> u32,
        release: unsafe extern "system" fn(*mut c_void) -> u32,
    }
    #[repr(C)]
    struct Factory {
        base: Unknown,
        create_instance: unsafe extern "system" fn(*mut c_void, *mut c_void, *const GUID, *mut *mut c_void) -> HRESULT,
        lock_server: unsafe extern "system" fn(*mut c_void, i32) -> HRESULT,
    }
    #[repr(C)]
    struct Hal {
        base: Unknown,
        enumerate: unsafe extern "system" fn(*mut c_void, *mut *mut c_void, *mut u32) -> HRESULT,
    }
    #[repr(C)]
    struct Device {
        base: Unknown,
        get_capability: unsafe extern "system" fn(*mut c_void, *mut BSTR) -> HRESULT,
        set_effect: unsafe extern "system" fn(*mut c_void, u32, *const u32, u32) -> HRESULT,
        synchronize: unsafe extern "system" fn(*mut c_void, u32, u64) -> HRESULT,
        set_effect_opt_speed: unsafe extern "system" fn(*mut c_void, u32, *const u32, u32, u32, u32) -> HRESULT,
    }

    unsafe fn vt<T>(p: *mut c_void) -> &'static T {
        &**(p as *const *const T)
    }

    fn wide(s: &str) -> Vec<u16> {
        s.encode_utf16().chain(std::iter::once(0)).collect()
    }

    struct Checks(u32);
    impl Checks {
        fn check(&mut self, ok: bool, what: &str) {
            println!("{} {what}", if ok { "ok  " } else { "FAIL" });
            if !ok {
                self.0 += 1;
            }
        }
    }

    pub fn run() -> i32 {
        let dll = std::env::args().nth(1).map(std::path::PathBuf::from).unwrap_or_else(|| {
            let exe = std::env::current_exe().unwrap();
            exe.parent().and_then(|d| d.parent()).unwrap().join("TTLcdAura.dll")
        });
        let mut c = Checks(0);
        unsafe {
            CoInitializeEx(null(), COINIT_MULTITHREADED as u32);
            let h = LoadLibraryW(wide(&dll.to_string_lossy()).as_ptr());
            if h.is_null() {
                println!("FAIL cannot load {}", dll.display());
                return 1;
            }
            let (Some(gco), Some(cu)) = (GetProcAddress(h, c"DllGetClassObject".as_ptr() as _), GetProcAddress(h, c"DllCanUnloadNow".as_ptr() as _)) else {
                println!("FAIL exports missing");
                return 1;
            };
            let gco: GetClassObject = std::mem::transmute(gco);
            let can_unload: CanUnload = std::mem::transmute(cu);

            let mut f = null_mut();
            let wrong = GUID::from_u128(1);
            c.check(gco(&wrong, &IID_ICLASSFACTORY, &mut f) == 0x8004_0111_u32 as i32 && f.is_null(), "unknown CLSID refused");
            c.check(gco(&CLSID, &IID_ICLASSFACTORY, &mut f) == 0 && !f.is_null(), "class factory");
            c.check(can_unload() == 1, "DllCanUnloadNow = S_FALSE while the factory is held");
            let fv = vt::<Factory>(f);
            let mut hal = null_mut();
            c.check((fv.create_instance)(f, null_mut(), &IID_HAL, &mut hal) == 0 && !hal.is_null(), "CreateInstance(IAacLedDeviceHal)");
            (fv.base.release)(f);

            let hv = vt::<Hal>(hal);
            let mut n = 0u32;
            c.check((hv.enumerate)(hal, null_mut(), &mut n) == 0 && n == 1, "Enumerate(NULL) reports 1 device");
            let mut dev = null_mut();
            n = 1;
            c.check((hv.enumerate)(hal, &mut dev, &mut n) == 0 && n == 1 && !dev.is_null(), "Enumerate returns the device");
            c.check((hv.enumerate)(hal, null_mut(), null_mut()) == 0x8000_4003_u32 as i32, "Enumerate(count NULL) = E_POINTER");

            let mut opt = null_mut();
            let dv = vt::<Device>(dev);
            c.check((dv.base.qi)(dev, &IID_DEVICE_OPT, &mut opt) == 0 && opt == dev, "IAacLedDeviceOpt on the same object");
            (dv.base.release)(opt);
            let mut m = null_mut();
            c.check((dv.base.qi)(dev, &IID_IMARSHAL, &mut m) == 0 && !m.is_null(), "IMarshal (free-threaded marshaler)");
            if !m.is_null() {
                (vt::<Unknown>(m).release)(m);
            }
            let mut d2 = null_mut();
            c.check((dv.base.qi)(dev, &IID_DEVICE, &mut d2) == 0, "IAacLedDevice");
            (dv.base.release)(d2);

            let mut b: BSTR = null_mut();
            c.check((dv.get_capability)(dev, &mut b) == 0 && !b.is_null(), "GetCapability");
            if !b.is_null() {
                let xml = String::from_utf16_lossy(std::slice::from_raw_parts(b, SysStringLen(b) as usize));
                c.check(xml.contains("<name>TT LCD CL-W383</name>") && xml.contains("<led_count>8</led_count>") && xml.ends_with("</root>\n"), "capability XML");
                SysFreeString(b);
            }

            let colors: [u32; 8] = [0x0000FF, 0x00FF00, 0xFF0000, 0x123456, 0, 0, 0, 0xFFFFFF];
            c.check((dv.set_effect_opt_speed)(dev, 2, colors.as_ptr(), 8, 3, 0) == 0, "SetEffectOptSpeed");
            let block = read_block();
            match &block {
                Some(b) => {
                    let i = |o: usize| i32::from_le_bytes(b[o..o + 4].try_into().unwrap());
                    c.check(i(0) == 0x5341_5454, "magic TTAS");
                    c.check(i(8) == 2 && i(12) == 8 && i(16) == 3 && i(20) == 0, "effect, LED count, speed, direction");
                    c.check(i(40 + 3 * 4) as u32 == 0x123456 && i(40 + 8 * 4) == 0, "colours");
                    let seq = i(4);
                    (dv.synchronize)(dev, 5, 123_456_789);
                    let b2 = read_block().unwrap();
                    let j = |o: usize| i64::from_le_bytes(b2[o..o + 8].try_into().unwrap());
                    c.check(i32::from_le_bytes(b2[4..8].try_into().unwrap()) == seq.wrapping_add(1), "Synchronize bumps seq");
                    c.check(j(24) == 123_456_789 && j(32) > 0, "sync ms and update time");
                    let many = [0x00FF00u32; 100];
                    c.check((dv.set_effect)(dev, 1, many.as_ptr(), 100) == 0, "SetEffect with 100 colours");
                    let b3 = read_block().unwrap();
                    c.check(i32::from_le_bytes(b3[12..16].try_into().unwrap()) == 64, "LED count clamped to 64");
                }
                None => c.check(false, "shared memory block readable"),
            }
            c.check((dv.base.release)(dev) == 0, "device released");
            c.check((hv.base.release)(hal) == 0, "HAL released");
            c.check(can_unload() == 0, "DllCanUnloadNow = S_OK after everything is released");
        }
        println!("{}", if c.0 == 0 { "all checks passed" } else { "some checks FAILED" });
        c.0 as i32
    }

    fn read_block() -> Option<Vec<u8>> {
        const SIZE: usize = 40 + 64 * 4;
        for name in ["Global\\TTLcdAuraSync", "Local\\TTLcdAuraSync"] {
            unsafe {
                let h = OpenFileMappingW(FILE_MAP_READ, 0, wide(name).as_ptr());
                if h.is_null() {
                    continue;
                }
                let v = MapViewOfFile(h, FILE_MAP_READ, 0, 0, SIZE);
                if v.Value.is_null() {
                    continue;
                }
                return Some(std::slice::from_raw_parts(v.Value as *const u8, SIZE).to_vec()); // (view leaked: a probe)
            }
        }
        None
    }
}
