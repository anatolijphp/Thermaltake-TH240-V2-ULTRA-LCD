// Shared by the build scripts of ttlcd-studio, ttlcd-cli and ttlcd-setup (pulled in with `include!`): embeds the app
// icon, an application manifest and version info into a Windows binary. Uses the resource compiler of the bundled
// llvm-mingw (`<arch>-w64-mingw32-windres`, i.e. llvm-windres) found on PATH; it needs no C preprocessor.
// Does nothing when the target is not Windows.

/// Manifest flavour: per-monitor v2 DPI awareness (Studio, CLI) or system DPI awareness (the setup window, which
/// lays itself out once at the system DPI).
#[allow(dead_code)]
#[derive(Clone, Copy, PartialEq)]
enum Dpi {
    PerMonitorV2,
    System,
}

#[allow(dead_code)]
struct WinRes<'a> {
    /// Only this bin target gets the resources (None: every bin of the package).
    bin: Option<&'a str>,
    /// .ico file (resource ID 1, the first icon group: what Explorer, shortcuts and tao's Icon::from_resource(1) use).
    icon: &'a std::path::Path,
    /// FileDescription in the version info (shown by Task Manager and Explorer).
    description: &'a str,
    /// OriginalFilename in the version info.
    file_name: &'a str,
    dpi: Dpi,
    /// Link with the GUI subsystem (no console window).
    gui: bool,
}

fn manifest(dpi: Dpi) -> String {
    let dpi_xml = match dpi {
        Dpi::PerMonitorV2 => {
            "      <dpiAware xmlns=\"http://schemas.microsoft.com/SMI/2005/WindowsSettings\">true/pm</dpiAware>\n      \
             <dpiAwareness xmlns=\"http://schemas.microsoft.com/SMI/2016/WindowsSettings\">PerMonitorV2, PerMonitor</dpiAwareness>\n"
        }
        Dpi::System => "      <dpiAware xmlns=\"http://schemas.microsoft.com/SMI/2005/WindowsSettings\">true</dpiAware>\n",
    };
    format!(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">\n\
  <dependency>\n\
    <dependentAssembly>\n\
      <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\" version=\"6.0.0.0\" processorArchitecture=\"*\" publicKeyToken=\"6595b64144ccf1df\" language=\"*\"/>\n\
    </dependentAssembly>\n\
  </dependency>\n\
  <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">\n\
    <security>\n\
      <requestedPrivileges>\n\
        <requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\"/>\n\
      </requestedPrivileges>\n\
    </security>\n\
  </trustInfo>\n\
  <compatibility xmlns=\"urn:schemas-microsoft-com:compatibility.v1\">\n\
    <application>\n\
      <supportedOS Id=\"{{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}}\"/>\n\
    </application>\n\
  </compatibility>\n\
  <application xmlns=\"urn:schemas-microsoft-com:asm.v3\">\n\
    <windowsSettings>\n\
{dpi_xml}\
    </windowsSettings>\n\
  </application>\n\
</assembly>\n"
    )
}

/// A path as an .rc string literal (forward slashes work on every host; quotes cannot occur in sane paths).
fn rc_path(p: &std::path::Path) -> String {
    p.to_string_lossy().replace('\\', "/").replace('"', "\"\"")
}

fn embed_windows_resources(r: &WinRes) {
    use std::env;
    use std::path::PathBuf;
    use std::process::Command;
    println!("cargo:rerun-if-changed={}", r.icon.display());
    if env::var("CARGO_CFG_TARGET_OS").as_deref() != Ok("windows") {
        return;
    }
    let link_arg = |a: &str| match r.bin {
        Some(b) => println!("cargo:rustc-link-arg-bin={b}={a}"),
        None => println!("cargo:rustc-link-arg-bins={a}"),
    };
    if env::var("CARGO_CFG_TARGET_ENV").as_deref() != Ok("msvc") {
        // rustc passes -no-pie to the clang linker driver of llvm-mingw, which ignores it and says so on every link
        // (rustc turns that into a warning); for every bin of the package
        println!("cargo:rustc-link-arg-bins=-Wno-unused-command-line-argument");
        if r.gui {
            link_arg("-Wl,--subsystem,windows");
        }
    } else if r.gui {
        link_arg("/SUBSYSTEM:WINDOWS");
        link_arg("/ENTRY:mainCRTStartup");
    }

    let out = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR"));
    let stem = r.bin.unwrap_or("app").to_string();
    let man = out.join(format!("{stem}.manifest"));
    std::fs::write(&man, manifest(r.dpi)).expect("write manifest");
    let ver = env::var("CARGO_PKG_VERSION").unwrap_or_else(|_| "0.0.0".into());
    let mut nums: Vec<u32> = ver.split(['.', '-', '+']).filter_map(|s| s.parse().ok()).take(3).collect();
    nums.resize(4, 0);
    let vnum = format!("{},{},{},{}", nums[0], nums[1], nums[2], nums[3]);
    let rc = format!(
        "1 ICON \"{icon}\"\n\
1 24 \"{man}\"\n\
1 VERSIONINFO\n\
FILEVERSION {vnum}\n\
PRODUCTVERSION {vnum}\n\
FILEFLAGSMASK 0x3F\n\
FILEFLAGS 0x0\n\
FILEOS 0x40004\n\
FILETYPE 0x1\n\
FILESUBTYPE 0x0\n\
BEGIN\n\
  BLOCK \"StringFileInfo\"\n\
  BEGIN\n\
    BLOCK \"040904B0\"\n\
    BEGIN\n\
      VALUE \"CompanyName\", \"TT LCD Studio\"\n\
      VALUE \"FileDescription\", \"{desc}\"\n\
      VALUE \"FileVersion\", \"{ver}\"\n\
      VALUE \"InternalName\", \"{file}\"\n\
      VALUE \"LegalCopyright\", \"MIT License\"\n\
      VALUE \"OriginalFilename\", \"{file}\"\n\
      VALUE \"ProductName\", \"TT LCD Studio\"\n\
      VALUE \"ProductVersion\", \"{ver}\"\n\
    END\n\
  END\n\
  BLOCK \"VarFileInfo\"\n\
  BEGIN\n\
    VALUE \"Translation\", 0x409, 1200\n\
  END\n\
END\n",
        icon = rc_path(r.icon),
        man = rc_path(&man),
        desc = r.description,
        file = r.file_name,
    );
    let rc_file = out.join(format!("{stem}.rc"));
    std::fs::write(&rc_file, rc).expect("write .rc");
    let obj = out.join(format!("{stem}-resources.o"));
    let triple = match env::var("CARGO_CFG_TARGET_ARCH").as_deref() {
        Ok("x86") => "i686",
        Ok("aarch64") => "aarch64",
        _ => "x86_64",
    };
    let tools = [format!("{triple}-w64-mingw32-windres"), "llvm-windres".to_string(), "windres".to_string()];
    let mut errors = Vec::new();
    for tool in &tools {
        let mut cmd = Command::new(tool);
        if tool != "windres" {
            cmd.arg("--no-preprocess"); // llvm-windres only; the .rc above has no #include / #define
        }
        if tool == "llvm-windres" {
            cmd.arg(format!(
                "--target={}",
                match triple {
                    "i686" => "pe-i386",
                    "aarch64" => "pe-aarch64-little",
                    _ => "pe-x86-64",
                }
            ));
        }
        cmd.args(["-O", "coff", "-i"]).arg(&rc_file).arg("-o").arg(&obj);
        match cmd.output() {
            Ok(o) if o.status.success() => {
                link_arg(&obj.to_string_lossy());
                return;
            }
            Ok(o) => errors.push(format!("{tool}: {}", String::from_utf8_lossy(&o.stderr).trim())),
            Err(e) => errors.push(format!("{tool}: {e}")),
        }
    }
    panic!("no resource compiler worked (the bundled llvm-mingw must be on PATH): {}", errors.join("; "));
}
