#define WINDOWS

#include <stdio.h>
#include <windows.h>
#include <Windowsx.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

#include "glHelper.cpp"
#include "game.cpp"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "OpenGL32.lib")


int CALLBACK WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{
    
}
