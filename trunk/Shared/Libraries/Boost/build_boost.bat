@echo off

rem ##################################################
rem # Script to compile boost in Win32 and Win64 env #
rem ##################################################

set PWD=%cd%

rem check first param
if "%1" EQU "x86" goto x86
if "%1" EQU "x64" goto x64
if "%1" EQU "--help" goto error
if "%1" NEQ "" goto error
goto x64

:x86
set ENV=32
goto check_mode
:x64
set ENV=64
goto check_mode

:check_mode
if "%2" EQU "debug" goto debug
if "%2" EQU "release" goto release
if "%2" NEQ "" goto error
goto release

:debug
set MODE="debug"
goto build
:release
set MODE="release"
goto build

:build
cd boost_1_59_0
rem Launching configuration script
call bootstrap.bat
rem Launching compilation script
call b2.exe toolset=msvc-12.0 architecture=x86 address-model=%ENV% variant=%MODE% link=static,shared threading=multi --build-type=complete
goto exit

:error
echo "usage: build_boost.bat <x86|[x64]> <debug|[release]>"
echo "parameters are optional; default values are those in square brackets"

:exit
cd %PWD%
echo. 
echo process complete; press any key to continue...
pause > nul
