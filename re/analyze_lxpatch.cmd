@echo off
setlocal
set JAVA_HOME=%~dp0..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0..\re
call "%G%\support\analyzeHeadless.bat" %R%\proj lxpatch -import %~dp0..\firmware\lx800_tt021panel.bin -overwrite -processor ARM:LE:32:v5t -loader BinaryLoader -loader-baseAddr 0xFFFE0 -scriptPath %R%\scripts -postScript DumpLcd.java %R%\out\lxpatch.c "LCM_Init" -log %R%\out\lxpatch.log
