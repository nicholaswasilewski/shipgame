if [[ $OSTYPE == *"darwin"* ]]; then
    gcc -std=c++14 -Wno-deprecated-declarations -framework Carbon -framework Cocoa -framework OpenGL osx_shipgame.mm -o ../bin/osx_shipgame
else
    cc linux_glx.cpp -o gl.out -Wall -Wno-write-strings -lX11 -lGL -lGLU -lm
fi
