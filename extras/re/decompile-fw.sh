#!/bin/sh
# Decompiles the shipped LCD firmware (firmware/lx800_tt021panel.bin, LX800 image with the TT021 panel init) with
# Ghidra into firmware/re-src/ (code/*.c, functions.csv, strings.txt). Linux host. Uses only files in tools/:
# Ghidra 12.1.4 (tools/ghidra_12.1.4_PUBLIC) and a Linux JDK 21 (tools/jdk21-linux, Eclipse Temurin 21.0.12.1+1:
# OpenJDK21U-jdk_x64_linux_hotspot_21.0.12.1_1.tar.gz, SHA256
# ce79869e1307ed8ee1e2baa86a412b1eb5b75d10a01006d788a6f968bcfaee94, unpacked there).
#   extras/re/decompile-fw.sh
# Names come from firmware/re-src/names.txt; the Ghidra project goes to extras/re/proj (not in git).
set -e
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
G=$ROOT/tools/ghidra_12.1.4_PUBLIC
export JAVA_HOME=$ROOT/tools/jdk21-linux
export PATH="$JAVA_HOME/bin:$PATH"
[ -x "$JAVA_HOME/bin/java" ] || { echo "no JDK 21 in tools/jdk21-linux (see the comment at the top)" >&2; exit 1; }
OUT=$ROOT/firmware/re-src
mkdir -p "$OUT" "$ROOT/extras/re/proj" "$ROOT/extras/re/out"
# code ends below 0x130000 (file 0x30020); fonts and pictures follow
sh "$G/support/analyzeHeadless" "$ROOT/extras/re/proj" lx800full \
    -import "$ROOT/firmware/lx800_tt021panel.bin" -overwrite \
    -processor ARM:LE:32:v5t -loader BinaryLoader -loader-baseAddr 0xFFFE0 \
    -scriptPath "$ROOT/extras/re/scripts" \
    -postScript DecompileFirmware.java "$OUT" "$OUT/names.txt" 0x130000 \
    -log "$ROOT/extras/re/out/decompile-fw.log"
