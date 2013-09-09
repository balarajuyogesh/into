@echo off
mkdir %1\include
for /R %1\src\core %%i in (*.h) do copy /y %%i %1\include
for /R %1\src\ydin %%i in (*.h) do copy /y %%i %1\include
for /R %1\src\gui %%i in (*.h) do copy /y %%i %1\include
for /R %1\src\plugins %%i in (*.h) do copy /y %%i %1\include
for /R %1\demos %%i in (*.exe) do move %%i %%~pi\..
for /R %1\demos %%i in (*.exe) do rmdir %%~pi\release
move %1\intodemo.exe %1\demos
