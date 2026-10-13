#!/bin/sh
# Fetches the Rust build toolchain into tools/ (Linux host). Nothing is installed: no rustup, no sudo, no PATH change.
# Every download is pinned by version + SHA256 and kept in tools/dl/rs/, so re-running only checks hashes.
#   tools/rust/          rustc + cargo 1.98.0, std for Linux and for Windows (x86_64 and i686 -pc-windows-gnullvm)
#   tools/llvm-mingw/    clang + lld + mingw-w64 (links the Windows build; no Visual Studio needed)
#   tools/linux-pc/      pkg-config stubs for the system GTK 3 / WebKitGTK libraries (no -dev packages needed) and
#                        a small pkg-config that reads them (no pkg-config package needed)
#   tools/zig/           Zig 0.16.0 as the Linux C linker ("zig cc"), only when the machine has no cc (or TTLCD_ZIG=1)
#   tools/cargo-home/    CARGO_HOME: crates.io downloads (pinned by Cargo.lock checksums)
# Windows host: rs\fetch.cmd does the same with the Windows-hosted archives.
set -e
cd "$(dirname "$0")/.."
T=$PWD/tools
DL=$T/dl/rs
mkdir -p "$DL"

RUST=https://static.rust-lang.org/dist/2026-08-20
MINGW=https://github.com/mstorsjo/llvm-mingw/releases/download/20260922
ZIG=https://ziglang.org/download/0.16.0
# url sha256
PINS="
$RUST/rustc-1.98.0-x86_64-unknown-linux-gnu.tar.xz 0e37cb339f447fc44d6d781073bacacebfdc5612f2600e4c7e84c266f5f3aced
$RUST/cargo-1.98.0-x86_64-unknown-linux-gnu.tar.xz 2f512d170d3dd23e16ababcda32ee2e6d5172d861a7af1f504e0b1e270cafab9
$RUST/rust-std-1.98.0-x86_64-unknown-linux-gnu.tar.xz f5022e6c95a5ad23cca2513dc8281200f585fa188de6370aa37b128a43f876a3
$RUST/rust-std-1.98.0-x86_64-pc-windows-gnullvm.tar.xz 9e7a4a69893cd2dff7211fbaa63f909eb13dd65b0f2d7400c1bb3ee005f9ec16
$RUST/rust-std-1.98.0-i686-pc-windows-gnullvm.tar.xz c5796b30bacafb54e6343bf89653c167f558c1f9b840a04996565c8219d17c27
$MINGW/llvm-mingw-20260922-ucrt-ubuntu-22.04-x86_64.tar.xz bb7bb7654b33d5aa8712acb837c963b2e0c56352560c76105270a3268c665c21
$ZIG/zig-x86_64-linux-0.16.0.tar.xz 70e49664a74374b48b51e6f3fdfbf437f6395d42509050588bd49abe52ba3d00
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
# Each merged archive leaves tools/rust/.done-<archive>, so a newly pinned component (the i686 std) is added to an
# existing tools/rust without fetching the rest again.
merge_rust() { # pattern
    echo "$PINS" | grep -E "$1" | while read -r url sha; do
        n=$(basename "$url" .tar.xz)
        [ -f "$T/rust/.done-$n" ] && continue
        a=$(fetch "$url" "$sha")
        rm -rf "$DL/x" && mkdir -p "$DL/x" && tar -xJf "$a" -C "$DL/x"
        for c in $(cat "$DL/x/$n/components"); do cp -a "$DL/x/$n/$c/." "$T/rust/"; done
        rm -rf "$DL/x"
        touch "$T/rust/.done-$n"
    done
}
if [ ! -f "$T/rust/.done-1.98.0" ]; then
    rm -rf "$T/rust" && mkdir -p "$T/rust"
    merge_rust 'rustc-|cargo-|rust-std-1.98.0-x86_64'
    touch "$T/rust/.done-1.98.0"
fi
merge_rust 'rust-std-1.98.0-i686-pc-windows-gnullvm'

if [ ! -f "$T/llvm-mingw/.done-20260922" ]; then
    rm -rf "$T/llvm-mingw"
    set -- $(echo "$PINS" | grep llvm-mingw)
    a=$(fetch "$1" "$2")
    mkdir -p "$T/llvm-mingw" && tar -xJf "$a" -C "$T/llvm-mingw" --strip-components=1
    touch "$T/llvm-mingw/.done-20260922"
fi

# Zig: the Linux binaries need a C linker (rustc drives "cc"). Ubuntu desktops have no gcc by default, so without
# one, link with "zig cc" against Zig's own glibc stubs (glibc 2.35 = Ubuntu 22.04 symbol versions). build.sh picks
# tools/zig/cc-linux when it exists and the machine has no cc (or TTLCD_ZIG=1).
if { ! command -v cc >/dev/null 2>&1 || [ "${TTLCD_ZIG:-}" = 1 ]; } && [ ! -f "$T/zig/.done-0.16.0" ]; then
    rm -rf "$T/zig"
    set -- $(echo "$PINS" | grep zig-)
    a=$(fetch "$1" "$2")
    mkdir -p "$T/zig" && tar -xJf "$a" -C "$T/zig" --strip-components=1
    cat > "$T/zig/cc-linux" <<'EOF'
#!/bin/sh
# C linker for the Linux build when the machine has no cc (rs/fetch.sh writes this): zig cc for glibc 2.35.
Z=$(cd "$(dirname "$0")" && pwd)
export ZIG_GLOBAL_CACHE_DIR="$Z/cache" ZIG_LOCAL_CACHE_DIR="$Z/cache"
# zig links its own libunwind statically instead of the GCC runtime
for a; do shift; case $a in -lgcc_s) set -- "$@" -lunwind ;; *) set -- "$@" "$a" ;; esac; done
exec "$Z/zig" cc -target x86_64-linux-gnu.2.35 "$@"
EOF
    chmod +x "$T/zig/cc-linux"
    touch "$T/zig/.done-0.16.0"
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
# The pkg-config crate runs $PKG_CONFIG with --libs/--cflags/--modversion and "name >= version" arguments; this
# answers exactly that from the stubs above, so the pkg-config package isn't needed either.
cat > "$PC/pkg-config" <<'EOF'
#!/bin/sh
# Minimal pkg-config over the stub .pc files next to it (rs/fetch.sh writes both).
D=$(dirname "$0")/pkgconfig
libs= cflags= modver= mods=
for a; do
    case $a in
        --libs) libs=1 ;;
        --cflags) cflags=1 ;;
        --modversion) modver=1 ;;
        --version) echo 0.29.2; exit 0 ;;
        -*) ;;
        *) mods="$mods$a
" ;;
    esac
done
out=
while read -r name op want; do
    [ -n "$name" ] || continue
    pc=$D/$name.pc
    [ -f "$pc" ] || { echo "Package $name was not found in the pkg-config search path." >&2; exit 1; }
    have=$(sed -n 's/^Version: *//p' "$pc")
    if [ -n "$op" ]; then
        low=$(printf '%s\n%s\n' "$want" "$have" | sort -V | head -n 1)
        case $op in
            '>=') [ "$low" = "$want" ] ;;
            '>') [ "$low" = "$want" ] && [ "$have" != "$want" ] ;;
            '<=') [ "$low" = "$have" ] ;;
            '<') [ "$low" = "$have" ] && [ "$have" != "$want" ] ;;
            *) [ "$have" = "$want" ] ;;
        esac || { echo "Requested '$name $op $want' but version of $name is $have" >&2; exit 1; }
        continue
    fi
    [ -z "$modver" ] || echo "$have"
    [ -z "$libs" ] || out="$out $(sed -n 's/^Libs: *//p' "$pc")"
    [ -z "$cflags" ] || out="$out $(sed -n 's/^Cflags: *//p' "$pc")"
done <<EOF2
$mods
EOF2
[ -z "$libs$cflags" ] || echo $out
EOF
chmod +x "$PC/pkg-config"

mkdir -p "$T/cargo-home"
echo "toolchain ready: $("$T/rust/bin/rustc" --version), llvm-mingw 20260922$([ -f "$T/zig/.done-0.16.0" ] && echo ", zig 0.16.0")"
