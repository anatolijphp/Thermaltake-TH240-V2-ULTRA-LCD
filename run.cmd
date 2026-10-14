@echo off
rem Runs the new build straight from build\windows\app (nothing installed). Windows host.
rem   run.cmd [studio options]        TT LCD Studio (e.g. --tray, --screenshot shot.png dashboard)
rem   run.cmd ttlcd ^<command ...^>     the command-line tool (e.g. run.cmd ttlcd info)
rem   run.cmd --build ...             rebuild first (build.cmd); without it, it builds only when nothing is built yet
rem Close an installed TT LCD Studio first: only one program can drive the cooler.
setlocal
cd /d "%~dp0"
set "APP=%~dp0build\windows\app"
set ARGS=%*
set BUILD=
if /i "%~1"=="--build" (set BUILD=1& set "ARGS=%ARGS:*--build=%")
if not exist "%APP%\TTLcdStudio.exe" set BUILD=1
if not exist "%APP%\ttlcd.exe" set BUILD=1
if defined BUILD call "%~dp0build.cmd" || exit /b 1
for /f "tokens=1" %%a in ("%ARGS%") do if /i "%%a"=="ttlcd" goto cli
start "" "%APP%\TTLcdStudio.exe" %ARGS%
exit /b 0
:cli
set "ARGS=%ARGS:*ttlcd=%"
"%APP%\ttlcd.exe" %ARGS%
exit /b %errorlevel%
