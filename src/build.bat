@echo off

IF NOT EXIST ..\bin mkdir ..\bin
pushd ..\bin


set CommonCompilerFlags=/Od /MTd /fp:fast /I ../ext/headers /nologo /Z7
set CommonLinkerFlags=/incremental:no /opt:ref /LIBPATH:"../ext/lib/win64"
rem set CommonLinkerFlags=/incremental:no /opt:ref /LIBPATH:"../ext/lib/win32"
cl %CommonCompilerFlags% ..\src\win32_glx.cpp /link %CommonLinkerFlags%



popd
