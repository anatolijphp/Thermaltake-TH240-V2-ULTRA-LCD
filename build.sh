#!/bin/sh
# Builds TT LCD on a Linux host: the Linux packages (build/linux/) and the Windows app + installer cross-linked with
# llvm-mingw (build/windows/). The toolchain is fetched into tools/ (pinned + SHA256); nothing is installed.
#   ./build.sh [all|linux|windows]
# The implementation is src/build.sh (see there for the output list); src/fetch.sh fetches the toolchain.
exec sh "$(dirname "$0")/src/build.sh" "$@"
