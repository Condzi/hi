@echo off
setlocal

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
set cc_dir=         .codechecker
set cc_report_dir=   %cc_dir%/report/
set cc_http_dir=     %cc_dir%/report_http/
set cc_analyze=      call CodeChecker analyze
set cc_parse=        call CodeChecker parse
set cc_compile_cmds= compile_commands.json
set cc_analyzers=    clangsa clang-tidy cppcheck

if not exist %cc_dir% mkdir %cc_dir%
if not exist %cc_report_dir% set new=1

:: --- Run the analysis -------------------------------------------------------
if "%new%"=="1" %cc_analyze% -c -i skipfile.txt --output %cc_report_dir% %cc_compile_cmds% --analyzers %cc_analyzers% --analyzer-config 'clang-tidy:take-config-from-directory=true'

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
