#!/bin/sh
# Builds TT LCD (Rust, src/) with the bundled toolchain from src/fetch.sh; nothing is installed. Linux host; the
# Windows build is cross-linked with llvm-mingw. ./build.sh at the top of the repo runs this.
#   ./build.sh [all|linux|windows]       (default all)
#   ./build.sh run                       only the Linux binaries in build/target (no packages): what ../run.sh starts
# Output (sources stay in src/; intermediate files in build/target/):
#   build/linux/ttlcd_<ver>_amd64.deb            CLI + screen service: /usr/bin/ttlcd, systemd user unit, udev rule
#   build/linux/ttlcd-studio_<ver>_amd64.deb     all of the above + /usr/bin/ttlcd-studio, desktop entry, icon
#                                                (self-contained: Provides/Conflicts/Replaces ttlcd)
#   build/linux/ttlcd-<ver>-linux-x86_64.tar.gz  Studio's tree, for other distributions (sudo tar -xzf ... -C /)
#   build/windows/app/                           the Windows app laid out as installed (runnable from there); its
#                                                firmware\ttrescue.exe + PatchPanel.exe are the C# tools from
#                                                extras/rescue, compiled with Mono's mcs when it's there (build.cmd
#                                                on Windows uses the csc that ships with Windows)
#   build/windows/TTLcdStudio-Setup.exe          installer stub + build/windows/app as payload
# Build machine needs: curl, tar, xz, sha256sum, dpkg (dpkg-deb, dpkg-query) and the runtime GTK 3 / WebKitGTK 4.1
# libraries (any Ubuntu desktop has them). A C compiler and pkg-config are optional: without cc, fetch.sh brings Zig
# as the linker, and tools/linux-pc/pkg-config stands in for pkg-config.
set -e
cd "$(dirname "$0")"
RS=$PWD
ROOT=$(cd .. && pwd)
T=$ROOT/tools
WHAT=${1:-all}
case $WHAT in all|linux|windows|run) ;; *) echo "usage: $0 [all|linux|windows|run]" >&2; exit 2 ;; esac

need_zig() { ! command -v cc >/dev/null 2>&1 || [ "${TTLCD_ZIG:-}" = 1 ]; }
if [ ! -f "$T/rust/.done-1.98.0" ] || [ ! -f "$T/rust/.done-rust-std-1.98.0-i686-pc-windows-gnullvm" ] ||
   [ ! -f "$T/llvm-mingw/.done-20260922" ] || [ ! -x "$T/linux-pc/pkg-config" ] || [ ! -f "$T/node/.done-24.21.0" ] ||
   { need_zig && [ ! -f "$T/zig/.done-0.16.0" ]; } ||
   [ ! -e "$T/rust/bin/rustc" ] || [ ! -e "$T/llvm-mingw/bin/clang" ] || [ ! -e "$T/node/bin/node" ]; then
    sh ./fetch.sh
fi
export PATH="$T/rust/bin:$T/llvm-mingw/bin:$PATH"
export CARGO_HOME="$T/cargo-home"
export PKG_CONFIG="$T/linux-pc/pkg-config" PKG_CONFIG_LIBDIR="$T/linux-pc/pkgconfig" PKG_CONFIG_PATH=
if need_zig; then
    echo "linker: zig cc (tools/zig)"
    export CARGO_TARGET_X86_64_UNKNOWN_LINUX_GNU_LINKER="$T/zig/cc-linux"
fi
unset CARGO_TARGET_DIR   # src/.cargo/config.toml: build/target
TGT=$ROOT/build/target

if [ "$WHAT" = run ]; then
    # cargo rebuilds only what changed, so this is quick when the build is current
    cargo build --release --locked --target x86_64-unknown-linux-gnu -p ttlcd-cli -p ttlcd-studio
    exit 0
fi
VER=$(sed -n '/^\[workspace.package\]/,/^\[/s/^version *= *"\(.*\)"/\1/p' Cargo.toml)
[ -n "$VER" ] || { echo "no [workspace.package] version in src/Cargo.toml" >&2; exit 1; }
echo "TT LCD $VER"

# extras/rescue (C#, unchanged): ttrescue.exe and PatchPanel.exe for the Windows firmware\ folder, x86 like before.
# PatchPanel must turn firmware/make_fix.bin into exactly the shipped firmware/lx800_tt021panel.bin.
build_rescue() { # output dir
    if ! command -v mcs >/dev/null 2>&1; then
        echo "  skipped: no Mono mcs to compile extras/rescue (build.cmd on Windows compiles it with csc)" >&2
        return 0
    fi
    mcs -nologo -optimize+ -platform:x86 -target:exe -out:"$1/ttrescue.exe" -r:System.Core.dll "$ROOT/extras/rescue/Rescue.cs"
    mcs -nologo -optimize+ -platform:x86 -target:exe -out:"$1/PatchPanel.exe" "$ROOT/extras/rescue/PatchPanel.cs"
    command -v mono >/dev/null 2>&1 || return 0
    out=$TGT/lx800_tt021panel.check.bin
    mono "$1/PatchPanel.exe" "$ROOT/firmware/make_fix.bin" "$out" >/dev/null
    want=$(sed -n 's/  lx800_tt021panel.bin$//p' "$ROOT/firmware/SHA256SUMS.txt")
    got=$(sha256sum "$out" | cut -d' ' -f1)
    rm -f "$out"
    [ "$got" = "$want" ] || { echo "PatchPanel output differs from firmware/lx800_tt021panel.bin" >&2; exit 1; }
    echo "  PatchPanel: rebuilds lx800_tt021panel.bin byte for byte"
}

# Debian package name for each shared library the binaries link (readelf NEEDED). Alternatives cover the
# Ubuntu 24.04 time_t64 renames. An unknown library stops the build, so a new dependency is never left out.
pkg_for() {
    case $1 in
        libc.so.6|libm.so.6|libdl.so.2|librt.so.1|libpthread.so.0|ld-linux-x86-64.so.2) echo "libc6" ;;
        libgcc_s.so.1) echo "libgcc-s1" ;;
        libgtk-3.so.0|libgdk-3.so.0) echo "libgtk-3-0 | libgtk-3-0t64" ;;
        libglib-2.0.so.0|libgobject-2.0.so.0|libgio-2.0.so.0|libgmodule-2.0.so.0) echo "libglib2.0-0 | libglib2.0-0t64" ;;
        libgdk_pixbuf-2.0.so.0) echo "libgdk-pixbuf-2.0-0 | libgdk-pixbuf2.0-0" ;;
        libcairo.so.2) echo "libcairo2" ;;
        libcairo-gobject.so.2) echo "libcairo-gobject2" ;;
        libpango-1.0.so.0) echo "libpango-1.0-0" ;;
        libpangocairo-1.0.so.0) echo "libpangocairo-1.0-0" ;;
        libatk-1.0.so.0) echo "libatk1.0-0 | libatk1.0-0t64" ;;
        libwebkit2gtk-4.1.so.0) echo "libwebkit2gtk-4.1-0" ;;
        libjavascriptcoregtk-4.1.so.0) echo "libjavascriptcoregtk-4.1-0" ;;
        libsoup-3.0.so.0) echo "libsoup-3.0-0" ;;
        libX11.so.6) echo "libx11-6" ;;
        libxdo.so.3) echo "libxdo3" ;;
        *) echo "build.sh: no Debian package known for $1 (add it to pkg_for)" >&2; return 1 ;;
    esac
}
depends() { # binaries... -> "libc6, a, b | c, ..."
    list=libc6
    for l in $(for b; do readelf -d "$b" | sed -n 's/.*(NEEDED).*\[\(.*\)\]/\1/p'; done | sort -u); do
        p=$(pkg_for "$l") || exit 1
        list="$list
$p"
    done
    echo "$list" | awk '!seen[$0]++' | paste -sd, - | sed 's/,/, /g'
}

# directories 755 and control 644 whatever the umask or the file system (an NTFS mount reports 775)
fix_modes() { find "$1" -type d -exec chmod 755 {} +; chmod 644 "$1/DEBIAN/control"; }

build_linux() {
    L=x86_64-unknown-linux-gnu
    echo "[linux] cargo build"
    cargo build --release --locked --target $L -p ttlcd-cli -p ttlcd-studio
    R=$TGT/$L/release

    OUT=$ROOT/build/linux
    PKG=$OUT/pkg
    PKGS=$OUT/pkg-studio
    rm -rf "$OUT" && mkdir -p "$OUT"

    # ttlcd: CLI + screen service
    mkdir -p "$PKG/usr/bin" "$PKG/lib/udev/rules.d" "$PKG/usr/lib/systemd/user" "$PKG/DEBIAN"
    install -m 755 "$R/ttlcd" "$PKG/usr/bin/ttlcd"
    install -m 644 "$ROOT/linux/60-ttlcd.rules" "$PKG/lib/udev/rules.d/60-ttlcd.rules"
    install -m 644 "$ROOT/linux/ttlcd.service" "$PKG/usr/lib/systemd/user/ttlcd.service"
    install -m 755 "$ROOT/linux/postinst" "$PKG/DEBIAN/postinst"
    CORE_DEPENDS="$(depends "$R/ttlcd"), fonts-dejavu-core"
    sed -e "s/@VERSION@/$VER/" -e "s/@DEPENDS@/$CORE_DEPENDS/" \
        -e "s/@SIZE@/$(du -sk --exclude=DEBIAN "$PKG" | cut -f1)/" "$ROOT/linux/control" > "$PKG/DEBIAN/control"
    echo "[linux] ttlcd_${VER}_amd64.deb   Depends: $CORE_DEPENDS"
    fix_modes "$PKG"
    dpkg-deb -Zxz --root-owner-group --build "$PKG" "$OUT/ttlcd_${VER}_amd64.deb" >/dev/null

    # ttlcd-studio is self-contained (Provides/Conflicts/Replaces: ttlcd): the Ubuntu App Center installs a single
    # local .deb and can't resolve a dependency on a package that's in no repository. So it carries everything in
    # the ttlcd package too, and its dependencies.
    mkdir -p "$PKGS/DEBIAN"
    cp -a "$PKG/usr" "$PKG/lib" "$PKGS/"
    mkdir -p "$PKGS/usr/share/applications" "$PKGS/usr/share/icons/hicolor/128x128/apps"
    install -m 755 "$R/ttlcd-studio" "$PKGS/usr/bin/ttlcd-studio"
    install -m 644 "$ROOT/linux/ttlcd-studio.desktop" "$PKGS/usr/share/applications/ttlcd-studio.desktop"
    install -m 644 "$ROOT/linux/ttlcd-studio.png" "$PKGS/usr/share/icons/hicolor/128x128/apps/ttlcd-studio.png"
    install -m 755 "$ROOT/linux/postinst" "$PKGS/DEBIAN/postinst"
    SD=$(depends "$R/ttlcd-studio")
    STUDIO_DEPENDS=$(printf '%s, %s' "$CORE_DEPENDS" "$SD" | sed 's/, /\n/g' | awk '!seen[$0]++' | paste -sd, - |
                     sed 's/,/, /g')
    sed -e "s/@VERSION@/$VER/g" -e "s/@DEPENDS@/$STUDIO_DEPENDS/" \
        -e "s/@SIZE@/$(du -sk --exclude=DEBIAN "$PKGS" | cut -f1)/" "$ROOT/linux/control-studio" > "$PKGS/DEBIAN/control"
    echo "[linux] ttlcd-studio_${VER}_amd64.deb   Depends: $STUDIO_DEPENDS"
    fix_modes "$PKGS"
    dpkg-deb -Zxz --root-owner-group --build "$PKGS" "$OUT/ttlcd-studio_${VER}_amd64.deb" >/dev/null

    echo "[linux] ttlcd-${VER}-linux-x86_64.tar.gz"
    tar -czf "$OUT/ttlcd-${VER}-linux-x86_64.tar.gz" --owner=0 --group=0 -C "$PKGS" usr lib
}

build_windows() {
    W=x86_64-pc-windows-gnullvm
    W32=i686-pc-windows-gnullvm
    echo "[windows] cargo build (x86_64)"
    cargo build --release --locked --target $W -p ttlcd-cli -p ttlcd-studio -p ttlcd-aura-hal -p ttlcd-setup
    echo "[windows] cargo build (i686 Aura plugin: ASUS LightingService is a 32-bit process)"
    cargo build --release --locked --target $W32 -p ttlcd-aura-hal
    R=$TGT/$W/release
    R32=$TGT/$W32/release

    OUT=$ROOT/build/windows
    APP=$OUT/app
    rm -rf "$APP" && mkdir -p "$APP/driver" "$APP/firmware"
    cp "$R/ttlcd.exe" "$APP/ttlcd.exe"
    cp "$R/ttlcd-studio.exe" "$APP/TTLcdStudio.exe"
    cp "$R32/TTLcdAura.dll" "$APP/TTLcdAura.dll"
    cp "$R/TTLcdAura.dll" "$APP/TTLcdAura64.dll"
    # Studio carries its own WebView2 loader; it must not need Microsoft's WebView2Loader.dll next to it
    if llvm-objdump -p "$APP/TTLcdStudio.exe" | grep -qi "DLL Name: WebView2Loader.dll"; then
        echo "TTLcdStudio.exe imports WebView2Loader.dll; the app ships without it" >&2; exit 1
    fi
    echo "[windows] extras/rescue (C#)"
    build_rescue "$APP/firmware"
    for f in MttVDD.inf MttVDD.dll mttvdd.cat; do cp "$ROOT/driver/$f" "$APP/driver/"; done
    for f in README.md lx800_tt021panel.bin FA93_musb.bin hardware_boot_pins.jpg SHA256SUMS.txt; do
        cp "$ROOT/firmware/$f" "$APP/firmware/"
    done

    echo "[windows] TTLcdStudio-Setup.exe"
    rm -f "$OUT/TTLcdStudio-Setup.exe"
    if cargo metadata --no-deps --format-version 1 | grep -q '"name":"ttlcd-pack"'; then
        cargo run --release --locked -q -p ttlcd-setup --bin ttlcd-pack -- \
            "$R/TTLcdStudio-Setup.exe" "$APP" "$OUT/TTLcdStudio-Setup.exe"
    else
        echo "  skipped: the installer packer (ttlcd-setup bin ttlcd-pack) doesn't exist yet" >&2
    fi
}

case $WHAT in all|linux) build_linux ;; esac
case $WHAT in all|windows) build_windows ;; esac
echo "build OK: $VER ($WHAT)"
