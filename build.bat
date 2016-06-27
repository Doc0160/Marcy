@echo off
set CommonCompilerFlags=-DMARCY_DEBUG=1 -DMARCY_SLOW=1 -DMARCY_WIN32=1-WX -W4 -wd4201 -wd4100 -wd4189 -MT -Gm- -GR- -EHa- -Od -Oi -FC -Z7 -nologo
set CommonLinkererFlags=-opt:ref kernel32.lib user32.lib gdi32.lib winmm.lib
echo "%MODE%"
ibt.exe -begin marcy.ibt
ibt.exe -begin central.ibt
pushd .\bin
cl %CommonCompilerFlags% ..\code\marcy.cpp -Fmmarcy.map /DLL
set LastError=%ERRORLEVEL%
..\ibt.exe -end ..\central.ibt %LastError%
..\ibt.exe -begin ..\win32.ibt
if "%MODE%" == "x64" (
	cl %CommonCompilerFlags% ..\code\win32_platform.cpp -Fmwin32_marcy.map /link -subsystem:windows,5.2 %CommonLinkererFlags%
) else (
	cl %CommonCompilerFlags% ..\code\win32_platform.cpp -Fmwin32_marcy.map /link -subsystem:windows,5.1 %CommonLinkererFlags%
)
set LastError=%ERRORLEVEL%
popd
ibt.exe -end win32.ibt %LastError%
ibt.exe -end marcy.ibt %LastError%
echo on