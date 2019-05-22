if [[ $OSTYPE == *"darwin"* ]]; then
    clang -std=c++14 -Wno-deprecated-declarations -framework Cocoa -framework OpenGL osx_shipgame.mm -o ../bin/osx_shipgame
else
    cc linux_glx.cpp -o gl.out -Wall -Wno-write-strings -lX11 -lGL -lGLU -lm
fi
