@echo off
rem Builds the Windows version with the C# compiler that ships with Windows (.NET Framework 4.8); nothing to install.
rem Sources stay in src\ aura\ rescue\ installer\; everything built goes to build\windows\:
rem   build\windows\app\                       the app, laid out exactly as installed (runnable from here)
rem     ttlcd.exe                              CLI (also installs the driver, the Aura plugin and the sensor service)
rem     TTLcdStudio.exe                        WPF app + tray
rem     TTLcdAura.dll                          Armoury Crate / Aura Sync HAL plugin (AnyCPU, loaded by 32-bit ASUS LightingService)
rem     driver\                                virtual display driver (copied from driver\)
rem     firmware\ttrescue.exe                  firmware read/write tool for the LCD controller's ROM recovery mode (x86)
rem     firmware\PatchPanel.exe                rebuilds lx800_tt021panel.bin from firmware\make_fix.bin (x86)
rem     firmware\...                           firmware image, Nuvoton helper, burn guide (copied from firmware\)
rem   build\windows\TTLcdStudio-Setup.exe      installer carrying build\windows\app
rem Other operating systems get their own build\<os>\ folder.
rem Note: names must differ by more than case (Windows file names are case-insensitive).
setlocal
set FW=%WINDIR%\Microsoft.NET\Framework64\v4.0.30319
set FW32=%WINDIR%\Microsoft.NET\Framework\v4.0.30319
set CSC=%FW%\csc.exe
set CSC32=%FW32%\csc.exe
set REFS=/r:System.Drawing.dll /r:System.Windows.Forms.dll /r:System.Xml.dll /r:System.Core.dll /r:System.ServiceProcess.dll /r:System.Management.dll
set WPF=/r:"%FW%\WPF\PresentationFramework.dll" /r:"%FW%\WPF\PresentationCore.dll" /r:"%FW%\WPF\WindowsBase.dll" /r:"%FW%\System.Xaml.dll"
cd /d "%~dp0"
set OUT=build\windows
set APP=%OUT%\app
if not exist %APP%\driver mkdir %APP%\driver
if not exist %APP%\firmware mkdir %APP%\firmware
rem A running exe can't be overwritten (Studio, or the TT LCD sensors service), but it can be renamed.
rem So move the current binaries aside, build fresh ones, and delete the leftovers that are no longer in use.
del /q %APP%\*.old >nul 2>nul
for %%f in (ttlcd.exe TTLcdStudio.exe TTLcdAura.dll) do if exist %APP%\%%f ren %APP%\%%f %%f.%RANDOM%.old
del /q %APP%\*.old >nul 2>nul
set CORE=src\Version.cs src\Hid.cs src\Lcd.cs src\Render.cs src\Capture.cs src\Config.cs src\Engine.cs src\Video.cs src\VirtualDisplay.cs src\Aura.cs src\Storage.cs src\Sensors.cs src\Games.cs

echo [1/6] ttlcd.exe
"%CSC%" /nologo /optimize+ /unsafe /platform:x64 /target:exe /main:TtLcd.Program /win32icon:src\app.ico /out:%APP%\ttlcd.exe %REFS% %CORE% src\Program.cs || exit /b 1
echo [2/6] TTLcdStudio.exe
"%CSC%" /nologo /optimize+ /unsafe /platform:x64 /target:winexe /main:TtLcd.TrayProgram /win32icon:src\app.ico /resource:src\app.ico,TtLcd.app.ico /out:%APP%\TTLcdStudio.exe %REFS% %WPF% /resource:src\MainWindow.xaml,TtLcd.MainWindow.xaml %CORE% src\App.cs src\Record.cs src\Gif.cs || exit /b 1
echo [3/6] TTLcdAura.dll
"%CSC%" /nologo /optimize+ /platform:anycpu /target:library /out:%APP%\TTLcdAura.dll /r:System.Core.dll aura\AuraHal.cs || exit /b 1
echo [4/6] rescue tools
rem x86 so SCSI_PASS_THROUGH_DIRECT has the same 32-bit layout as Nuvoton's TurboWriter
"%CSC32%" /nologo /optimize+ /platform:x86 /target:exe /out:%APP%\firmware\ttrescue.exe /r:System.Core.dll rescue\Rescue.cs || exit /b 1
"%CSC32%" /nologo /optimize+ /platform:x86 /target:exe /out:%APP%\firmware\PatchPanel.exe rescue\PatchPanel.cs || exit /b 1
echo [5/6] driver and firmware files
for %%f in (MttVDD.inf MttVDD.dll mttvdd.cat) do copy /y driver\%%f %APP%\driver\ >nul || exit /b 1
for %%f in (README.md lx800_tt021panel.bin FA93_musb.bin hardware_boot_pins.jpg SHA256SUMS.txt) do copy /y firmware\%%f %APP%\firmware\ >nul || exit /b 1
echo [6/6] TTLcdStudio-Setup.exe
set PAYLOAD=/resource:%APP%\ttlcd.exe,payload/ttlcd.exe /resource:%APP%\TTLcdStudio.exe,payload/TTLcdStudio.exe /resource:%APP%\TTLcdAura.dll,payload/TTLcdAura.dll
set PAYLOAD=%PAYLOAD% /resource:%APP%\driver\MttVDD.inf,payload/driver/MttVDD.inf /resource:%APP%\driver\MttVDD.dll,payload/driver/MttVDD.dll /resource:%APP%\driver\mttvdd.cat,payload/driver/mttvdd.cat
set PAYLOAD=%PAYLOAD% /resource:%APP%\firmware\ttrescue.exe,payload/firmware/ttrescue.exe /resource:%APP%\firmware\README.md,payload/firmware/README.md
set PAYLOAD=%PAYLOAD% /resource:%APP%\firmware\lx800_tt021panel.bin,payload/firmware/lx800_tt021panel.bin /resource:%APP%\firmware\FA93_musb.bin,payload/firmware/FA93_musb.bin
set PAYLOAD=%PAYLOAD% /resource:%APP%\firmware\hardware_boot_pins.jpg,payload/firmware/hardware_boot_pins.jpg /resource:%APP%\firmware\SHA256SUMS.txt,payload/firmware/SHA256SUMS.txt
"%CSC%" /nologo /optimize+ /platform:anycpu /target:winexe /win32icon:src\app.ico /out:%OUT%\TTLcdStudio-Setup.exe /r:System.Windows.Forms.dll /r:System.Drawing.dll /r:System.Core.dll %PAYLOAD% installer\Setup.cs src\Version.cs || exit /b 1
echo build OK: %OUT%
