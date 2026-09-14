@echo off
rem compile-wat.cmd - Build Bingo/2 for OS/2 with Open Watcom
rem Detects WATCOM environment variable and runs wmake.

set LOGFILE=compile-wat.log
echo Build started %DATE% %TIME% > %LOGFILE%

rem Locate Open Watcom
if not "%WATCOM%" == "" goto :found_watcom

rem Try common ArcaOS / eCS install paths
if exist C:\WATCOM2\binp\wmake.exe set WATCOM=C:\WATCOM2
if not "%WATCOM%" == "" goto :found_watcom
if exist C:\WATCOM\binp\wmake.exe set WATCOM=C:\WATCOM
if not "%WATCOM%" == "" goto :found_watcom

echo ERROR: WATCOM environment variable not set and C:\WATCOM not found. >> %LOGFILE%
echo ERROR: WATCOM environment variable not set and C:\WATCOM not found.
goto :fail

:found_watcom
echo Using WATCOM at %WATCOM% >> %LOGFILE%
set PATH=%WATCOM%\binp;%WATCOM%\binw;%PATH%
set INCLUDE=%WATCOM%\h;%WATCOM%\h\os2;%INCLUDE%
set LIB=%WATCOM%\lib386;%WATCOM%\lib386\os2;%LIB%

rem Locate OS/2 Toolkit
if not "%OS2TK%" == "" goto :found_tk
if exist C:\os2tk45\h\os2.h set OS2TK=C:\os2tk45
if not "%OS2TK%" == "" goto :found_tk
echo WARNING: OS2TK not set and C:\os2tk45 not found; MMPM/2 headers may be missing. >> %LOGFILE%
goto :build

:found_tk
echo Using OS2TK at %OS2TK% >> %LOGFILE%
set INCLUDE=%OS2TK%\h;%OS2TK%\h\mm;%INCLUDE%

:build
rem Make sure bin\ exists
if not exist bin md bin

rem Clean then build
echo Cleaning... >> %LOGFILE%
wmake -f makefile.wat clean >> %LOGFILE% 2>&1

echo Building... >> %LOGFILE%
wmake -f makefile.wat >> %LOGFILE% 2>&1

if errorlevel 1 goto :fail

echo. >> %LOGFILE%
echo BUILD OK >> %LOGFILE%
echo BUILD OK
goto :end

:fail
echo. >> %LOGFILE%
echo BUILD FAILED >> %LOGFILE%
echo BUILD FAILED - check %LOGFILE% for details

:end
