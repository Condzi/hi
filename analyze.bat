:: Generate HTML Report based on CodeChecker run.

::
:: Run the analysis
::
call CodeChecker analyze --tidyargs .clang-tidy -i skipfile.txt --output W:\\hi\\.codechecker\\reports W:\\hi/compile_commands.json

::
:: Generate the report
::
call CodeChecker parse -e html .codechecker/reports -o .codechecker/reports_html

::
:: Open the browser
::
call start .codechecker/reports_html/index.html
