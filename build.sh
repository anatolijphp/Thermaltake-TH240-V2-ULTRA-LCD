#!/bin/sh
# Builds the Linux version with Mono (Ubuntu: sudo apt install mono-devel; add libgtk3.0-cil-dev too for the Studio
# package, see below) into build/linux/:
#   build/linux/pkg/                        ttlcd (core): usr/bin/ttlcd, usr/lib/ttlcd/ttlcd.exe, udev rule, systemd
#                                            user service
#   build/linux/pkg-studio/                 ttlcd-studio: usr/bin/ttlcd-studio, usr/lib/ttlcd/TTLcdStudio.exe,
#                                            desktop entry and icon. Built only if gtk-sharp-3.0 is installed.
#   build/linux/ttlcd_<version>_all.deb         Debian/Ubuntu package (core, no GTK dependency)
#   build/linux/ttlcd-studio_<version>_all.deb  Debian/Ubuntu package (Studio); absent if gtk-sharp-3.0 is missing
#   build/linux/ttlcd-<version>-linux.tar.gz    both of the above, for other distributions (sudo tar -xzf ... -C /)
# ttlcd.exe is the command-line tool and screen service ('ttlcd run'); TTLcdStudio.exe is the GTK window (src/gtk).
# They are two packages because GTK# 3 (libgtk3.0-cil) was dropped from Debian/Ubuntu (gone since Ubuntu 25.10
# questing; still in 24.04 noble) - ttlcd itself has no GTK dependency and keeps installing without it.
# The shared sources are the ones build.cmd lists in CORE and STUDIO, so both builds always use the same files.
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
rm -rf "$OUT"
mkdir -p "$LIB" "$PKG/usr/bin" "$PKG/lib/udev/rules.d" "$PKG/usr/lib/systemd/user" "$PKG/DEBIAN"

echo "[1/4] ttlcd.exe"
mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.Program -out:"$LIB/ttlcd.exe" $REFS $CORE src/Program.cs

install -m 755 linux/ttlcd "$PKG/usr/bin/ttlcd"
install -m 644 linux/60-ttlcd.rules "$PKG/lib/udev/rules.d/60-ttlcd.rules"
install -m 644 linux/ttlcd.service "$PKG/usr/lib/systemd/user/ttlcd.service"
sed "s/@VERSION@/$VER/" linux/control > "$PKG/DEBIAN/control"
install -m 755 linux/postinst "$PKG/DEBIAN/postinst"

echo "[2/4] ttlcd_${VER}_all.deb"
dpkg-deb --root-owner-group --build "$PKG" "$OUT/ttlcd_${VER}_all.deb" >/dev/null

if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists gtk-sharp-3.0 2>/dev/null; then
    LIBS=$PKGS/usr/lib/ttlcd
    mkdir -p "$LIBS" "$PKGS/usr/bin" "$PKGS/usr/share/applications" "$PKGS/usr/share/icons/hicolor/128x128/apps" "$PKGS/DEBIAN"

    echo "[3/4] TTLcdStudio.exe, ttlcd-studio_${VER}_all.deb"
    mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.GtkUi.GtkProgram -out:"$LIBS/TTLcdStudio.exe" $REFS \
        -pkg:gtk-sharp-3.0 $CORE $STUDIO src/gtk/*.cs

    install -m 755 linux/ttlcd-studio "$PKGS/usr/bin/ttlcd-studio"
    install -m 644 linux/ttlcd-studio.desktop "$PKGS/usr/share/applications/ttlcd-studio.desktop"
    install -m 644 linux/ttlcd-studio.png "$PKGS/usr/share/icons/hicolor/128x128/apps/ttlcd-studio.png"
    sed "s/@VERSION@/$VER/" linux/control-studio > "$PKGS/DEBIAN/control"

    dpkg-deb --root-owner-group --build "$PKGS" "$OUT/ttlcd-studio_${VER}_all.deb" >/dev/null
else
    echo "[3/4] gtk-sharp-3.0 not found (install libgtk3.0-cil-dev) - skipping TTLcdStudio.exe and ttlcd-studio" >&2
fi

echo "[4/4] ttlcd-${VER}-linux.tar.gz"
if [ -d "$PKGS/usr" ]; then
    tar -czf "$OUT/ttlcd-${VER}-linux.tar.gz" --owner=0 --group=0 -C "$PKG" usr lib -C "$PKGS" usr
else
    tar -czf "$OUT/ttlcd-${VER}-linux.tar.gz" --owner=0 --group=0 -C "$PKG" usr lib
fi
echo "build OK: $OUT"
