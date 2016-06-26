@echo off
ibt.exe -begin marcy.ibt
pushd .\bin
cl -DMARCY_DEBUG=1 -DMARCY_SLOW=1 -WX -W4 -wd4201 -wd4100 -wd4189 -GR- -EHa- -Oi -FC -Z7 -nologo ..\code\win32_platform.cpp kernel32.lib user32.lib Gdi32.lib 
set LastError=%ERRORLEVEL%
popd
ibt.exe -end marcy.ibt %LastError%
echo on 