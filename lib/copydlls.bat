@echo off
setlocal enabledelayedexpansion

set MODE=%1
if not "%MODE%" == "" goto modeselected
set MODE=debug
:modeselected

for /R .. %%i in (%MODE%\*.dll) do copy /Y %%i .

endlocal
