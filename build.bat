@echo off
ibt.exe -begin marcy.ibt
pushd .\bin
cl -DMARCY_DEBUG=1 -DMARCY_SLOW=1 -WX -W4 -wd4201 -wd4100 -wd4189 -MT -Gm- -GR- -EHa- -Oi -FC -Z7 -nologo -Fmwin32_marcy.map ..\code\win32_platform.cpp /link -subsystem:windows,5.02 -opt:ref kernel32.lib user32.lib Gdi32.lib 
set LastError=%ERRORLEVEL%
popd
ibt.exe -end marcy.ibt %LastError%
echo on 