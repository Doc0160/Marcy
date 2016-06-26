@echo off
bin\ibt.exe -begin marcy.ibt
cd .\bin
cl -FC -Zi -nologo ..\code\win32_platform.cpp kernel32.lib user32.lib Gdi32.lib 
set LastError=%ERRORLEVEL%
cd ..
bin\ibt.exe -end marcy.ibt %LastError%
echo on