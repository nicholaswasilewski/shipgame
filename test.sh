#!/bin/sh

cc main.cpp -o test.out -Wall -Wno-write-strings -lX11 -lGL -lGLU -lm -D TESTING
if [ $? -ne 0 ]
then
    echo "Compilation Failed"
else
    echo "Compilation Successful"
    ./test.out
fi
