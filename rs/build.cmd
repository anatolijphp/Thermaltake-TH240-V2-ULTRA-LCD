@echo off
rem Builds the Rust port on Windows with the bundled toolchain (rs\fetch.cmd runs first if tools\ lacks it).
rem Nothing is installed. Output: build\windows\rs\ttlcd-rs.exe, ttlcd-studio.exe, WebView2Loader.dll
setlocal
cd /d "%~dp0"
set ROOT=%CD%\..
set T=%ROOT%\tools
if not exist "%T%\rust\.done-1.98.0" call fetch.cmd || exit /b 1
if not exist "%T%\llvm-mingw\.done-20260922" call fetch.cmd || exit /b 1
set PATH=%T%\rust\bin;%T%\llvm-mingw\bin;%PATH%
set CARGO_HOME=%T%\cargo-home
cargo build --release --locked --target x86_64-pc-windows-gnullvm || exit /b 1
set REL=%ROOT%\build\rs-target\x86_64-pc-windows-gnullvm\release
if not exist "%ROOT%\build\windows\rs" mkdir "%ROOT%\build\windows\rs"
copy /y "%REL%\ttlcd-rs.exe" "%ROOT%\build\windows\rs\" >nul || exit /b 1
copy /y "%REL%\ttlcd-studio.exe" "%ROOT%\build\windows\rs\" >nul || exit /b 1
rem Microsoft's redistributable WebView2 loader (from the webview2-com-sys crate); the WebView2 runtime ships with Windows
for /d %%d in ("%REL%\build\webview2-com-sys-*") do if exist "%%d\out\x64\WebView2Loader.dll" copy /y "%%d\out\x64\WebView2Loader.dll" "%ROOT%\build\windows\rs\" >nul
echo build OK: build\windows\rs
