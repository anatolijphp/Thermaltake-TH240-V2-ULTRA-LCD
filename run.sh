#!/bin/sh
# Runs the new build straight from build/ (nothing installed). Linux host.
#   ./run.sh [studio options]        TT LCD Studio (e.g. --tray, --screenshot shot.png dashboard)
#   ./run.sh ttlcd <command ...>     the command-line tool (e.g. ./run.sh ttlcd info)
#   ./run.sh --build ...             rebuild first even if the build looks current
# Builds the Linux part (./build.sh linux) first when it's missing or older than the sources.
# Studio takes over the cooler: while it runs it stops the `ttlcd` user service (start it again afterwards with
# systemctl --user start ttlcd). The cooler needs the udev rule from linux/60-ttlcd.rules (the .deb installs it).
set -e
cd "$(dirname "$0")"
B=build/target/x86_64-unknown-linux-gnu/release
force=
if [ "${1:-}" = --build ]; then force=1; shift; fi
if [ -n "$force" ] || [ ! -x "$B/ttlcd-studio" ] || [ ! -x "$B/ttlcd" ] ||
   [ -n "$(find src linux -type f -newer "$B/ttlcd-studio" -print -quit)" ]; then
    ./build.sh linux
fi
if [ "${1:-}" = ttlcd ]; then
    shift
    exec "$B/ttlcd" "$@"
fi
exec "$B/ttlcd-studio" "$@"
