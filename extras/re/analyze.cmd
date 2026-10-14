@echo off
setlocal
set JAVA_HOME=%~dp0..\..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0.
call "%G%\support\analyzeHeadless.bat" %R%\proj ttrgb -import %R%\bin\TTRGBPlus.exe -overwrite -scriptPath %R%\scripts -postScript DumpLcd.java %R%\out\lcd.c Device_LCD VID_264A suspend md5 -log %R%\out\ghidra.log
