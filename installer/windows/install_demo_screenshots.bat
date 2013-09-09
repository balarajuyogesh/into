@echo off
set SS=z:\development\intodemo\screenshots
for %%i in (%SS%\*.png) do call :CopyFile %SS% %%~ni
exit /b

:CopyFile %1 %2
set T=%2
set T=%T:_=\%
copy %1\%2.png demos\%T%.png
exit /b
