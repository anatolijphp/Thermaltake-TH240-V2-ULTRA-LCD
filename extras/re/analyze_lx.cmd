@echo off
setlocal
set JAVA_HOME=%~dp0..\..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0.
call "%G%\support\analyzeHeadless.bat" %R%\proj lx800 -import %R%\lx800\make_fix.bin -overwrite -processor ARM:LE:32:v5t -loader BinaryLoader -loader-baseAddr 0xFFFE0 -scriptPath %R%\scripts -postScript DumpLcd.java %R%\out\lx.c "receiving cmd error" "receiving data error" "no enough space0" "LCM_Init" "load config" "wapper.jpg" "syst\boot" "lcd_boot_proc" -log %R%\out\lx.log
