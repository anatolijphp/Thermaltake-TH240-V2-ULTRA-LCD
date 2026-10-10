@echo off
rem usage: runtw.cmd <Script.java> <args...>   (runs a script against the analyzed TurboWriter project)
setlocal
set JAVA_HOME=%~dp0..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0..\re
call "%G%\support\analyzeHeadless.bat" %R%\proj tw -process TurboWriter.exe -noanalysis -readOnly -scriptPath %R%\scripts -postScript %*
