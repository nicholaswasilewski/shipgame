@echo off

IF NOT EXIST ..\bin mkdir ..\bin
pushd ..\bin

cl /nologo /Od /fp:fast ..\src\win32_glx.cpp /Z7 /link /incremental:no /opt:ref

popd
