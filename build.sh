#!/bin/sh
# Builds the Linux version with Mono (Ubuntu: sudo apt install mono-devel) into build/linux/:
#   build/linux/pkg/                       the installed layout (usr/bin/ttlcd, usr/lib/ttlcd/ttlcd.exe, udev rule,
#                                          systemd user service)
#   build/linux/ttlcd_<version>_all.deb    Debian/Ubuntu package
#   build/linux/ttlcd-<version>-linux.tar.gz  the same files for other distributions (sudo tar -xzf ... -C /)
# Linux gets the command-line tool and the screen service ('ttlcd run'); the Studio window is Windows-only.
# The shared sources are the ones build.cmd lists in CORE, so both builds always use the same files.
set -e
cd "$(dirname "$0")"
VER=$(sed -n 's/.*AssemblyVersion("\([0-9]*\.[0-9]*\.[0-9]*\).*/\1/p' src/Version.cs)
CORE=$(sed -n 's/^set CORE=//p' build.cmd | tr -d '\r' | tr '\' '/')
OUT=build/linux
PKG=$OUT/pkg
rm -rf "$OUT"
mkdir -p "$PKG/usr/lib/ttlcd" "$PKG/usr/bin" "$PKG/lib/udev/rules.d" "$PKG/usr/lib/systemd/user" "$PKG/DEBIAN"

echo "[1/3] ttlcd.exe"
mcs -nologo -optimize+ -unsafe -target:exe -main:TtLcd.Program -out:"$PKG/usr/lib/ttlcd/ttlcd.exe" \
    -r:System.Drawing.dll -r:System.Windows.Forms.dll -r:System.Xml.dll -r:System.Core.dll \
    -r:System.ServiceProcess.dll -r:System.Management.dll $CORE src/Program.cs

install -m 755 linux/ttlcd "$PKG/usr/bin/ttlcd"
install -m 644 linux/60-ttlcd.rules "$PKG/lib/udev/rules.d/60-ttlcd.rules"
install -m 644 linux/ttlcd.service "$PKG/usr/lib/systemd/user/ttlcd.service"
sed "s/@VERSION@/$VER/" linux/control > "$PKG/DEBIAN/control"
install -m 755 linux/postinst "$PKG/DEBIAN/postinst"

echo "[2/3] ttlcd_${VER}_all.deb"
dpkg-deb --root-owner-group --build "$PKG" "$OUT/ttlcd_${VER}_all.deb" >/dev/null

echo "[3/3] ttlcd-${VER}-linux.tar.gz"
tar -czf "$OUT/ttlcd-${VER}-linux.tar.gz" --owner=0 --group=0 -C "$PKG" usr lib
echo "build OK: $OUT"
