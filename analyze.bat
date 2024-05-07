@echo off
setlocal
call cls
cd /D "%~dp0"

:: --- Usage Notes (2024/2/3) ------------------------------------------------
::
:: This is a script for running the analysis using CodeChecker. Internally, it
:: uses clang-tidy, clang-static-analyzer and cppcheck for analysis.
::
:: Supported arguments:
:: `analyze new`  -- create new analysis report
:: `analyze view` -- print the results to the stdout
:: `analyze web`  -- create a web report and open it in the browser
::

:: --- Unpack Arguments -------------------------------------------------------
for %%a in (%*) do set "%%a=1"

if not "%new%"=="1" set view=1
if not "%web%"=="1" set web=0

:: --- Definitions ------------------------------------------------------------
set cc_dir=              .codechecker
set cc_report_dir=       %cc_dir%/report/
set cc_http_dir=         %cc_dir%/report_http/
set cc_analyze=          call CodeChecker analyze --config codechecker.yml
set cc_parse=            call CodeChecker parse
set cc_compile_cmds=     build/compile_commands.json
set cc_analyzers=        --analyzers clangsa clang-tidy
set cc_analyzer_config=  --analyzer-config 'clang-tidy:take-config-from-directory=true' --analyzer-config clangsa:AllEnable=true

if not exist %cc_dir% mkdir %cc_dir%
if not exist %cc_report_dir% set new=1

:: --- Run the analysis -------------------------------------------------------
if "%new%"=="1" %cc_analyze% -c -i skipfile.txt --output %cc_report_dir% %cc_compile_cmds% %cc_analyzers% %cc_analyzer_config%

:: --- Generate the report ----------------------------------------------------
if "%web%"=="1" %cc_parse% -e html %cc_report_dir% -o %cc_http_dir%

:: --- Open the browser -------------------------------------------------------
if "%web%"=="1" call start %cc_http_dir%/index.html

:: --- Print in terminal ------------------------------------------------------
if "%view%"=="1" %cc_parse% --print-steps %cc_report_dir%

:: --- Unset ------------------------------------------------------------------
for %%a in (%*) do set "%%a=0"
set cc_dir=
set cc_report_dir=
set cc_http_dir=
set cc_analyze=
set cc_parse=
set cc_compile_cmds=
set cc_analyzers=
set cc_analyzer_config=
