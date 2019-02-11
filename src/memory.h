#ifndef MEMORY_H__
#define MEMORY_H__

#include "numerical_types.h"

#define TERABYTES(x) 1024*GIGABYTES(x)
#define GIGABYTES(x) 1024*MEGABYTES(x)
#define MEGABYTES(x) 1024*KILOBYTES(x)
#define KILOBYTES(x) 1024*x

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

#define PushObject(Arena, type) (type *)PushSize(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize(Arena, (Count)*sizeof(type))
void*
PushSize(memory_arena *Arena, size_t Size)
{
    Assert((Arena->Used + Size) <= (Arena->Size));
    void *NewSpace = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return NewSpace;
}

#define PopObject(Arena, type) PopSize(Arena, sizeof(type))
#define PopArray(Arena, Count, type) PopSize(Arena, (Count)*sizeof(type))
void PopSize(memory_arena *Arena, size_t Size)
{
    Assert(Arena->Used >= Size);
    Arena->Size += Size;
    Arena->Used -= Size;
}

void InitArena(memory_arena *Arena, size_t Size, uint8 *Base)
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
    Result.Base = (uint8 *)PushSize(Arena, Size);
    return Result;
}

size_t ArenaSizeRemaining(memory_arena *Arena)
{
    return Arena->Size - Arena->Used;
}

#endif