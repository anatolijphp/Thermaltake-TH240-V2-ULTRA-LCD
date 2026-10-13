#!/bin/sh
# Builds the Rust port with the bundled toolchain (rs/fetch.sh runs first if tools/ lacks it). Nothing is installed.
#   build/linux/rs/     ttlcd-rs, ttlcd-studio            (x86_64-unknown-linux-gnu)
#   build/windows/rs/   ttlcd-rs.exe, ttlcd-studio.exe    (x86_64-pc-windows-gnullvm, cross-linked with llvm-mingw)
# Needs on the build machine: a C linker (cc) and pkg-config, which link the Linux binaries against the desktop's
# own GTK 3 / WebKitGTK through the stubs in tools/linux-pc. Pass "linux" or "windows" to build only one.
set -e
cd "$(dirname "$0")"
ROOT=$(cd .. && pwd)
T=$ROOT/tools
[ -f "$T/rust/.done-1.98.0" ] && [ -f "$T/llvm-mingw/.done-20260922" ] && [ -d "$T/linux-pc" ] || sh ./fetch.sh
export PATH="$T/rust/bin:$T/llvm-mingw/bin:$PATH"
export CARGO_HOME="$T/cargo-home"
export PKG_CONFIG_LIBDIR="$T/linux-pc/pkgconfig" PKG_CONFIG_PATH=
WHAT=${1:-all}
if [ "$WHAT" = all ] || [ "$WHAT" = linux ]; then
    cargo build --release --locked --target x86_64-unknown-linux-gnu
    mkdir -p "$ROOT/build/linux/rs"
    cp "$ROOT/build/rs-target/x86_64-unknown-linux-gnu/release/ttlcd-rs" \
       "$ROOT/build/rs-target/x86_64-unknown-linux-gnu/release/ttlcd-studio" "$ROOT/build/linux/rs/"
fi
if [ "$WHAT" = all ] || [ "$WHAT" = windows ]; then
    cargo build --release --locked --target x86_64-pc-windows-gnullvm
    mkdir -p "$ROOT/build/windows/rs"
    cp "$ROOT/build/rs-target/x86_64-pc-windows-gnullvm/release/ttlcd-rs.exe" \
       "$ROOT/build/rs-target/x86_64-pc-windows-gnullvm/release/ttlcd-studio.exe" "$ROOT/build/windows/rs/"
    # Microsoft's redistributable WebView2 loader (shipped in the webview2-com-sys crate); the WebView2 runtime
    # itself comes with Windows 10/11
    cp "$(ls -d "$ROOT"/build/rs-target/x86_64-pc-windows-gnullvm/release/build/webview2-com-sys-*/out/x64 | head -n 1)/WebView2Loader.dll" \
       "$ROOT/build/windows/rs/"
fi
echo "build OK"
