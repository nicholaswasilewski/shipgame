#ifndef _GAME_H__
#define _GAME_H__

#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;
typedef uint8_t byte;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

typedef int32_t bool32;

struct gameData {
    bool Initialized;

    uint32 VertexBufferID;
    uint32 ColorBufferID;
    uint32 UVBufferID;
    uint32 ProgramID;
    uint32 MatrixID;

    uint32 TextureID;

    float BoxRotation;
};

struct controller {
    bool32 Up;
    bool32 Down;
    bool32 Left;
    bool32 Right;
};
#endif
