@echo off
rem Builds TT LCD on a Windows host: build\windows\app\ (the app laid out as installed) and
rem build\windows\TTLcdStudio-Setup.exe. The toolchain is fetched into tools\ (pinned + SHA256); nothing is installed.
rem The implementation is rs\build.cmd (see there for the output list); rs\fetch.cmd fetches the toolchain.
call "%~dp0rs\build.cmd" %*
