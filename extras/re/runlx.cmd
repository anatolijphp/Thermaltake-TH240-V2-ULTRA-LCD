@echo off
rem usage: runlx.cmd <project> <program> <Script.java> <args...>
setlocal
set JAVA_HOME=%~dp0..\..\tools\jdk21
set PATH=%JAVA_HOME%\bin;%SystemRoot%\system32;%SystemRoot%
set G=%~dp0..\..\tools\ghidra_12.1.4_PUBLIC
set R=%~dp0.
set P=%1
set F=%2
shift
shift
call "%G%\support\analyzeHeadless.bat" %R%\proj %P% -process %F% -noanalysis -readOnly -scriptPath %R%\scripts -postScript %1 %2 %3 %4 %5 %6 %7 %8 %9
