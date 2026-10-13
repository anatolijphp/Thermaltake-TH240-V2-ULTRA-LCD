// Windows only: icon, manifest (system DPI aware, asInvoker), version info and the GUI subsystem for the setup.
// The packer (ttlcd-pack) is a host tool and gets nothing.
include!("winres.rs");

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-changed=winres.rs");
    let icon = std::path::Path::new(env!("CARGO_MANIFEST_DIR")).join("../../src/app.ico");
    embed_windows_resources(&WinRes {
        bin: Some("TTLcdStudio-Setup"),
        icon: &icon,
        description: "TT LCD Studio setup",
        file_name: "TTLcdStudio-Setup.exe",
        dpi: Dpi::System,
        gui: true,
    });
}
