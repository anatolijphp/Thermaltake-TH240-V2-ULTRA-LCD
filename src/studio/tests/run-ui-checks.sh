#!/bin/sh
# Runs every UI check in tests/ui/ (one Studio run each: `ttlcd-studio --check <script.js>`) and fails if any does.
#   tests/run-ui-checks.sh [studio binary]     default: the Linux build in build/target, else ttlcd-studio on PATH
# Without a display (CI) each run goes through xvfb-run. TTLCD_NO_DEVICE=1: the cooler is never opened.
set -u
here=$(cd "$(dirname "$0")" && pwd)
bin=${1:-}
if [ -z "$bin" ]; then
    bin=$here/../../../build/target/x86_64-unknown-linux-gnu/release/ttlcd-studio
    [ -x "$bin" ] || bin=ttlcd-studio
fi
export TTLCD_NO_DEVICE=1
run() {
    if [ -n "${DISPLAY:-}" ] || [ -n "${WAYLAND_DISPLAY:-}" ]; then
        timeout 90 "$bin" --check "$1"
    else
        timeout 90 xvfb-run -a -s "-screen 0 1400x1000x24" dbus-launch --exit-with-session "$bin" --check "$1"
    fi
}
failed=0
for f in "$here"/ui/*.js; do
    echo "== $(basename "$f")"
    run "$f" || { echo "   $(basename "$f") failed"; failed=$((failed + 1)); }
done
[ $failed -eq 0 ] && echo "UI checks: all passed" || echo "UI checks: $failed failed"
[ $failed -eq 0 ]
