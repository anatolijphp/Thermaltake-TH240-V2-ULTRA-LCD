@echo off
rem Runs the current sources straight from build\windows\app (nothing installed). Windows host.
rem   run.cmd [studio options]        TT LCD Studio (e.g. --tray, --screenshot shot.png dashboard)
rem   run.cmd ttlcd ^<command ...^>     the command-line tool (e.g. run.cmd ttlcd info)
rem Every start builds first (build.cmd run: the binaries, no installer); cargo recompiles only what changed, so that
rem is quick when nothing did. --build as the first argument is still accepted (it changes nothing).
rem Close an installed TT LCD Studio first: only one program can drive the cooler.
setlocal
cd /d "%~dp0"
set "APP=%~dp0build\windows\app"
set ARGS=%*
if /i "%~1"=="--build" set "ARGS=%ARGS:*--build=%"
call "%~dp0build.cmd" run || exit /b 1
for /f "tokens=1" %%a in ("%ARGS%") do if /i "%%a"=="ttlcd" goto cli
start "" "%APP%\TTLcdStudio.exe" %ARGS%
exit /b 0
:cli
set "ARGS=%ARGS:*ttlcd=%"
"%APP%\ttlcd.exe" %ARGS%
exit /b %errorlevel%
