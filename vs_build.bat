@echo off

IF NOT EXIST .\bin mkdir .\bin
pushd .\bin

call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64   
set CommonCompilerFlags=/Od /MTd /fp:fast /I ../ext/headers /nologo /Z7
set CommonLinkerFlags=/incremental:no /opt:ref /LIBPATH:"../ext/lib/win64"
rem set CommonLinkerFlags=/incremental:no /opt:ref /LIBPATH:"../ext/lib/win32"
cl %CommonCompilerFlags% ..\src\win32_shipgame.cpp /link %CommonLinkerFlags%

if %errorlevel%==0 start win32_shipgame.exe

popd
