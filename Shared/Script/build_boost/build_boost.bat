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
set ENV_DIR=x86
goto check_mode
:x64
set ENV=64
set ENV_DIR=x86_64
goto check_mode

:check_mode
if "%2" EQU "debug" goto debug
if "%2" EQU "release" goto release
if "%2" NEQ "" goto error
goto release

:debug
set MODE="debug"
set DLL=*-mt-gd-1_59.dll
goto build
:release
set MODE="release"
set DLL=*-mt-1_59.dll
goto build

:build
set LIB_SUB_DIR="%ENV_DIR%\lib\%MODE%"
set INC_SUB_DIR="%ENV_DIR%"
cd boost_1_59_0
rem Launching configuration script
call bootstrap.bat
rem Launching compilation script
call b2.exe toolset=msvc-12.0 architecture=x86 address-model=%ENV% variant=%MODE% link=shared threading=multi

set BOOST_LIB="..\..\..\Libraries\%ENV_DIR%\lib\%MODE%"
set BOOST_INC="..\..\..\Libraries\%ENV_DIR%\include"
mkdir %BOOST_LIB%\boost
copy /Y stage\lib\%DLL% %BOOST_LIB%\boost
mkdir %BOOST_INC%\boost
xcopy /E /Y boost %BOOST_INC%\boost
goto exit

:error
echo "usage: build_boost.bat <x86|[x64]> <debug|[release]>"
echo "parameters are optional; default values are those in square brackets."
echo "if you want to use debug, you have to specify also the first param"

:exit
cd %PWD%
echo. 
echo process complete; press any key to continue...
pause > nul
