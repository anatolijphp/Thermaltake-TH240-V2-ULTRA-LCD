//! TTLcdAura.dll: ASUS Aura Sync (Armoury Crate) HAL plugin for the Thermaltake CL-W383 LCD, a native in-process
//! COM server (port of aura/AuraHal.cs). Windows only; elsewhere only the platform-independent parts (and their
//! tests) are built.
//!
//! ASUS LightingService (32-bit, SYSTEM) creates the COM object CLSID {C0C96C60-30E3-4021-9904-966EC4FA0F0F} from
//! the Aura HAL category CLSID\{109DC3E4-...}\Instance\{E9BBD754-...}\Instance\{CLSID} (registered by
//! `ttlcd aura install`). It enumerates one "device" whose LEDs are virtual: every effect/colour Aura sends is
//! written to the shared memory block "Global\TTLcdAuraSync", and TT LCD Studio draws it.
#![allow(non_snake_case)] // the crate is named after the DLL (TTLcdAura)
#![allow(clippy::missing_safety_doc)]

#[cfg(any(windows, test))]
mod caps;
#[cfg(windows)]
mod com;
#[cfg(windows)]
mod shared;

#[cfg(windows)]
pub use com::{DllCanUnloadNow, DllGetClassObject};
