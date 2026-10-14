// Windows only: icon, manifest (PerMonitorV2 DPI, common controls v6, asInvoker), version info, GUI subsystem.
include!("../installer/winres.rs");

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=../installer/winres.rs");
    let icon = std::path::Path::new(env!("CARGO_MANIFEST_DIR")).join("../app.ico");
    embed_windows_resources(&WinRes {
        bin: Some("ttlcd-studio"),
        icon: &icon,
        description: "TT LCD Studio",
        file_name: "TTLcdStudio.exe",
        dpi: Dpi::PerMonitorV2,
        gui: true,
    });
}
