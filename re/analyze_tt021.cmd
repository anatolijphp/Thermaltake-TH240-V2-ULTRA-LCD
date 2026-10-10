@echo off
setlocal
set JAVA_HOME=%~dp0..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0..\re
call "%G%\support\analyzeHeadless.bat" %R%\proj tt021 -import "C:\Program Files\Tt\TT RGB PLUS\TT021-00-0_FW_V1.0.5.4.sys" -overwrite -processor ARM:LE:32:v5t -loader BinaryLoader -loader-fileOffset 0x24 -loader-baseAddr 0x0 -scriptPath %R%\scripts -postScript FindImm.java %R%\out\tt021_lcm.c 6 77 13 -log %R%\out\tt021.log
