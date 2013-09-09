@echo off
setlocal enabledelayedexpansion

set MODE=%1
if not "%MODE%" == "" goto modeselected
set MODE=release
:modeselected

cd ..

for /R src %%i in (%MODE%\*.dll) do copy /Y %%i bin
for /R src %%i in (%MODE%\*.a) do copy /Y %%i lib
for /R src\demos %%i in (%MODE%\*.exe) do call :CopyDemo %%~pi %%~nxi %MODE%
copy /Y src\tools\demo\%MODE%\demo.exe demos\intodemo.exe
copy /Y src\tools\jsrunner\%MODE%\jsrunner.exe bin
move /Y bin\piiscript.dll bin\script
move /Y bin\piideclarative.dll bin\declarative

goto end

:CopyDemo %1 %2 %3
set SRC=%1%2
set DEST=%SRC:src\demos=demos%
set DEST=!DEST:%3\=!
copy /Y %SRC% %DEST%
exit /b

:end
set MODE=

cd src

endlocal
