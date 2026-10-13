// Windows only: icon, manifest (PerMonitorV2 DPI, asInvoker) and version info for ttlcd.exe (a console program).
include!("../installer/winres.rs");

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=../installer/winres.rs");
    let icon = std::path::Path::new(env!("CARGO_MANIFEST_DIR")).join("../../src/app.ico");
    embed_windows_resources(&WinRes {
        bin: Some("ttlcd"),
        icon: &icon,
        description: "TT LCD command line",
        file_name: "ttlcd.exe",
        dpi: Dpi::PerMonitorV2,
        gui: false,
    });
}
