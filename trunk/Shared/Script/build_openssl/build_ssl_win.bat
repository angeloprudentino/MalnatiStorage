@echo off
rem **********************************************************************
rem *
rem * Script for compiling Openssl in WIN32 environment using VS 
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
set ARCH=x64
SET LIB_DIR=%BASE%\x86_64
set WIN=VC-WIN64A
set NO_ASM=no-asm
set COMPILE_SCRIPT=ms\do_win64a
goto sel_mode

:use_x86
set ARCH=x86
SET LIB_DIR=%BASE%\x86
set WIN=VC-WIN32
set COMPILE_SCRIPT=ms\do_nasm
goto sel_mode

:sel_mode
if "%2" EQU "debug" goto use_debug
if "%2" EQU "release" goto use_release
if "%2" NEQ "" goto error

:use_release
SET MODE=""
SET OUT_LIB_DIR=%LIB_DIR%\release
goto config

:use_debug
SET MODE=debug-
SET OUT_LIB_DIR=%LIB_DIR%\debug
goto config

:config
SET VERSION=1.0.2g
SET VC="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC"
SET OUT_OPENSSL_DIR=C:\OpenSSL_%ARCH%_%VERSION%
SET OPENSSL_OPTIONS=no-hw
mkdir "%OUT_INCLUDE_DIR%\openssl"
mkdir "%OUT_LIB_DIR%\openssl"
mkdir "%OUT_LIB_DIR%\openssl\static"

mkdir temp
cd temp
call curl -O http://www.openssl.org/source/openssl-%VERSION%.tar.gz
call 7z e openssl-%VERSION%.tar.gz
call 7z x openssl-%VERSION%.tar 
SET OPENSSL_DIR="%PWD%\temp\openssl-%VERSION%"

rem **********************************************************************
rem * Setting VS environment variables
rem **********************************************************************
call %VC%\vcvarsall.bat %ARCH%

cd %OPENSSL_DIR%
perl Configure %MODE%%WIN% %NO_ASM% %OPENSSL_OPTIONS% --prefix=%OUT_OPENSSL_DIR%
call %COMPILE_SCRIPT%

rem **********************************************************************
rem * Building
rem **********************************************************************
nmake -f ms\ntdll.mak
nmake -f ms\ntdll.mak install

cd %OUT_OPENSSL_DIR%\bin
copy /Y *.dll "%OUT_LIB_DIR%\openssl"
cd %OUT_OPENSSL_DIR%\lib
copy /Y *.lib "%OUT_LIB_DIR%\openssl\static"
cd %OUT_OPENSSL_DIR%\include\openssl
copy /Y * "%OUT_INCLUDE_DIR%\openssl"

cd "%PWD%"
rmdir /S /Q temp
goto exit

:error
echo "use <script> [x64]|x86 [release]|debug"
echo "default parameters are in square brackets."

:exit
echo Premi un tasto per confermare
pause > nul
