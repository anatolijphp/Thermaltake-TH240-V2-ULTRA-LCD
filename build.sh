#!/bin/sh
# Builds the Linux version with Mono and GTK# 3 (Ubuntu: sudo apt install mono-devel libgtk3.0-cil-dev) into build/linux/:
#   build/linux/pkg/                       the installed layout: usr/bin/ttlcd and ttlcd-studio, usr/lib/ttlcd/*.exe,
#                                          udev rule, systemd user service, desktop entry and icon
#   build/linux/ttlcd_<version>_all.deb    Debian/Ubuntu package
#   build/linux/ttlcd-<version>-linux.tar.gz  the same files for other distributions (sudo tar -xzf ... -C /)
# ttlcd.exe is the command-line tool and screen service ('ttlcd run'); TTLcdStudio.exe is the GTK window (src/gtk).
# The shared sources are the ones build.cmd lists in CORE and STUDIO, so both builds always use the same files.
set -e
cd "$(dirname "$0")"
VER=$(sed -n 's/.*AssemblyVersion("\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' src/Version.cs)
list() { sed -n "s/^set $1=//p" build.cmd | tr -d '\r' | tr '\\' '/'; }
CORE=$(list CORE)
STUDIO=$(list STUDIO)
REFS="-r:System.Drawing.dll -r:System.Windows.Forms.dll -r:System.Xml.dll -r:System.Core.dll -r:System.ServiceProcess.dll -r:System.Management.dll"
OUT=build/linux
PKG=$OUT/pkg
LIB=$PKG/usr/lib/ttlcd
rm -rf "$OUT"
mkdir -p "$LIB" "$PKG/usr/bin" "$PKG/lib/udev/rules.d" "$PKG/usr/lib/systemd/user" "$PKG/DEBIAN" \
         "$PKG/usr/share/applications" "$PKG/usr/share/icons/hicolor/128x128/apps"

echo "[1/4] ttlcd.exe"
mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.Program -out:"$LIB/ttlcd.exe" $REFS $CORE src/Program.cs

echo "[2/4] TTLcdStudio.exe"
mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.GtkUi.GtkProgram -out:"$LIB/TTLcdStudio.exe" $REFS \
    -pkg:gtk-sharp-3.0 $CORE $STUDIO src/gtk/*.cs

install -m 755 linux/ttlcd "$PKG/usr/bin/ttlcd"
install -m 755 linux/ttlcd-studio "$PKG/usr/bin/ttlcd-studio"
install -m 644 linux/60-ttlcd.rules "$PKG/lib/udev/rules.d/60-ttlcd.rules"
install -m 644 linux/ttlcd.service "$PKG/usr/lib/systemd/user/ttlcd.service"
install -m 644 linux/ttlcd-studio.desktop "$PKG/usr/share/applications/ttlcd-studio.desktop"
install -m 644 linux/ttlcd-studio.png "$PKG/usr/share/icons/hicolor/128x128/apps/ttlcd-studio.png"
sed "s/@VERSION@/$VER/" linux/control > "$PKG/DEBIAN/control"
install -m 755 linux/postinst "$PKG/DEBIAN/postinst"

echo "[3/4] ttlcd_${VER}_all.deb"
dpkg-deb --root-owner-group --build "$PKG" "$OUT/ttlcd_${VER}_all.deb" >/dev/null

echo "[4/4] ttlcd-${VER}-linux.tar.gz"
tar -czf "$OUT/ttlcd-${VER}-linux.tar.gz" --owner=0 --group=0 -C "$PKG" usr lib
echo "build OK: $OUT"
