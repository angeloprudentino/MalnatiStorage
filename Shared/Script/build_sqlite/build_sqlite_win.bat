@echo off
rem **********************************************************************
rem *
rem * Script for compiling SQLite in WIN32 environment using VS 
rem *
rem **********************************************************************
if "%1" EQU "--help" goto error

SET PWD=%cd%
cd "%PWD%\..\..\Libraries"
SET BASE=%cd%
cd "%PWD%"
SET OUT_INCLUDE_DIR=%BASE%\include

if "%1" EQU "x86" goto use_x86
if "%1" EQU "x64" goto use_x64
if "%1" NEQ "" goto error

:use_x64
set PLATFORM=x64
SET LIB_DIR=%BASE%\x86_64
goto sel_mode

:use_x86
set PLATFORM=x86
SET LIB_DIR=%BASE%\x86
goto sel_mode

:sel_mode
if "%2" EQU "debug" goto use_debug
if "%2" EQU "release" goto use_release
if "%2" NEQ "" goto error

:use_release
SET DEBUG=0
SET OUT_LIB_DIR=%LIB_DIR%\release
goto config

:use_debug
SET DEBUG=3
SET OUT_LIB_DIR=%LIB_DIR%\debug
goto config

:config
SET VERSION=1.0.2g
SET VC="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC"
mkdir "%OUT_INCLUDE_DIR%\sqlite"
mkdir "%OUT_LIB_DIR%\sqlite"
mkdir "%OUT_LIB_DIR%\sqlite\static"


rem **********************************************************************
rem * Setting VS environment variables
rem **********************************************************************
call %VC%\vcvarsall.bat %PLATFORM%


rem **********************************************************************
rem * Building
rem **********************************************************************
cd sqlite-autoconf-3140200 
call nmake /f Makefile.msc sqlite3.dll

copy /Y *.dll "%OUT_LIB_DIR%\sqlite"
copy /Y *.lib "%OUT_LIB_DIR%\sqlite\static"
copy /Y *.h "%OUT_INCLUDE_DIR%\sqlite"

cd "%PWD%"
goto exit

:error
echo "use <script> [x64]|x86 [release]|debug"
echo "default parameters are in square brackets."

:exit
echo Premi un tasto per confermare
pause > nul
