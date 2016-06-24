@cd archive
@set hr=%time:~0,2%
@if "%hr:~0,1%" equ " " set hr=0%hr:~1,1%
"C:\Program Files\7-Zip\7z" a %date:~0,2%-%date:~3,2%-%date:~6,4%--%hr%-%time:~3,2%-%time:~6,2%--%1 ..\code
@cd ..