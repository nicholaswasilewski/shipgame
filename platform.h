#ifndef PLATFORM_H__
#define PLATFORM_H__

#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;
typedef uint8_t byte;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

typedef int32_t bool32;

#define TERABYTES(x) 1024*GIGABYTES(x)
#define GIGABYTES(x) 1024*MEGABYTES(x)
#define MEGABYTES(x) 1024*KILOBYTES(x)
#define KILOBYTES(x) 1024*x

#define SWAP(x,y,T)					\
        {						\
	    T swap = x;					\
	    x = y;					\
	    y = swap;					\
	}
    

struct button_state 
{
    bool32 Down;
};

struct stick_state
{
    float X;
    float Y;
};

struct controller
{
    button_state Up;
    button_state Down;
    button_state Forward;
    button_state Back;
    button_state Left;
    button_state Right;

    button_state UpperLeft;
    button_state UpperRight;

    stick_state LStick;
    stick_state RStick;
};

struct input
{
    float dT;
    controller Keyboard;
};

struct platform_data
{
    int32 MainMemorySize;
    void* MainMemory;
    int32 TempMemorySize;
    void* TempMemory;

    input *LastInput;
    input *NewInput;
};

#endif
