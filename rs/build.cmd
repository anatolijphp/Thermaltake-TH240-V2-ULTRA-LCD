@echo off
rem Builds TT LCD (Rust, rs\) on Windows with the bundled toolchain from rs\fetch.cmd; nothing is installed.
rem build.cmd at the top of the repo runs this. Sources stay in rs\; intermediate files go to build\rs-target\.
rem   build\windows\app\                     the app laid out exactly as installed (runnable from here)
rem     ttlcd.exe                            CLI (also installs the driver, the Aura plugin and the sensor service)
rem     TTLcdStudio.exe                      Studio window + tray (WebView2, loader built in)
rem     TTLcdAura.dll                        Armoury Crate / Aura Sync HAL plugin, 32-bit (ASUS LightingService)
rem     TTLcdAura64.dll                      the same plugin, 64-bit
rem     driver\                              virtual display driver (copied from driver\)
rem     firmware\ttrescue.exe                firmware read/write tool for the LCD controller's ROM recovery mode
rem     firmware\PatchPanel.exe              rebuilds lx800_tt021panel.bin from firmware\make_fix.bin
rem     firmware\...                         firmware image, Nuvoton helper, burn guide (copied from firmware\)
rem   build\windows\TTLcdStudio-Setup.exe    installer stub with build\windows\app as payload
rem The Linux packages are built on Linux (build.sh). Names must differ by more than case.
setlocal
cd /d "%~dp0"
set "RS=%CD%"
for %%i in ("%RS%\..") do set "ROOT=%%~fi"
set "T=%ROOT%\tools"
set FETCH=
if not exist "%T%\rust\.done-1.98.0" set FETCH=1
if not exist "%T%\rust\.done-rust-std-1.98.0-i686-pc-windows-gnullvm" set FETCH=1
if not exist "%T%\llvm-mingw\.done-20260922" set FETCH=1
if defined FETCH call "%RS%\fetch.cmd" || exit /b 1
set "PATH=%T%\rust\bin;%T%\llvm-mingw\bin;%PATH%"
set "CARGO_HOME=%T%\cargo-home"
rem rs\.cargo\config.toml puts intermediate files in build\rs-target
set CARGO_TARGET_DIR=
set "TGT=%ROOT%\build\rs-target"
set W=x86_64-pc-windows-gnullvm
set W32=i686-pc-windows-gnullvm
set "R=%TGT%\%W%\release"
set "R32=%TGT%\%W32%\release"
set "OUT=%ROOT%\build\windows"
set "APP=%OUT%\app"
set VER=
for /f "tokens=2 delims==" %%v in ('findstr /b /c:"version = " "%RS%\Cargo.toml"') do if not defined VER set "VER=%%~v"
set "VER=%VER: =%"
set "VER=%VER:"=%"
echo TT LCD %VER%

echo [1/5] cargo build (x86_64)
cargo build --release --locked --target %W% -p ttlcd-cli -p ttlcd-studio -p ttlcd-aura-hal -p ttlcd-rescue -p ttlcd-patchpanel -p ttlcd-setup || exit /b 1
echo [2/5] cargo build (i686 Aura plugin: ASUS LightingService is a 32-bit process)
cargo build --release --locked --target %W32% -p ttlcd-aura-hal || exit /b 1

echo [3/5] PatchPanel check
"%R%\PatchPanel.exe" "%ROOT%\firmware\make_fix.bin" "%TGT%\lx800_tt021panel.check.bin" >nul || exit /b 1
set WANT=
for /f "usebackq tokens=1,2" %%a in ("%ROOT%\firmware\SHA256SUMS.txt") do if "%%b"=="lx800_tt021panel.bin" set "WANT=%%a"
call :sha256 "%TGT%\lx800_tt021panel.check.bin"
del /q "%TGT%\lx800_tt021panel.check.bin"
if /i not "%H%"=="%WANT%" (echo PatchPanel output differs from firmware\lx800_tt021panel.bin & exit /b 1)
echo PatchPanel: rebuilds lx800_tt021panel.bin byte for byte

echo [4/5] build\windows\app
if not exist "%APP%\driver" mkdir "%APP%\driver"
if not exist "%APP%\firmware" mkdir "%APP%\firmware"
rem A running exe or loaded DLL can't be overwritten (Studio, the sensors service, LightingService with the Aura
rem plugin), but it can be renamed. So move the current binaries aside, copy fresh ones, and delete the leftovers
rem that are no longer in use (the rest go on a later build).
del /q "%APP%\*.old" "%APP%\firmware\*.old" >nul 2>nul
for %%f in (ttlcd.exe TTLcdStudio.exe TTLcdAura.dll TTLcdAura64.dll firmware\ttrescue.exe firmware\PatchPanel.exe) do if exist "%APP%\%%f" ren "%APP%\%%f" "%%~nxf.%RANDOM%.old"
del /q "%APP%\*.old" "%APP%\firmware\*.old" >nul 2>nul
copy /y "%R%\ttlcd.exe" "%APP%\ttlcd.exe" >nul || exit /b 1
copy /y "%R%\ttlcd-studio.exe" "%APP%\TTLcdStudio.exe" >nul || exit /b 1
copy /y "%R32%\TTLcdAura.dll" "%APP%\TTLcdAura.dll" >nul || exit /b 1
copy /y "%R%\TTLcdAura.dll" "%APP%\TTLcdAura64.dll" >nul || exit /b 1
rem Studio carries its own WebView2 loader; it must not need Microsoft's WebView2Loader.dll next to it
llvm-objdump -p "%APP%\TTLcdStudio.exe" | findstr /i /c:"DLL Name: WebView2Loader.dll" >nul
if not errorlevel 1 (echo TTLcdStudio.exe imports WebView2Loader.dll; the app ships without it & exit /b 1)
copy /y "%R%\ttrescue.exe" "%APP%\firmware\" >nul || exit /b 1
copy /y "%R%\PatchPanel.exe" "%APP%\firmware\" >nul || exit /b 1
for %%f in (MttVDD.inf MttVDD.dll mttvdd.cat) do copy /y "%ROOT%\driver\%%f" "%APP%\driver\" >nul || exit /b 1
for %%f in (README.md lx800_tt021panel.bin FA93_musb.bin hardware_boot_pins.jpg SHA256SUMS.txt) do copy /y "%ROOT%\firmware\%%f" "%APP%\firmware\" >nul || exit /b 1

echo [5/5] TTLcdStudio-Setup.exe
if exist "%OUT%\TTLcdStudio-Setup.exe" del /q "%OUT%\TTLcdStudio-Setup.exe"
cargo metadata --no-deps --format-version 1 | findstr /c:"ttlcd-pack" >nul
if errorlevel 1 (
    echo   skipped: the installer packer ^(ttlcd-setup bin ttlcd-pack^) doesn't exist yet
) else (
    cargo run --release --locked -q -p ttlcd-setup --bin ttlcd-pack -- "%R%\TTLcdStudio-Setup.exe" "%APP%" "%OUT%\TTLcdStudio-Setup.exe" || exit /b 1
)
echo build OK: build\windows (%VER%)
exit /b 0

rem :sha256 "file" - sets H to the file's SHA256 (certutil: header line, hash, footer; older Windows spaces the bytes)
:sha256
set H=
for /f "skip=1 delims=" %%h in ('certutil -hashfile %1 SHA256') do if not defined H set "H=%%h"
set "H=%H: =%"
exit /b 0
