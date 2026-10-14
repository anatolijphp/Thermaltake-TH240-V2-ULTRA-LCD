// The probe example (an exe) on llvm-mingw: rustc passes -no-pie to the clang linker driver, which ignores it and says
// so on every link (rustc turns that into a warning). The DLL itself needs nothing here.
fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    let windows = std::env::var("CARGO_CFG_TARGET_OS").as_deref() == Ok("windows");
    if windows && std::env::var("CARGO_CFG_TARGET_ENV").as_deref() != Ok("msvc") {
        println!("cargo:rustc-link-arg-examples=-Wno-unused-command-line-argument");
    }
}
