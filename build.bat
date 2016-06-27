@echo off
set CommonCompilerFlags=-DMARCY_DEBUG=1 -DMARCY_SLOW=1 -WX -W4 -wd4201 -wd4100 -wd4189 -MT -Gm- -GR- -EHa- -Od -Oi -FC -Z7 -nologo -Fmwin32_marcy.map
set CommonLinkererFlags=-opt:ref kernel32.lib user32.lib gdi32.lib winmm.lib
ibt.exe -begin marcy.ibt
pushd .\bin
cl %CommonCompilerFlags% ..\code\win32_platform.cpp /link -subsystem:windows,5.2 %CommonLinkererFlags%
REM cl %CommonCompilerFlags% ..\code\win32_platform.cpp /link -subsystem:windows,5.1 %CommonLinkererFlags%
set LastError=%ERRORLEVEL%
popd
ibt.exe -end marcy.ibt %LastError%
echo on 