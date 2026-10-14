//! The in-process COM server, written by hand: a class factory for the HAL, the HAL (IAacLedDeviceHal) and its one
//! device (IAacLedDevice / IAacLedDeviceOpt). Interface layout follows the public AacHalSample. Every object is
//! free-threaded (ThreadingModel Both): reference counts are atomic, the shared memory has its own lock, and like a
//! .NET object (what the C# plugin was) each one aggregates the free-threaded marshaler, so it can be handed between
//! apartments without a proxy/stub.
use crate::{caps, shared};
use std::ffi::c_void;
use std::ptr::null_mut;
use std::sync::atomic::{AtomicI32, AtomicU32, Ordering};
use windows_sys::core::{BSTR, GUID, HRESULT};

const S_OK: HRESULT = 0;
const S_FALSE: HRESULT = 1;
const E_NOINTERFACE: HRESULT = 0x8000_4002_u32 as i32;
const E_POINTER: HRESULT = 0x8000_4003_u32 as i32;
const E_FAIL: HRESULT = 0x8000_4005_u32 as i32;
const E_OUTOFMEMORY: HRESULT = 0x8007_000E_u32 as i32;
const CLASS_E_NOAGGREGATION: HRESULT = 0x8004_0110_u32 as i32;
const CLASS_E_CLASSNOTAVAILABLE: HRESULT = 0x8004_0111_u32 as i32;

pub const CLSID_HAL: GUID = GUID::from_u128(0xC0C96C60_30E3_4021_9904_966EC4FA0F0F);
const IID_IUNKNOWN: GUID = GUID::from_u128(0x00000000_0000_0000_C000_000000000046);
const IID_ICLASSFACTORY: GUID = GUID::from_u128(0x00000001_0000_0000_C000_000000000046);
const IID_IMARSHAL: GUID = GUID::from_u128(0x00000003_0000_0000_C000_000000000046);
const IID_IAGILEOBJECT: GUID = GUID::from_u128(0x94EA2B94_E9CC_49E0_C0FF_EE64CA8F5B90);
const IID_HAL: GUID = GUID::from_u128(0xF2C8D5B4_3854_4325_8A4F_FD7C5072E3B9);
const IID_DEVICE: GUID = GUID::from_u128(0x61711778_AB59_4026_89E8_7A63422C29C2);
const IID_DEVICE_OPT: GUID = GUID::from_u128(0x68F0C6E1_7469_40B3_84D5_E0793F449E4D);

/// Live objects + LockServer locks + class factory references: the DLL may unload when all are 0.
static OBJECTS: AtomicI32 = AtomicI32::new(0);
static LOCKS: AtomicI32 = AtomicI32::new(0);

fn eq(a: &GUID, b: &GUID) -> bool {
    a.data1 == b.data1 && a.data2 == b.data2 && a.data3 == b.data3 && a.data4 == b.data4
}

/// Runs a method body; a panic becomes E_FAIL (like the C# plugin's catch-all) instead of crossing into COM.
fn guard(what: &str, f: impl FnOnce() -> HRESULT) -> HRESULT {
    match std::panic::catch_unwind(std::panic::AssertUnwindSafe(f)) {
        Ok(hr) => hr,
        Err(e) => {
            let msg = e.downcast_ref::<&str>().map(|s| s.to_string()).or_else(|| e.downcast_ref::<String>().cloned()).unwrap_or_default();
            shared::log(&format!("{what}: panic {msg}"));
            E_FAIL
        }
    }
}

// ---------------- IUnknown helpers ----------------

#[repr(C)]
struct UnknownVtbl {
    query_interface: unsafe extern "system" fn(*mut c_void, *const GUID, *mut *mut c_void) -> HRESULT,
    add_ref: unsafe extern "system" fn(*mut c_void) -> u32,
    release: unsafe extern "system" fn(*mut c_void) -> u32,
}

/// Calls a method of any IUnknown-derived interface pointer.
unsafe fn vtbl(p: *mut c_void) -> &'static UnknownVtbl {
    &**(p as *const *const UnknownVtbl)
}

/// The aggregated free-threaded marshaler (its inner, non-delegating IUnknown), or null if it couldn't be created.
unsafe fn create_ftm(outer: *mut c_void) -> *mut c_void {
    let mut inner = null_mut();
    if windows_sys::Win32::System::Com::CoCreateFreeThreadedMarshaler(outer, &mut inner) < 0 {
        return null_mut();
    }
    inner
}

/// Common header of the HAL and device objects: vtable, reference count, aggregated marshaler.
#[repr(C)]
struct Object<V: 'static> {
    vtbl: &'static V,
    refs: AtomicU32,
    ftm: *mut c_void,
}

impl<V: 'static> Object<V> {
    /// A new object with one reference for the caller.
    fn create(vtbl: &'static V) -> *mut Object<V> {
        let p = Box::into_raw(Box::new(Object { vtbl, refs: AtomicU32::new(1), ftm: null_mut() }));
        OBJECTS.fetch_add(1, Ordering::SeqCst);
        unsafe { (*p).ftm = create_ftm(p as *mut c_void) };
        p
    }

    unsafe fn add_ref(this: *mut Self) -> u32 {
        (*this).refs.fetch_add(1, Ordering::Relaxed) + 1
    }

    unsafe fn release(this: *mut Self) -> u32 {
        let n = (*this).refs.fetch_sub(1, Ordering::Release) - 1;
        if n == 0 {
            std::sync::atomic::fence(Ordering::Acquire);
            // Releasing the aggregated marshaler can call back into our AddRef/Release (COM aggregation rules):
            // guard against a second destruction by holding an artificial reference meanwhile.
            (*this).refs.store(1, Ordering::Relaxed);
            let ftm = std::mem::replace(&mut (*this).ftm, null_mut());
            if !ftm.is_null() {
                (vtbl(ftm).release)(ftm);
            }
            drop(Box::from_raw(this));
            OBJECTS.fetch_sub(1, Ordering::SeqCst);
        }
        n
    }

    /// QueryInterface for an object whose own interfaces are `iids` (IUnknown and IAgileObject are implied).
    unsafe fn query(this: *mut Self, riid: *const GUID, ppv: *mut *mut c_void, iids: &[GUID]) -> HRESULT {
        if ppv.is_null() {
            return E_POINTER;
        }
        *ppv = null_mut();
        if riid.is_null() {
            return E_POINTER;
        }
        let riid = &*riid;
        if eq(riid, &IID_IUNKNOWN) || eq(riid, &IID_IAGILEOBJECT) || iids.iter().any(|i| eq(riid, i)) {
            Self::add_ref(this);
            *ppv = this as *mut c_void;
            return S_OK;
        }
        if eq(riid, &IID_IMARSHAL) && !(*this).ftm.is_null() {
            let ftm = (*this).ftm;
            return (vtbl(ftm).query_interface)(ftm, riid, ppv);
        }
        E_NOINTERFACE
    }
}

// ---------------- class factory ----------------

#[repr(C)]
struct FactoryVtbl {
    query_interface: unsafe extern "system" fn(*mut Factory, *const GUID, *mut *mut c_void) -> HRESULT,
    add_ref: unsafe extern "system" fn(*mut Factory) -> u32,
    release: unsafe extern "system" fn(*mut Factory) -> u32,
    create_instance: unsafe extern "system" fn(*mut Factory, *mut c_void, *const GUID, *mut *mut c_void) -> HRESULT,
    lock_server: unsafe extern "system" fn(*mut Factory, i32) -> HRESULT,
}

/// The one class factory: static, its references count as server locks.
#[repr(C)]
struct Factory {
    vtbl: &'static FactoryVtbl,
}

static FACTORY_VTBL: FactoryVtbl = FactoryVtbl {
    query_interface: factory_query_interface,
    add_ref: factory_add_ref,
    release: factory_release,
    create_instance: factory_create_instance,
    lock_server: factory_lock_server,
};

static FACTORY: Factory = Factory { vtbl: &FACTORY_VTBL };

unsafe extern "system" fn factory_query_interface(this: *mut Factory, riid: *const GUID, ppv: *mut *mut c_void) -> HRESULT {
    if ppv.is_null() {
        return E_POINTER;
    }
    *ppv = null_mut();
    if riid.is_null() {
        return E_POINTER;
    }
    if eq(&*riid, &IID_IUNKNOWN) || eq(&*riid, &IID_ICLASSFACTORY) {
        factory_add_ref(this);
        *ppv = this as *mut c_void;
        return S_OK;
    }
    E_NOINTERFACE
}

unsafe extern "system" fn factory_add_ref(_: *mut Factory) -> u32 {
    (LOCKS.fetch_add(1, Ordering::SeqCst) + 1).max(1) as u32
}

unsafe extern "system" fn factory_release(_: *mut Factory) -> u32 {
    (LOCKS.fetch_sub(1, Ordering::SeqCst) - 1).max(1) as u32
}

unsafe extern "system" fn factory_create_instance(_: *mut Factory, outer: *mut c_void, riid: *const GUID, ppv: *mut *mut c_void) -> HRESULT {
    guard("CreateInstance", || {
        if ppv.is_null() {
            return E_POINTER;
        }
        *ppv = null_mut();
        if !outer.is_null() {
            return CLASS_E_NOAGGREGATION;
        }
        let hal = Object::create(&HAL_VTBL);
        let hr = Object::query(hal, riid, ppv, &[IID_HAL]);
        Object::release(hal); // the caller's reference (if any) keeps it alive
        if hr == S_OK {
            shared::log("CreateInstance");
        }
        hr
    })
}

unsafe extern "system" fn factory_lock_server(_: *mut Factory, lock: i32) -> HRESULT {
    if lock != 0 {
        LOCKS.fetch_add(1, Ordering::SeqCst);
    } else {
        LOCKS.fetch_sub(1, Ordering::SeqCst);
    }
    S_OK
}

// ---------------- IAacLedDeviceHal ----------------

#[repr(C)]
struct HalVtbl {
    query_interface: unsafe extern "system" fn(*mut Hal, *const GUID, *mut *mut c_void) -> HRESULT,
    add_ref: unsafe extern "system" fn(*mut Hal) -> u32,
    release: unsafe extern "system" fn(*mut Hal) -> u32,
    enumerate: unsafe extern "system" fn(*mut Hal, *mut *mut c_void, *mut u32) -> HRESULT,
}

type Hal = Object<HalVtbl>;

static HAL_VTBL: HalVtbl = HalVtbl { query_interface: hal_query_interface, add_ref: hal_add_ref, release: hal_release, enumerate: hal_enumerate };

unsafe extern "system" fn hal_query_interface(this: *mut Hal, riid: *const GUID, ppv: *mut *mut c_void) -> HRESULT {
    Object::query(this, riid, ppv, &[IID_HAL])
}

unsafe extern "system" fn hal_add_ref(this: *mut Hal) -> u32 {
    Object::add_ref(this)
}

unsafe extern "system" fn hal_release(this: *mut Hal) -> u32 {
    Object::release(this)
}

/// Enumerate(IAacLedDevice** devices, ULONG* count): one device. Same contract as the ASUS samples: devices == NULL
/// just reports the count.
unsafe extern "system" fn hal_enumerate(_: *mut Hal, devices: *mut *mut c_void, count: *mut u32) -> HRESULT {
    guard("Enumerate", || {
        if count.is_null() {
            return E_POINTER;
        }
        let capacity = count.read_unaligned();
        count.write_unaligned(1);
        if !devices.is_null() {
            // a new reference for the caller
            devices.write_unaligned(Object::create(&DEVICE_VTBL) as *mut c_void);
        }
        shared::log(&format!("Enumerate cap={} devices={}", capacity, if devices.is_null() { "False" } else { "True" }));
        S_OK
    })
}

// ---------------- IAacLedDevice / IAacLedDeviceOpt ----------------

/// IAacLedDeviceOpt(Speed) is IAacLedDevice plus SetEffectOptSpeed, so one vtable serves both interfaces.
#[repr(C)]
struct DeviceVtbl {
    query_interface: unsafe extern "system" fn(*mut Device, *const GUID, *mut *mut c_void) -> HRESULT,
    add_ref: unsafe extern "system" fn(*mut Device) -> u32,
    release: unsafe extern "system" fn(*mut Device) -> u32,
    get_capability: unsafe extern "system" fn(*mut Device, *mut BSTR) -> HRESULT,
    set_effect: unsafe extern "system" fn(*mut Device, u32, *const u32, u32) -> HRESULT,
    synchronize: unsafe extern "system" fn(*mut Device, u32, u64) -> HRESULT,
    set_effect_opt_speed: unsafe extern "system" fn(*mut Device, u32, *const u32, u32, u32, u32) -> HRESULT,
}

type Device = Object<DeviceVtbl>;

static DEVICE_VTBL: DeviceVtbl = DeviceVtbl {
    query_interface: device_query_interface,
    add_ref: device_add_ref,
    release: device_release,
    get_capability: device_get_capability,
    set_effect: device_set_effect,
    synchronize: device_synchronize,
    set_effect_opt_speed: device_set_effect_opt_speed,
};

unsafe extern "system" fn device_query_interface(this: *mut Device, riid: *const GUID, ppv: *mut *mut c_void) -> HRESULT {
    Object::query(this, riid, ppv, &[IID_DEVICE, IID_DEVICE_OPT])
}

unsafe extern "system" fn device_add_ref(this: *mut Device) -> u32 {
    Object::add_ref(this)
}

unsafe extern "system" fn device_release(this: *mut Device) -> u32 {
    Object::release(this)
}

unsafe extern "system" fn device_get_capability(_: *mut Device, capability: *mut BSTR) -> HRESULT {
    guard("GetCapability", || {
        if capability.is_null() {
            return E_POINTER;
        }
        let x: Vec<u16> = caps::capability_xml().encode_utf16().collect();
        let b = windows_sys::Win32::Foundation::SysAllocStringLen(x.as_ptr(), x.len() as u32);
        if b.is_null() {
            return E_OUTOFMEMORY;
        }
        capability.write_unaligned(b);
        shared::log("GetCapability");
        S_OK
    })
}

unsafe extern "system" fn device_set_effect(this: *mut Device, effect: u32, colors: *const u32, n: u32) -> HRESULT {
    device_set_effect_opt_speed(this, effect, colors, n, 1, 0)
}

unsafe extern "system" fn device_set_effect_opt_speed(_: *mut Device, effect: u32, colors: *const u32, n: u32, speed: u32, direction: u32) -> HRESULT {
    guard("SetEffect", || {
        let mut c = [0u32; shared::MAX_LEDS];
        let n = n.min(shared::MAX_LEDS as u32);
        if !colors.is_null() {
            for (i, slot) in c.iter_mut().enumerate().take(n as usize) {
                *slot = colors.add(i).read_unaligned();
            }
        }
        match shared::write(effect, &c, n, speed, direction, -1) {
            Ok(()) => S_OK,
            Err(e) => {
                shared::log(&format!("SetEffect: {e}"));
                E_FAIL
            }
        }
    })
}

unsafe extern "system" fn device_synchronize(_: *mut Device, effect: u32, ms: u64) -> HRESULT {
    guard("Synchronize", || match shared::write_sync(effect, ms as i64) {
        Ok(()) => S_OK,
        Err(e) => {
            shared::log(&format!("Synchronize: {e}"));
            E_FAIL
        }
    })
}

// ---------------- DLL exports ----------------

/// # Safety
/// Called by COM with valid pointers (or null, which is rejected).
#[no_mangle]
pub unsafe extern "system" fn DllGetClassObject(rclsid: *const GUID, riid: *const GUID, ppv: *mut *mut c_void) -> HRESULT {
    if ppv.is_null() {
        return E_POINTER;
    }
    *ppv = null_mut();
    if rclsid.is_null() || riid.is_null() {
        return E_POINTER;
    }
    if !eq(&*rclsid, &CLSID_HAL) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    factory_query_interface(&FACTORY as *const Factory as *mut Factory, riid, ppv)
}

#[no_mangle]
pub extern "system" fn DllCanUnloadNow() -> HRESULT {
    if OBJECTS.load(Ordering::SeqCst) == 0 && LOCKS.load(Ordering::SeqCst) <= 0 {
        S_OK
    } else {
        S_FALSE
    }
}
