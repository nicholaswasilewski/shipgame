#ifndef _WIN32_PLATFORM_H__

#include "platform.h"

typedef struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
} win32_offscreen_buffer;

typedef struct win32_state
{
    bool Running;
    bool ConsoleVisible;
    bool Focused;
    win32_offscreen_buffer Backbuffer;
    HWND MainWindow;
    WINDOWPLACEMENT PreviousWindowPlacement;
    HWND Console;

    int WindowWidth;
    int WindowHeight;

    uint64 TotalSize;
    void* GameMemoryBlock;

    vr::IVRSystem *VRSystem;
} win32_state;

typedef struct win32_window_dimension {
    int Width;
    int Height;
} win32_window_dimension;

typedef struct win32_point {
    int X;
    int Y;
} win32_point;

#define _WIN32_PLATFORM_H__
#endif
