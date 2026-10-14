// Windows only: icon, manifest (PerMonitorV2 DPI, common controls v6, asInvoker), version info, GUI subsystem.
include!("../installer/winres.rs");

/// The UI (React + Sass, ui/) bundled by esbuild into OUT_DIR/ui with the Node.js from tools/node; main.rs embeds it.
/// node_modules comes from `npm ci` (versions and hashes pinned by ui/package-lock.json) the first time.
fn build_ui() {
    use std::path::PathBuf;
    use std::process::Command;
    let manifest = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let ui = manifest.join("ui");
    // every source file by name: a directory's own timestamp doesn't change reliably (NTFS mounts on Linux)
    fn watch(dir: &std::path::Path) {
        for e in std::fs::read_dir(dir).into_iter().flatten().flatten() {
            let p = e.path();
            if p.is_dir() {
                watch(&p);
            } else {
                println!("cargo:rerun-if-changed={}", p.display());
            }
        }
    }
    watch(&ui.join("src"));
    for f in ["index.html", "build.mjs", "package.json", "package-lock.json"] {
        println!("cargo:rerun-if-changed={}", ui.join(f).display());
    }
    let tools = manifest.join("../../tools/node");
    let (node, npm_cli) = if cfg!(windows) {
        (tools.join("node.exe"), tools.join("node_modules/npm/bin/npm-cli.js"))
    } else {
        (tools.join("bin/node"), tools.join("lib/node_modules/npm/bin/npm-cli.js"))
    };
    if !node.exists() {
        panic!("Node.js not found at {} — run src/fetch.sh (Linux) or src\\fetch.cmd (Windows) first", node.display());
    }
    // node finds its own npm and the tools on PATH
    let path = format!("{}{}{}", node.parent().unwrap().display(), if cfg!(windows) { ";" } else { ":" }, std::env::var("PATH").unwrap_or_default());
    let cache = manifest.join("../../tools/npm-cache");
    // esbuild's native binary is per OS: node_modules installed on the other host (the same folder used from Linux and
    // Windows) is reinstalled
    let esbuild_bin = if cfg!(windows) { "node_modules/@esbuild/win32-x64" } else { "node_modules/@esbuild/linux-x64" };
    if !ui.join("node_modules/esbuild").exists() || !ui.join("node_modules/sass").exists() || !ui.join(esbuild_bin).exists() {
        let ok = Command::new(&node)
            .arg(&npm_cli)
            .args(["ci", "--no-audit", "--no-fund", "--loglevel=error"])
            .current_dir(&ui)
            .env("PATH", &path)
            .env("npm_config_cache", &cache)
            .env("npm_config_update_notifier", "false")
            .status()
            .map(|s| s.success())
            .unwrap_or(false);
        if !ok {
            panic!("npm ci failed in {}", ui.display());
        }
    }
    let out = PathBuf::from(std::env::var("OUT_DIR").unwrap()).join("ui");
    let ok = Command::new(&node).arg(ui.join("build.mjs")).arg(&out).current_dir(&ui).env("PATH", &path).status().map(|s| s.success()).unwrap_or(false);
    if !ok {
        panic!("the UI build (ui/build.mjs) failed");
    }
}

fn main() {
    build_ui();
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
