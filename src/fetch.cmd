@echo off
rem Fetches the Rust build toolchain into tools\ (Windows host). Nothing is installed: no rustup, no Visual Studio,
rem no PATH or registry change. Uses only curl.exe, tar.exe and certutil.exe, which ship with Windows 10/11.
rem Every download is pinned by version + SHA256 and kept in tools\dl\rs\, so re-running only checks hashes.
rem   tools\rust\        rustc + cargo 1.98.0 for x86_64-pc-windows-gnullvm, std for x86_64 and i686 (the Aura plugin)
rem   tools\llvm-mingw\  clang + lld + mingw-w64 (the linker; replaces Visual Studio's link.exe)
rem   tools\cargo-home\  CARGO_HOME: crates.io downloads (pinned by Cargo.lock checksums)
rem   tools\node\        Node.js 24.21.0 LTS: builds Studio's React UI (packages pinned by src\studio\ui\package-lock.json)
rem Linux host: src/fetch.sh.
setlocal enabledelayedexpansion
cd /d "%~dp0.."
set "T=%CD%\tools"
set "DL=%T%\dl\rs"
if not exist "%DL%" mkdir "%DL%"
set RUST=https://static.rust-lang.org/dist/2026-08-20
set MINGW=https://github.com/mstorsjo/llvm-mingw/releases/download/20260922
set NODE=https://nodejs.org/dist/v24.21.0

rem Each merged archive leaves tools\rust\.done-<archive>; .done-1.98.0 marks the base set. A component pinned later
rem (the i686 std) is merged into an existing tools\rust without fetching the rest again.
if exist "%T%\rust\.done-1.98.0" goto extra
if exist "%T%\rust" rmdir /s /q "%T%\rust"
mkdir "%T%\rust"
call :component rustc-1.98.0-x86_64-pc-windows-gnullvm 83512f35f96d874acee08d7f3a7f36063cbc4c17e19b6cf1b10391e88fad4492 || exit /b 1
call :component cargo-1.98.0-x86_64-pc-windows-gnullvm 18c29341fbffacd7dfd1703bc426d17c29367fdd3b47367f065885f6568e5849 || exit /b 1
call :component rust-std-1.98.0-x86_64-pc-windows-gnullvm b3af25904ddbd12d60f88ebda1c335e74eaaa207b83de3236d9eeaafcf1fd53c || exit /b 1
type nul > "%T%\rust\.done-1.98.0"
:extra
call :component rust-std-1.98.0-i686-pc-windows-gnullvm a441efaffd6cd859e7b2b9675a9c59276cb00c48bf3d4366d5b05bdf71342f67 || exit /b 1

if exist "%T%\llvm-mingw\.done-20260922" goto done
if exist "%T%\llvm-mingw" rmdir /s /q "%T%\llvm-mingw"
call :fetch %MINGW%/llvm-mingw-20260922-ucrt-x86_64.zip llvm-mingw-20260922-ucrt-x86_64.zip e3ad77d117a4bea19a7a3b333341824d79a5a371004a10e25b8504e7b3047666 || exit /b 1
mkdir "%T%\llvm-mingw"
rem tar.exe (bsdtar) reads .zip too
tar -xf "%DL%\llvm-mingw-20260922-ucrt-x86_64.zip" -C "%T%\llvm-mingw" --strip-components=1 || exit /b 1
type nul > "%T%\llvm-mingw\.done-20260922"

:done
if exist "%T%\node\.done-24.21.0" goto node_ok
if exist "%T%\node" rmdir /s /q "%T%\node"
call :fetch %NODE%/node-v24.21.0-win-x64.zip node-v24.21.0-win-x64.zip 158f7685b44de51f6c0df1d153526cbcd3e1bc739a8dfc607721cef75de9e541 || exit /b 1
mkdir "%T%\node"
tar -xf "%DL%\node-v24.21.0-win-x64.zip" -C "%T%\node" --strip-components=1 || exit /b 1
type nul > "%T%\node\.done-24.21.0"
:node_ok
if not exist "%T%\cargo-home" mkdir "%T%\cargo-home"
"%T%\rust\bin\rustc.exe" --version || exit /b 1
echo toolchain ready
exit /b 0

rem :component name sha256 - a Rust dist tarball holds <name>\<component>\{bin,lib,...} for each component listed in
rem <name>\components; merging those trees into one prefix is all its install.sh does.
:component
if exist "%T%\rust\.done-%1" exit /b 0
call :fetch %RUST%/%1.tar.gz %1.tar.gz %2 || exit /b 1
if exist "%DL%\x" rmdir /s /q "%DL%\x"
mkdir "%DL%\x"
tar -xzf "%DL%\%1.tar.gz" -C "%DL%\x" || exit /b 1
for /f "usebackq delims=" %%c in ("%DL%\x\%1\components") do (
    xcopy /e /i /q /y "%DL%\x\%1\%%c" "%T%\rust" >nul || exit /b 1
)
rmdir /s /q "%DL%\x"
type nul > "%T%\rust\.done-%1"
exit /b 0

rem :fetch url file sha256 - download unless a file with the right hash is already there
:fetch
call :check "%DL%\%2" %3 && exit /b 0
echo   download %2
if exist "%DL%\%2" del /q "%DL%\%2"
curl.exe -fL --retry 3 -s -o "%DL%\%2.part" %1 || exit /b 1
move /y "%DL%\%2.part" "%DL%\%2" >nul || exit /b 1
call :check "%DL%\%2" %3 && exit /b 0
echo checksum mismatch: %2
exit /b 1

rem :check "file" sha256 - certutil prints a header line, the hash (older Windows: bytes separated by spaces), a footer
:check
if not exist %1 exit /b 1
set H=
for /f "skip=1 delims=" %%h in ('certutil -hashfile %1 SHA256') do if not defined H set "H=%%h"
if not defined H exit /b 1
set "H=!H: =!"
if /i "!H!"=="%2" exit /b 0
exit /b 1
