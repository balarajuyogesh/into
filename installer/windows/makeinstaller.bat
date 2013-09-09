@echo off
set VERSION=%1
set LICENSE=%2

set COMPILER=mingw
if "%3" == "" goto mingwset
set COMPILER="%3"
:mingwset
set LICENSEFILE=Z:\development\licenses\into-%LICENSE%.txt
set LICENSEHEADER=Z:\development\licenses\into-%LICENSE%-header.txt

set ARCHITECTURE=win32
if "%4" == "" goto archset
set ARCHITECTURE="%4"
:archset

set LICENSECHANGER=%INTOWORKSPACE%\into\tools\licensechanger\licensechanger.exe

for /R ydin %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R gui %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R core %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R plugins %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R demos %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R tools %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R script %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i
for /R declarative %%i in (*.cc *.h) do "%LICENSECHANGER%" "%LICENSEHEADER%" %%i

set OUTFILE=into_%VERSION:-=%.%LICENSE%_%ARCHITECTURE%_%COMPILER%.exe
"%ProgramFiles(x86)%\NSIS\makensis.exe" /nocd /DVERSION=%VERSION% /DLICENSEFILE="%LICENSEFILE%" /DOUTFILE=%OUTFILE% /DCOMPILER=%COMPILER% installer\windows\installer.nsi
copy /Y %OUTFILE% z:\build\packages
