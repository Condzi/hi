:: Based on RAD Debugger project `build.bat` file.

@echo off
setlocal
call cls
cd /D "%~dp0"

:: --- Unpack Arguments -------------------------------------------------------
if "%1"==""       goto show_help
if "%1"=="?"      goto show_help
if "%1"=="help"   goto show_help
if "%1"=="-h"     goto show_help
if "%1"=="--help" goto show_help

for %%a in (%*) do set "%%a=1"

if "%clang%"=="1"   echo [clang++]
if "%debug%"=="1"   set release=0 && echo [debug]
if "%release%"=="1" set debug=0   && echo [release]

if "%asan%"=="1" (   
  echo [asan]
) else ( 
  echo [asan  -- disabled]
)

if "%ubsan%"=="1" (   
  echo [ubsan]
) else ( 
  echo [ubsan -- disabled]
)

:: --- Configure Sanitizers  --------------------------------------------------
set clang_sanitizer=
if "%asan%"=="1"      set clang_sanitizer=-fsanitize=address -fno-omit-frame-pointer
if "%ubsan%"=="1"     set clang_sanitizer=-fsanitize=undefined -fno-omit-frame-pointer
if "%asan%"=="1" (
  if "%ubsan%"=="1"   set clang_sanitizer=-fsanitize=address,undefined -fno-omit-frame-pointer
)

:: --- clang++ Compile/Link Line Definitions -------------------------------------
set clang_disabled_warnings= -Wno-format -Wno-pragma-once-outside-header -Wno-gcc-compat ^
                             -Wno-missing-field-initializers -Wno-missing-braces -Wno-unused-function

set clang_defines=    -DWIN32 -D_WINDOWS -D_HAS_EXCEPTIONS=0 -D_CRT_SECURE_NO_WARNING
set clang_misc=       -fno-exceptions -fno-rtti -ferror-limit=0 -Wall -Wextra -Werror
set clang_common=     -I..\code\ -I..\code\3rdparty -I..\code\3rdparty\box2c\include %clang_defines% %clang_misc% %clang_disabled_warnings% 
set clang_debug=      -g -O0 %clang_common% %clang_sanitizer%
set clang_release=    -Ofast -DNDEBUG %clang_common%
set clang_out=        -o 
set clang_link=       -fuse-ld=lld-link -Wl,/MANIFEST:EMBED,/INCREMENTAL:NO

:: This is bullshit workaround for broken UBSAN on windows.
if "%ubsan%"=="1"      set clang_link=%clang_link% ^
                      "C:\Program Files\LLVM\lib\clang\18\lib\windows\clang_rt.ubsan_standalone-x86_64.lib" ^
                      "C:\Program Files\LLVM\lib\clang\18\lib\windows\clang_rt.ubsan_standalone_cxx-x86_64.lib"

:: --- Choose Compile/Link Lines ----------------------------------------------
if "%clang%"=="1"     set compile_debug=     %clang_debug% 
if "%clang%"=="1"     set compile_release=   %clang_release%
if "%clang%"=="1"     set compile_link=      %clang_link%
if "%clang%"=="1"     set out=               %clang_out%

set compile_commands=
if "%clang%"=="1"     set compile_commands=  -MJ ../compile_commands.json

if "%debug%"=="1"     set compile=%compile_debug%
if "%release%"=="1"   set compile=%compile_release%

:: --- Get current Git commit id ----------------------------------------------
for /f %%i in ('call git describe --always --dirty') do set compile=%compile% -DHI_GIT=\"%%i\"

:: --- Prep Directories -------------------------------------------------------
if not exist build mkdir build
if exist game.exe del game.exe >nul

:: --- Build Everything (@build_targets) --------------------------------------
pushd build

:: --- Build Box2c as static library ------------------------------------------
echo [compiling -- box2c]
if not exist box2c mkdir box2c
set box2c_warnings=-Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-variable
for /F "tokens=*" %%F in ('dir ..\code\3rdparty\box2c\src\*.c /b /s') do (
    clang -c %compile% %%F -o box2c/%%~nF.o %compile_commands% -std=c17 -I..\code\3rdparty\box2c\extern\simde %box2c_warnings%
)
llvm-lib /OUT:box2c.lib box2c/*.o
echo [compiling done -- box2c]

:: --- Build Game and link Box2c ----------------------------------------------
echo [compiling -- game]
clang++ %compile% box2c.lib ..\code\game\game_main.cpp  %compile_link% %out%game.exe %compile_commands% -std=c++20
popd

if %ERRORLEVEL% neq 0 (
  echo [compiling failed -- game]
  goto :end
)
echo [compiling done -- game]

:: --- Compile Commands Madness  ----------------------------------------------
:: Clang generates the compile_commands.json, but the tools don't like it because:
::  1. It's not an array
::  2. It ends with a ','
:: This script simply removes the comma and adds [] around the file contents, so tools
:: don't complain.


if not "%clang%"=="1" (
    echo [compile_commands.json not available]
    goto :end
)

set "file=compile_commands.json"
set "tempFile=temp_%RANDOM%.txt"

:: Check if the compile_commands.json file exists
if not exist "%file%" (
    echo [%file% not found]
    goto :end
)

:: Create a new temp file with '[' at the beginning
echo [ > "%tempFile%"

:: Copy the original file content except for the last character to the temp file
setlocal enabledelayedexpansion
set "prevLine="
for /f "delims=" %%i in (%file%) do (
    if defined prevLine echo(!prevLine!>>"%tempFile%"
    set "prevLine=%%i"
)

:: Remove the last character from the last line (assuming it's a comma or similar)
set "lastLine=!prevLine:~0,-1!"
echo(!lastLine!>>"%tempFile%"

:: Append ']' to the end of the temp file
echo ]>>"%tempFile%"

:: Replace the original file with the new temp file
move /Y "%tempFile%" "%file%" >nul
echo [compile_commands.json generated]

:: --- Unset ------------------------------------------------------------------
:end
for %%a in (%*) do set "%%a=0"
set game=
set compile=
set compile_link=
set out=
set debug=

if %ERRORLEVEL% neq 0 (
    exit /b 1
)
exit /b

:show_help
echo Usage: build [-h^|--help] [release^|debug] [asan^|ubsan]
echo.
echo Script used for building the game. When using clang, it generates `compile_commands.json`.
echo It is assuming that `clang++` is present in the program PATH.
echo Example usage:
echo.
echo      build clang release
echo      build clang debug asan ubsan
echo.

