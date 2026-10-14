@echo off
setlocal
set JAVA_HOME=%~dp0..\..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0.
call "%G%\support\analyzeHeadless.bat" %R%\proj tw -import %~dp0..\..\tools\turbowriter\TurboWriter.exe -overwrite -scriptPath %R%\scripts -postScript DumpLcd.java %R%\out\tw.c "Enter IBR connection" "Detect IBR_MASSSTOR" "Read blocks for Save file" "Enter XSCSII" "Failed IbrConnect" "Send Write SDRAM CMD" "Send CMD_WriterProcess" "Read OK" "RAWSPI" "Enter Send CMD_WriteFile" -log %R%\out\tw.log
