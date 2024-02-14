:: Based on RAD Debugger project `build.bat` file.

@echo off
setlocal
call cls
cd /D "%~dp0"

call where cl.exe >nul 2>nul
if not "%ERRORLEVEL%" == "0" (
  echo [cl not found]
) else (echo [cl found])

call where clang++.exe >nul 2>nul
if not "%ERRORLEVEL%" == "0" (
  echo [clang-++ not found]
) else (echo [clang-++ found])


:: --- Usage Notes (2024/1/20) ------------------------------------------------
::
:: This is a central build script for the RAD Debugger project. It takes a list
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
:: - `msvc`: compile with MSVC (needs x64 native tools cmd prompt)
:: - `clang`: compile with clang++

:: --- Unpack Arguments -------------------------------------------------------
for %%a in (%*) do set "%%a=1"
if not "%clang%"=="1" set msvc=1
if not "%release%"=="1" set debug=1

if "%clang%"=="1"   echo [compiling with clang++]
if "%msvc%"=="1"    echo [compiling with cl]
if "%debug%"=="1"   set release=0 && echo [debug mode]
if "%release%"=="1" set debug=0 && echo [release mode]
if "%~1"==""        echo [default mode, assuming `game` build] && set game=1

:: --- Unpack Command Line Build Arguments ------------------------------------
if "%asan%"=="1"    echo [asan enabled]

set msvc_asan=
set clang_asan=
if "%asan%"=="1"      set msvc_asan= -fsanitize=address
if "%asan%"=="1"      set clang_asan=-fsanitize=address -fno-omit-frame-pointer


:: --- Custom stuff -----------------------------------------------------------
:: # 1. Set Windows Defines (no unicode!)
:: # 2. Disable Exceptions and RTTI
:: # 3. Disable warnings about insecure C functions (maybe remove that later?)
:: # 4. Enable all warnings and treat them as errors. Do not analyze external libraries.
:: #    (which are specified by angle brackets <> in include directives)
:: # 5. Check for buffer oveflow (/GS) and additional security checks
:: # 6. Disable warnings: 
:: #    - unused functions,
:: #    - spectre, 
:: #    - noexcept, 
:: #    - additional padding, 
:: #    - nonstandard nameless struct and unions
:: #    - initialization of subobjects in nameless unions should be wrapped in braces
:: #    - automatic inline expansion
:: #    - function not inlined
:: #    - enumerator not explicitely handled in a switch label
set msvc_defines=           /DWIN32 /D_WINDOWS /D_HAS_EXCEPTIONS=0 /D_CRT_SECURE_NO_WARNINGS
set msvc_disabled_warnings= /wd4505 /wd5045 /wd4577 /wd4820 /wd4201 /wd5246 /wd4710 /wd4711 /wd4061
set msvc_misc=              /GR- /Wall /WX /external:anglebrackets /external:W0 /GS /sdl /utf-8
set msvc_all=               %msvc_asan% %msvc_defines% %msvc_disabled_warnings% %msvc_misc%

:: --- Compile/Link Line Definitions ------------------------------------------
set cl_common=        /I..\code\ /I..\code\3rdparty\ /nologo /FC /Z7 /std:c++20
set cl_debug=         call cl /Od %cl_common% %mscv_all%
set cl_release=       call cl /O2 /DNDEBUG %cl_common% %mscv_all%
set cl_link=          /link /MANIFEST:EMBED /INCREMENTAL:NO
set cl_out=           /out:

set clang_disabled_warnings= -Wno-format -Wno-pragma-once-outside-header -Wno-gcc-compat -Wno-missing-field-initializers -Wno-missing-braces -Wno-unused-function

set clang_defines=    -DWIN32 -D_WINDOWS -D_HAS_EXCEPTIONS=0 -D_CRT_SECURE_NO_WARNING
set clang_misc=       -fno-exceptions -fno-rtti -Wall -Wextra -Werror
set clang_common=     -I..\code\ -I..\code\3rdparty -std=c++20 %clang_defines% %clang_misc% %clang_disabled_warnings%
set clang_debug=      call clang++ -g -O0 %clang_common% %clang_asan%
set clang_release=    call clang++ -O3 -DNDEBUG %clang_common%
set clang_out=         -o 
set clang_link=       

:: --- Choose Compile/Link Lines ----------------------------------------------
if "%msvc%"=="1"      set compile_debug=   %cl_debug%
if "%msvc%"=="1"      set compile_release= %cl_release%
if "%msvc%"=="1"      set compile_link=    %cl_link%
if "%msvc%"=="1"      set out=             %cl_out%

if "%clang%"=="1"     set compile_debug=   %clang_debug% 
if "%clang%"=="1"     set compile_release= %clang_release%
if "%clang%"=="1"     set compile_link=    %clang_link%
if "%clang%"=="1"     set out=             %clang_out%


if "%debug%"=="1"     set compile=%compile_debug%
if "%release%"=="1"   set compile=%compile_release%

:: --- Prep Directories -------------------------------------------------------
if not exist build mkdir build

:: --- Build Everything (@build_targets) --------------------------------------
pushd build
del game.exe
if "%game%"=="1" %compile%  ..\code\game\game_main.cpp  %compile_link% %out%game.exe
popd

:: --- Unset ------------------------------------------------------------------
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
