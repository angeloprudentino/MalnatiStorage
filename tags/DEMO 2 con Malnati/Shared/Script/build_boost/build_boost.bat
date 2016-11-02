@echo off

rem ##################################################
rem # Script to compile boost in Win32 and Win64 env #
rem ##################################################

set PWD=%cd%

rem check first param
if "%1" EQU "x86" goto x86
if "%1" EQU "x86_64" goto x86_64
if "%1" EQU "--help" goto error
if "%1" NEQ "" goto error
goto x86_64

:x86
set ENV=32
set ENV_DIR=x86

:x86_64
set ENV=64
set ENV_DIR=x86_64

:debug
set DEBUG_LIB=*-mt-gd-1_59.lib
set DEBUG_DLL=*-mt-gd-1_59.dll

:release
set RELEASE_LIB=*-mt-1_59.lib
set RELEASE_DLL=*-mt-1_59.dll

:build
cd boost_1_59_0
rem Launching configuration script
call bootstrap.bat
rem Launching compilation script
call b2.exe toolset=msvc-12.0 architecture=x86 address-model=%ENV% variant=debug,release link=shared threading=multi define=_BIND_TO_CURRENT_VCLIBS_VERSION

set BOOST_LIB="..\..\..\Libraries\%ENV_DIR%"
set BOOST_INC="..\..\..\Libraries\include"

mkdir %BOOST_LIB%
mkdir %BOOST_LIB%\debug
mkdir %BOOST_LIB%\debug\boost
mkdir %BOOST_LIB%\debug\boost\static
copy /Y stage\lib\%DEBUG_LIB% %BOOST_LIB%\debug\boost\static
copy /Y stage\lib\%DEBUG_DLL% %BOOST_LIB%\debug\boost

mkdir %BOOST_LIB%\release
mkdir %BOOST_LIB%\release\boost
mkdir %BOOST_LIB%\release\boost\static
copy /Y stage\lib\%RELEASE_LIB% %BOOST_LIB%\release\boost\static
copy /Y stage\lib\%RELEASE_DLL% %BOOST_LIB%\release\boost

mkdir %BOOST_INC%
mkdir %BOOST_INC%\boost
xcopy /E /Y boost %BOOST_INC%\boost
goto exit

:error
echo "usage: build_boost.bat <x86|[x86_64]>"
echo "parameters are optional; default values are those in square brackets."
echo "if you want to use debug, you have to specify also the first param"

:exit
cd %PWD%
echo. 
echo process complete; press any key to continue...
pause > nul
