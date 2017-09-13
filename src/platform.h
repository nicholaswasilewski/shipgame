#ifndef PLATFORM_H__
#define PLATFORM_H__

#include "glHelper.cpp"
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

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))
#define Max(A, B) ((A)>(B)?(A):(B))
#define Min(A, B) ((A)<(B)?(A):(B))
#define Clamp(A, B, C) Min(Max((A), (B)), (C))
    
#ifndef RELEASE
#define Assert(Expression)                      \
    if(!(Expression))                           \
    {                                           \
        (*(int*)0 = 0);                         \
    }

#define DebugLog(Format, ...) \
    printf("%s:%d: " Format, __FILE__, __LINE__, __VA_ARGS__);

#else
#define Assert(Expression, Message) {};
#define DebugLog(Format, ...) {};
#endif


struct stack
{
    int Count;
    void* Base;
};

typedef struct memory_arena
{
    size_t Size;
    uint8 *Base;
    size_t Used;
} memory_arena;

#define PushSize(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
void*
PushSize_(memory_arena *Arena, size_t Size)
{
    Assert((Arena->Used + Size) <= (Arena->Size));
    void *NewSpace = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return NewSpace;
}

inline void
InitArena(memory_arena *Arena, size_t Size, uint8 *Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

inline memory_arena
PushArena(memory_arena *Arena, size_t Size)
{
    memory_arena Result = {0};
    Result.Size = Size;
    Result.Used = 0;
    Result.Base = (uint8 *)PushSize_(Arena, Size);
    return Result;
}

size_t ArenaSizeRemaining(memory_arena *Arena)
{
    return Arena->Size - Arena->Used;
}

struct button_state 
{
    bool32 Down;
};

struct mouse
{
    int Moved;
    int X;
    int Y;
    
    union
    {
	button_state Buttons[3];
	struct
	{
	    button_state Mouse1;
	    button_state Mouse2;
	    button_state Mouse3;
	};
    };
};

struct stick_state
{
    float X;
    float Y;
};

struct controller
{
    union
    {
	button_state Buttons[16];
	struct
	{
	    button_state Up;
	    button_state Down;
	    button_state Forward;
	    button_state Back;
	    button_state Left;
	    button_state Right;
	    button_state ActionUp;
	    button_state ActionDown;
	    button_state ActionLeft;
	    button_state ActionRight;
	    button_state LeftShoulder;
	    button_state RightShoulder;
	    button_state UpperLeft;
	    button_state UpperRight;
	    button_state Start;
	    button_state Select;
	};
    };

    stick_state LeftTrigger;
    stick_state RightTrigger;
    stick_state LeftStick;
    stick_state RightStick;
	
};

struct input
{
    float dT;
    mouse Mouse;
    controller Keyboard;
};

    
typedef struct game_screen_buffer
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
} game_screen_buffer, game_offscreen_buffer;


//#define PLATFORM_READ_WHOLE_FILE(

typedef struct platform_functions
{
    
} platform_functions;

typedef struct platform_data
{
    int32 MainMemorySize;
    void* MainMemory;
    int32 TempMemorySize;
    void* TempMemory;
    int32 TotalMemorySize;

    input *LastInput;
    input *NewInput;

    int WindowWidth;
    int WindowHeight;
    
    int VRBufferWidth;
    int VRBufferHeight;
    FramebufferDesc *LeftEye;
    FramebufferDesc *RightEye;
} platform_data;

#endif
