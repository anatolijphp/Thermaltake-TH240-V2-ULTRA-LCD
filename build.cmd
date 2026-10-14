@echo off
rem Builds TT LCD on a Windows host: build\windows\app\ (the app laid out as installed) and
rem build\windows\TTLcdStudio-Setup.exe. The toolchain is fetched into tools\ (pinned + SHA256); nothing is installed.
rem The implementation is src\build.cmd (see there for the output list); src\fetch.cmd fetches the toolchain.
call "%~dp0src\build.cmd" %*
