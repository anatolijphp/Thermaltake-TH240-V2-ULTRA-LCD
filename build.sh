#!/bin/sh
# Builds TT LCD on a Linux host: the Linux packages (build/linux/) and the Windows app + installer cross-linked with
# llvm-mingw (build/windows/). The toolchain is fetched into tools/ (pinned + SHA256); nothing is installed.
#   ./build.sh [all|linux|windows]
# The implementation is rs/build.sh (see there for the output list); rs/fetch.sh fetches the toolchain.
exec sh "$(dirname "$0")/rs/build.sh" "$@"
