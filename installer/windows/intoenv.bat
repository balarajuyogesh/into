@echo off
setLocal EnableDelayedExpansion

rem Set your QTDIR-directory yourself
rem set QTDIR=c:\this\is\my\qtdir

rem -- Get the directory of the bat-file --
set BATDIR=%~dp0

rem -- Trying to find latest QTDIR
rem -- We will try to find all qt-directories from c:\
rem -- If your qt is somewhere other place, please
rem -- set it in QTDIR-variable above and remove 'rem'
if not "%QTDIR%" == "" goto :qtdiravailable
  set TEXT=Searhing qt-directories...
  set COUNTER=1
  PUSHD c:\
  for /f %%Q in ('dir /od /b /ad *qt*') do (
    set QTR=c:\%%Q
	PUSHD !QTR!
	for %%V in (qtvars qtenv) do (
		cls
		echo !TEXT!
		for /f %%R in ('dir /od /b /s %%V*.bat') do (
			set CHOICE!COUNTER!=%%R
			set /a COUNTER+=1
			cls
			echo !TEXT!
		)
	)
	POPD
	cls
	echo !TEXT!
  )
  POPD
  
  if "%COUNTER%" == "1" goto :warning
	set LASTSELECTION=I will set INTODIR myself.
    set CHOICE!COUNTER!=%LASTSELECTION%
	set /a COUNTER+=1
	echo --
	echo Select your Qt-variable file
	echo ----------------------------
	set C=1
	set /a COUNTER-=1
	:loop
	if !C! gtr !COUNTER! goto :choice
		echo !C!: !CHOICE%C%!
		set /a C+=1
		goto :loop
	:choice
	echo ----------------------------
	set /p SELECTION=Enter your choice: 
	echo --
	set QTVARSFILE=!CHOICE%SELECTION%!
	goto :intodir
:qtdiravailable

set QTVARSFILE=

rem -- Setting Qt-variables... --
if exist %QTDIR%\..\bin\qtvars.bat set QTVARSFILE=%QTDIR%\..\bin\qtvars.bat
if exist %QTDIR%\..\bin\qtenv.bat set QTVARSFILE=%QTDIR%\..\bin\qtenv.bat
if exist %QTDIR%\..\bin\qtenv2.bat set QTVARSFILE=%QTDIR%\..\bin\qtenv2.bat
if exist %QTDIR%\bin\qtvars.bat set QTVARSFILE=%QTDIR%\bin\qtvars.bat
if exist %QTDIR%\bin\qtenv.bat set QTVARSFILE=%QTDIR%\bin\qtenv.bat
if exist %QTDIR%\bin\qtenv2.bat set QTVARSFILE=%QTDIR%\bin\qtenv2.bat

:intodir
if "%QTVARSFILE%" == "" goto :warning
if "%QTVARSFILE%" == "%LASTSELECTION%" goto :warning

rem -- Write the temporary variable
echo %QTVARSFILE% > %BATDIR%\currentqtvars

endlocal

set BATDIR=%~dp0
set /p QTVARSFILE= <%BATDIR%\currentqtvars
call %QTVARSFILE%
set INTO=%BATDIR:~0,-5%
set INTODIR=%INTO%\into
set PATH=%INTO%\bin;%PATH%
set QT_PLUGIN_PATH=%INTO%\bin

echo --
echo *** Setting up Into environment... ***
echo -- INTODIR set to %INTODIR%
echo -- Adding %INTO%\bin to PATH
echo --

goto :end

:warning
echo **** WARNING ****
echo Couldn't find Qt-directory. 
echo Please set it in %BATDIR%intoenv.bat 
echo before using Into.
echo *****************
pause
exit

:end
cd %INTO%