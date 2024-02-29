:: Based on RAD Debugger project `build.bat` file.

@echo off
setlocal
call cls
cd /D "%~dp0"

:: --- Usage Notes (2024/02/14) ------------------------------------------------
::
:: This is a central build script for the game. It takes a list
:: of simple alphanumeric-only arguments which control (a) what is built, (b) extra 
:: high-level build options. By default, if no options are passed, then the main 
:: "game" is built.
::
:: Below is a non-exhaustive list of possible ways to use the script:
:: `build game`
:: `build game release`
:: `build game asan`
::
:: For a full list of possible build targets and their build command lines,
:: search for @build_targets in this file.
::
:: Below is a list of all possible non-target command line options:
::
:: - `asan`: enable address sanitizer
:: - `ubsan`: enable undefined behavior sanitizer
:: - `msvc`: compile with MSVC (needs x64 native tools cmd prompt)
:: - `clang`: compile with clang++

:: --- Unpack Arguments -------------------------------------------------------
for %%a in (%*) do set "%%a=1"

if "%clang%"=="1"   echo [compiling with clang++]
if "%msvc%"=="1"    echo [compiling with cl]
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0   && echo [release mode]

:: --- Unpack Command Line Build Arguments ------------------------------------
if "%asan%"=="1"    echo [asan enabled]
if "%ubsan%"=="1"   echo [ubsan enabled]


set msvc_sanitizer=
set clang_sanitizer=
if "%asan%"=="1"      set msvc_sanitizer= -fsanitize=address 
if "%asan%"=="1"      set clang_sanitizer=-fsanitize=address -fno-omit-frame-pointer

if "%ubsan%"=="1"     set msvc_sanitizer= -fsanitize=undefined
if "%ubsan%"=="1"     set clang_sanitizer=-fsanitize=undefined -fno-omit-frame-pointer


:: --- MSVC  -----------------------------------------------------------
::  1. Set Windows Defines (no unicode!)
::  2. Disable Exceptions and RTTI
::  3. Disable warnings about insecure C functions (maybe remove that later?)
::  4. Enable all warnings and treat them as errors. Do not analyze external libraries.
::     (which are specified by angle brackets <> in include directives)
::  5. Check for buffer oveflow (/GS) and additional security checks
::  6. Disable warnings: 
::     - unused functions,
::     - spectre, 
::     - noexcept, 
::     - additional padding, 
::     - nonstandard nameless struct and unions
::     - initialization of subobjects in nameless unions should be wrapped in braces
::     - automatic inline expansion
::     - function not inlined
::     - enumerator not explicitely handled in a switch label
set msvc_defines=           /DWIN32 /D_WINDOWS /D_HAS_EXCEPTIONS=0 /D_CRT_SECURE_NO_WARNINGS
set msvc_disabled_warnings= /wd4505 /wd5045 /wd4577 /wd4820 /wd4201 /wd5246 /wd4710 /wd4711 /wd4061
set msvc_misc=              /GR- /Wall /WX /external:anglebrackets /external:W0 /GS /sdl /utf-8
set msvc_all=               %msvc_sanitizer% %msvc_defines% %msvc_disabled_warnings% %msvc_misc%

:: --- Compile/Link Line Definitions ------------------------------------------
set cl_common=        /I..\code\ /I..\code\3rdparty\ /nologo /FC /Z7 /std:c++20
set cl_debug=         call cl /Od %cl_common% %mscv_all%
set cl_release=       call cl /O2 /DNDEBUG %cl_common% %mscv_all%
set cl_link=          /link /MANIFEST:EMBED /INCREMENTAL:NO
set cl_out=           /out:

:: --- clang++  ---------------------------------------------------------------
set clang_disabled_warnings= -Wno-format -Wno-pragma-once-outside-header -Wno-gcc-compat -Wno-missing-field-initializers -Wno-missing-braces -Wno-unused-function

set clang_defines=    -DWIN32 -D_WINDOWS -D_HAS_EXCEPTIONS=0 -D_CRT_SECURE_NO_WARNING
set clang_misc=       -fno-exceptions -fno-rtti -ferror-limit=0 -Wall -Wextra -Werror
set clang_common=     -I..\code\ -I..\code\3rdparty -std=c++20 %clang_defines% %clang_misc% %clang_disabled_warnings%
set clang_debug=      call clang++ -g -O0 %clang_common% %clang_sanitizer%
set clang_release=    call clang++ -Ofast -DNDEBUG %clang_common%
set clang_out=        -o 
set clang_link=       -fuse-ld=lld-link -Wl,/MANIFEST:EMBED,/INCREMENTAL:NO

:: This is bullshit workaround for broken UBSAN on windows.
if "%ubsan%"=="1"      set clang_link=%clang_link%,"C:\Program Files\LLVM\lib\clang\18\lib\windows\clang_rt.ubsan_standalone-x86_64.lib","C:\Program Files\LLVM\lib\clang\18\lib\windows\clang_rt.ubsan_standalone_cxx-x86_64.lib"
:: --- Choose Compile/Link Lines ----------------------------------------------
set compile_commands=

if "%msvc%"=="1"      set compile_debug=   %cl_debug%
if "%msvc%"=="1"      set compile_release= %cl_release%
if "%msvc%"=="1"      set compile_link=    %cl_link%
if "%msvc%"=="1"      set out=             %cl_out%

if "%clang%"=="1"     set compile_debug=     %clang_debug% 
if "%clang%"=="1"     set compile_release=   %clang_release%
if "%clang%"=="1"     set compile_link=      %clang_link%
if "%clang%"=="1"     set out=               %clang_out%
if "%clang%"=="1"     set compile_commands=  -MJ ../compile_commands.json


if "%debug%"=="1"     set compile=%compile_debug%
if "%release%"=="1"   set compile=%compile_release%

:: --- Prep Directories -------------------------------------------------------
if not exist build mkdir build

:: --- Build Everything (@build_targets) --------------------------------------
pushd build
del game.exe
%compile%  ..\code\game\game_main.cpp  %compile_link% %out%game.exe %compile_commands%
popd

if %ERRORLEVEL% neq 0 (
  echo [compilation failed]
  goto :end
)
echo [compiled]

:: --- Compile Commands Madness  ----------------------------------------------
:: Clang generates the compile_commands.json, but the tools don't like it because:
::  1. It's not an array
::  2. It ends with a ','
:: This script simply removes the comma and adds [] around the file contents, so tools
:: don't complain.


if not "%clang%"=="1" (
    echo [compile_commands.json not available]
    goto end
)

set "file=compile_commands.json"
set "tempFile=temp_%RANDOM%.txt"

:: Check if the compile_commands.json file exists
if not exist "%file%" (
    echo [%file% not found]
    goto end
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
set msvc=
set debug=
set msvc_disabled_warnings=
set msvc_defines=
set msvc_misc=

if %ERRORLEVEL% neq 0 (
    exit /b 1
)
