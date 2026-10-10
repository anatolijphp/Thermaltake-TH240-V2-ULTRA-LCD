@echo off
setlocal
set JAVA_HOME=%~dp0..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0..\re
call "%G%\support\analyzeHeadless.bat" %R%\proj hw -import "C:\Program Files\T'OWEI\resources\app.asar.unpacked\build\Release\hwmonitor_addon.node" -overwrite -scriptPath %R%\scripts -postScript DumpLcd.java %R%\out\hw.c "Calling MXNSDK_Init" "cpu_temperature" "MXNSDK_GetHWInfo returned" -log %R%\out\hw.log
