#!/bin/sh
# Runs the current sources straight from build/ (nothing installed). Linux host.
#   ./run.sh [studio options]        TT LCD Studio (e.g. --tray, --screenshot shot.png dashboard)
#   ./run.sh ttlcd <command ...>     the command-line tool (e.g. ./run.sh ttlcd info)
# Every start builds first (./build.sh run: only the binaries, no packages); cargo recompiles only what changed, so
# that is quick when nothing did. --build as the first argument is still accepted (it changes nothing).
# Studio takes over the cooler: while it runs it stops the `ttlcd` user service (start it again afterwards with
# systemctl --user start ttlcd). The cooler needs the udev rule from linux/60-ttlcd.rules (the .deb installs it).
set -e
cd "$(dirname "$0")"
B=build/target/x86_64-unknown-linux-gnu/release
if [ "${1:-}" = --build ]; then shift; fi
./build.sh run
if [ "${1:-}" = ttlcd ]; then
    shift
    exec "$B/ttlcd" "$@"
fi
exec "$B/ttlcd-studio" "$@"
