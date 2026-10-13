#!/bin/sh
# Fetches the Rust build toolchain into tools/ (Linux host). Nothing is installed: no rustup, no sudo, no PATH change.
# Every download is pinned by version + SHA256 and kept in tools/dl/rs/, so re-running only checks hashes.
#   tools/rust/          rustc + cargo 1.98.0, std for Linux and for Windows (x86_64-pc-windows-gnullvm)
#   tools/llvm-mingw/    clang + lld + mingw-w64 (links the Windows build; no Visual Studio needed)
#   tools/linux-pc/      pkg-config stubs for the system GTK 3 / WebKitGTK libraries, so no -dev packages are needed
#   tools/cargo-home/    CARGO_HOME: crates.io downloads (pinned by Cargo.lock checksums)
# Windows host: rs\fetch.cmd does the same with the Windows-hosted archives.
set -e
cd "$(dirname "$0")/.."
T=$PWD/tools
DL=$T/dl/rs
mkdir -p "$DL"

RUST=https://static.rust-lang.org/dist/2026-08-20
MINGW=https://github.com/mstorsjo/llvm-mingw/releases/download/20260922
# url sha256
PINS="
$RUST/rustc-1.98.0-x86_64-unknown-linux-gnu.tar.xz 0e37cb339f447fc44d6d781073bacacebfdc5612f2600e4c7e84c266f5f3aced
$RUST/cargo-1.98.0-x86_64-unknown-linux-gnu.tar.xz 2f512d170d3dd23e16ababcda32ee2e6d5172d861a7af1f504e0b1e270cafab9
$RUST/rust-std-1.98.0-x86_64-unknown-linux-gnu.tar.xz f5022e6c95a5ad23cca2513dc8281200f585fa188de6370aa37b128a43f876a3
$RUST/rust-std-1.98.0-x86_64-pc-windows-gnullvm.tar.xz 9e7a4a69893cd2dff7211fbaa63f909eb13dd65b0f2d7400c1bb3ee005f9ec16
$MINGW/llvm-mingw-20260922-ucrt-ubuntu-22.04-x86_64.tar.xz bb7bb7654b33d5aa8712acb837c963b2e0c56352560c76105270a3268c665c21
"

fetch() { # url sha -> path of the verified archive
    f=$DL/$(basename "$1")
    if [ ! -f "$f" ] || ! echo "$2  $f" | sha256sum -c - >/dev/null 2>&1; then
        echo "  download $(basename "$1")" >&2
        curl -fL --retry 3 -s -o "$f.part" "$1" && mv "$f.part" "$f"
    fi
    echo "$2  $f" | sha256sum -c - >/dev/null || { echo "checksum mismatch: $f" >&2; exit 1; }
    echo "$f"
}

# A Rust dist tarball holds <name>/<component>/{bin,lib,...} for each component listed in <name>/components.
# Merging those trees into one prefix is all its install.sh does (apart from manifests we don't need).
if [ ! -f "$T/rust/.done-1.98.0" ]; then
    rm -rf "$T/rust" "$DL/x"
    mkdir -p "$T/rust"
    echo "$PINS" | grep -E 'rustc-|cargo-|rust-std-' | while read url sha; do
        a=$(fetch "$url" "$sha")
        mkdir -p "$DL/x" && tar -xJf "$a" -C "$DL/x"
        d=$DL/x/$(basename "$a" .tar.xz)
        for c in $(cat "$d/components"); do cp -a "$d/$c/." "$T/rust/"; done
        rm -rf "$DL/x"
    done
    touch "$T/rust/.done-1.98.0"
fi

if [ ! -f "$T/llvm-mingw/.done-20260922" ]; then
    rm -rf "$T/llvm-mingw"
    set -- $(echo "$PINS" | grep llvm-mingw)
    a=$(fetch "$1" "$2")
    mkdir -p "$T/llvm-mingw" && tar -xJf "$a" -C "$T/llvm-mingw" --strip-components=1
    touch "$T/llvm-mingw/.done-20260922"
fi

# pkg-config stubs: the gtk-rs/webkit2gtk crates only ask pkg-config for link flags (their bindings are pregenerated,
# no C headers). Each stub points at a lib<name>.so symlink to the runtime library the desktop already has, with the
# version dpkg reports for it (the crates check minimum versions).
PC=$T/linux-pc
rm -rf "$PC" && mkdir -p "$PC/pkgconfig" "$PC/lib"
SYS=/usr/lib/x86_64-linux-gnu
# module lib
for m in "glib-2.0 glib-2.0" "gobject-2.0 gobject-2.0" "gio-2.0 gio-2.0" "gmodule-2.0 gmodule-2.0" \
         "gtk+-3.0 gtk-3" "gdk-3.0 gdk-3" "gdk-x11-3.0 gdk-3" "atk atk-1.0" "pango pango-1.0" \
         "pangocairo pangocairo-1.0" "cairo cairo" "cairo-gobject cairo-gobject" "gdk-pixbuf-2.0 gdk_pixbuf-2.0" \
         "javascriptcoregtk-4.1 javascriptcoregtk-4.1" "webkit2gtk-4.1 webkit2gtk-4.1" "libsoup-3.0 soup-3.0"; do
    set -- $m
    so=$(ls "$SYS/lib$2.so."* 2>/dev/null | sort | head -n 1)
    [ -n "$so" ] || { echo "  missing runtime library lib$2.so (skipping $1)" >&2; continue; }
    ln -sf "$so" "$PC/lib/lib$2.so"
    pkg=$(dpkg -S "$so" 2>/dev/null | head -n 1 | cut -d: -f1)
    ver=$(dpkg-query -W -f '${Version}' "$pkg" 2>/dev/null | sed 's/^[0-9]*://; s/[-+~].*//')
    printf 'Name: %s\nDescription: stub for %s\nVersion: %s\nLibs: -L%s -l%s\nCflags:\n' \
        "$1" "$so" "${ver:-999}" "$PC/lib" "$2" > "$PC/pkgconfig/$1.pc"
done

mkdir -p "$T/cargo-home"
echo "toolchain ready: $("$T/rust/bin/rustc" --version), llvm-mingw 20260922"
