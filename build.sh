#!/bin/sh
# Builds the Linux version with Mono (Ubuntu: sudo apt install mono-devel) into build/linux/:
#   build/linux/pkg/                        ttlcd (core): usr/bin/ttlcd, usr/lib/ttlcd/ttlcd.exe, udev rule, systemd
#                                            user service
#   build/linux/pkg-studio/                 ttlcd-studio: usr/bin/ttlcd-studio, usr/lib/ttlcd/TTLcdStudio.exe, GTK#
#                                            3 (fetched, see below), desktop entry and icon
#   build/linux/ttlcd_<version>_all.deb         Debian/Ubuntu package (core)
#   build/linux/ttlcd-studio_<version>_all.deb  Debian/Ubuntu package (Studio)
#   build/linux/ttlcd-<version>-linux.tar.gz    both of the above, for other distributions (sudo tar -xzf ... -C /)
# ttlcd.exe is the command-line tool and screen service ('ttlcd run'); TTLcdStudio.exe is the GTK window (src/gtk).
# They are two packages so installing just ttlcd never needs GTK# at all.
#
# GTK# 3 (libgtk3.0-cil / gtk-sharp3): Debian dropped it in 2023 and Ubuntu's own copy is gone from 25.10 questing
# onward (still in 24.04 noble), so depending on the system package stopped working. Instead, fetch_gtksharp3()
# below downloads the last Ubuntu noble build straight from archive.ubuntu.com (pinned exact version + SHA256,
# cached in build/.gtksharp3-cache/ across runs) and ttlcd-studio bundles the assemblies and native glue libraries
# next to TTLcdStudio.exe, where Mono resolves both without touching the GAC or any system gtk-sharp3 package.
# That leaves only the native GTK3/GLib/Pango/Atk/Cairo libraries themselves as a runtime dependency - those
# aren't going anywhere (practically every GNOME/GTK desktop needs them) and are declared in linux/control-studio.
# Verified end to end on Ubuntu 26.04 (resolute), which has no gtk-sharp3 package at all: fetch, build, install,
# launch the real window - see the project history for that session's notes.
set -e
cd "$(dirname "$0")"
VER=$(sed -n 's/.*AssemblyVersion("\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' src/Version.cs)
list() { sed -n "s/^set $1=//p" build.cmd | tr -d '\r' | tr '\\' '/'; }
CORE=$(list CORE)
STUDIO=$(list STUDIO)
REFS="-r:System.Drawing.dll -r:System.Windows.Forms.dll -r:System.Xml.dll -r:System.Core.dll -r:System.ServiceProcess.dll -r:System.Management.dll"
OUT=build/linux
PKG=$PWD/$OUT/pkg
PKGS=$PWD/$OUT/pkg-studio
LIB=$PKG/usr/lib/ttlcd
LIBS=$PKGS/usr/lib/ttlcd

GTKSHARP_VER=2.99.3-4.1build2
GTKSHARP_BASE=http://archive.ubuntu.com/ubuntu/pool/universe/g/gtk-sharp3
GTKSHARP_CACHE=$PWD/build/.gtksharp3-cache
# package:sha256, as shipped in Ubuntu noble (24.04)
GTKSHARP_PKGS='
libglib3.0-cil:1047e5bedec198ab5d92cfbebff485a4ec2fd70cebe36182a6017a680525624d
libatk3.0-cil:32e10dae35d9f1f1b288b2fb6d5f86ff8ce80aef3d87be2472bddce8020a13ff
libcairo1.10-cil:a2d200027be1fa82b1f5712fe0753d86ac63062262e674e5b110ffd0d24dacaf
libgio3.0-cil:6b45b585b31caa10ff61cd5a946224cf4c101d34fc5cc1a008ee6f65f1085ff6
libpango3.0-cil:6c8d40cd058c7a8af2a9d28be7e6457e513a5b9bcba38b6523641d828f323497
libgdk3.0-cil:fe2d32544874646d09a9d2dc81a8fbc76c2491671beeffe27ac3ea992551942b
libgtk3.0-cil:d41128732b0520571b688fda60c2fbf6f9b87a17da9b525abad883f3d697ada9
'

# Downloads + verifies the gtk-sharp3 .deb files (cached after the first run) and lays out the flat,
# runtime-patched set of assemblies + native glue libraries any build can just copy into place.
fetch_gtksharp3() {
    flat=$GTKSHARP_CACHE/flat
    [ -f "$flat/.done" ] && return 0
    mkdir -p "$GTKSHARP_CACHE" "$flat"
    for entry in $GTKSHARP_PKGS; do
        [ -z "$entry" ] && continue
        pkg=${entry%%:*}
        sum=${entry#*:}
        deb="$GTKSHARP_CACHE/${pkg}_${GTKSHARP_VER}_amd64.deb"
        if [ ! -f "$deb" ] || ! echo "$sum  $deb" | sha256sum -c - >/dev/null 2>&1; then
            curl -fsSL -o "$deb" "$GTKSHARP_BASE/${pkg}_${GTKSHARP_VER}_amd64.deb"
        fi
        echo "$sum  $deb" | sha256sum -c - >/dev/null || { echo "gtk-sharp3 checksum mismatch: $deb" >&2; exit 1; }
        dpkg-deb -x "$deb" "$GTKSHARP_CACHE/extract"
    done
    find "$GTKSHARP_CACHE/extract/usr/lib/cli" -mindepth 2 -maxdepth 2 -type f -exec cp {} "$flat/" \;
    # the 4 native glue libraries are found by an absolute path baked into the matching *.dll.config; repoint it
    # at the layout every ttlcd-studio install uses (apt package or tarball), regardless of where this is built
    for g in atk gio pango gtk; do
        sed -i "s#/usr/lib/cli/[^/]*/lib${g}sharpglue-3\.so#/usr/lib/ttlcd/lib${g}sharpglue-3.so#" "$flat/${g}-sharp.dll.config"
    done
    touch "$flat/.done"
}

rm -rf "$OUT"
mkdir -p "$LIB" "$PKG/usr/bin" "$PKG/lib/udev/rules.d" "$PKG/usr/lib/systemd/user" "$PKG/DEBIAN"

echo "[1/5] ttlcd.exe"
mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.Program -out:"$LIB/ttlcd.exe" $REFS $CORE src/Program.cs

install -m 755 linux/ttlcd "$PKG/usr/bin/ttlcd"
install -m 644 linux/60-ttlcd.rules "$PKG/lib/udev/rules.d/60-ttlcd.rules"
install -m 644 linux/ttlcd.service "$PKG/usr/lib/systemd/user/ttlcd.service"
sed "s/@VERSION@/$VER/" linux/control > "$PKG/DEBIAN/control"
install -m 755 linux/postinst "$PKG/DEBIAN/postinst"

echo "[2/5] ttlcd_${VER}_all.deb"
dpkg-deb --root-owner-group --build "$PKG" "$OUT/ttlcd_${VER}_all.deb" >/dev/null

echo "[3/5] gtk-sharp3 (fetch + verify)"
fetch_gtksharp3
mkdir -p "$LIBS" "$PKGS/usr/bin" "$PKGS/usr/share/applications" "$PKGS/usr/share/icons/hicolor/128x128/apps" "$PKGS/DEBIAN"
cp "$GTKSHARP_CACHE"/flat/*.dll "$GTKSHARP_CACHE"/flat/*.dll.config "$GTKSHARP_CACHE"/flat/*.so "$LIBS/"

echo "[4/5] TTLcdStudio.exe, ttlcd-studio_${VER}_all.deb"
mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.GtkUi.GtkProgram -out:"$LIBS/TTLcdStudio.exe" $REFS \
    -r:"$LIBS/cairo-sharp.dll" -r:"$LIBS/pango-sharp.dll" -r:"$LIBS/atk-sharp.dll" -r:"$LIBS/gtk-sharp.dll" \
    -r:"$LIBS/gio-sharp.dll" -r:"$LIBS/gdk-sharp.dll" -r:"$LIBS/glib-sharp.dll" $CORE $STUDIO src/Gif.cs src/gtk/*.cs

install -m 755 linux/ttlcd-studio "$PKGS/usr/bin/ttlcd-studio"
install -m 644 linux/ttlcd-studio.desktop "$PKGS/usr/share/applications/ttlcd-studio.desktop"
install -m 644 linux/ttlcd-studio.png "$PKGS/usr/share/icons/hicolor/128x128/apps/ttlcd-studio.png"
# ttlcd-studio is self-contained (Provides/Conflicts/Replaces: ttlcd): one .deb a desktop installer (App Center,
# which installs a single file and can't resolve a dependency on a package that's in no repository) can install
# by itself. So it carries the ttlcd package's files, postinst and dependencies too.
cp -a "$PKG/usr" "$PKG/lib" "$PKGS/"
install -m 755 linux/postinst "$PKGS/DEBIAN/postinst"
CORE_DEPENDS=$(sed -n 's/^Depends: //p' linux/control)
sed -e "s/@VERSION@/$VER/g" -e "s/@CORE_DEPENDS@/$CORE_DEPENDS/" linux/control-studio > "$PKGS/DEBIAN/control"

dpkg-deb --root-owner-group --build "$PKGS" "$OUT/ttlcd-studio_${VER}_all.deb" >/dev/null

echo "[5/5] ttlcd-${VER}-linux.tar.gz"
tar -czf "$OUT/ttlcd-${VER}-linux.tar.gz" --owner=0 --group=0 -C "$PKGS" usr lib   # Studio's tree has everything
echo "build OK: $OUT"
