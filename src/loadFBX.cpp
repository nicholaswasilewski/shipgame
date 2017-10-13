#ifndef _LOADFBX_CPP__
#define _LOADFBX_CPP__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrixMath.cpp"

#define MAX_TOKEN_LENGTH 1024

enum fbx_token_type
{
    FBXT_Unknown,
    FBXT_EndOfFile,
    FBXT_Endline,
    FBXT_WhiteSpace,
    FBXT_Comment,
    FBXT_TypeName,
    FBXT_String,
    FBXT_Float,
    FBXT_Integer,
    FBXT_StartTuple,
    FBXT_EndTuple,
    FBXT_Comma
};

struct fbx_parse_info
{
    FILE* File;
    char* Token;
};

int IsAlpha(char c)
{
    return ((c >= 65 && c <= 90) ||
            (c >= 97 && c <= 122));
}

int IsNumeric(char c)
{
    return (c >= 48 && c <= 57);
}

int IsAlphanumeric(char c)
{
    return IsAlpha(c) ||
        IsNumeric(c);
}

int IsNumberStart(char c)
{
    return (IsNumeric(c)  ||
            (c == '-') ||
            (c == '.'));
}

int IsWhiteSpace(char c)
{
    return c == '\t' ||
        c == '\r' ||
        c == ' ';
}

//Returns 0 if EOF
fbx_token_type ReadFBXToken(FILE* file, char *outString)
{
    int charCounter = 0;
    char c = fgetc(file);

    fbx_token_type Type;

    if (IsWhiteSpace(c))
    {
        Type = FBXT_WhiteSpace;
    }
    else if (c == '\n')
    {
        Type = FBXT_Endline;
    }
    //comment start
    else if (c == ';')
    {
        Type = FBXT_Comment;
        while((c = fgetc(file)) != '\n');
    }
    else if (c == '{')
    {
        Type = FBXT_StartTuple;
    }
    else if (c == '}')
    {
        Type = FBXT_EndTuple;
    }
    //string start
    else if (c == '"')
    {
        Type = FBXT_String;
        while((c = fgetc(file)) != '"')
        {
            outString[charCounter++] = c;
        }
    }
    else if (IsNumberStart(c))
    {
        if (c == '.')
        {
            Type = FBXT_Float;
        }
        else
        {
            Type = FBXT_Integer;
        }
        outString[charCounter++] = c;
        
        while(1)
        {
            c = fgetc(file);
            if (IsNumeric(c))
            {
                outString[charCounter++] = c;
            }
            else if (c == '.')
            {
                outString[charCounter++] = c;
                Type = FBXT_Float;
            }
            else
            {
                ungetc(c, file);
                break;
            }
        }
    }
    //Type Name
    else if (IsAlpha(c))
    {
        Type = FBXT_TypeName;
        outString[charCounter++] = c;
        while(1)
        {
            c = fgetc(file);
            if (IsAlphanumeric(c))
            {
                outString[charCounter++] = c;
            }
            else if (c == ':')
            {
                break;
            }
            else
            {
                ungetc(c, file);
                break;
            }
        }
    }
    else if (c == ',')
    {
        return FBXT_Comma;
    }
    else if (c == EOF)
    {
        Type = FBXT_EndOfFile;
    }
    else 
    {
        Type = FBXT_Unknown;
    }
    
    outString[charCounter] = '\0';
    return Type;
}

fbx_token_type ReadToken(fbx_parse_info *Info)
{
    return ReadFBXToken(Info->File, Info->Token);
}

struct CreationTimeStamp
{
    int Version;
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int Millisecond;
};

struct OtherFlags
{
    int FlagPLE;
};

struct FBXHeaderExtension
{
    int FBXHeaderVersion;
    int FBXVersion;
    char Creator[128];
};

struct ObjectType
{
    int Count;
};

struct FBXDefinitions
{
    int Version;
    int Count;
    ObjectType Model;
    ObjectType Geometry;
    ObjectType Material;
    ObjectType Pose;
    ObjectType GlobalSettings;
};

struct Property {
    char StringProps[3][64];
    int IntProps[3];
    float FloatProps[3];
};

struct FBXModel {
    int VertexCount;
    float *Vertices;
    
    int IndexCount;
    int *Indices;
    
    int NormalCount;
    float *Normals;
    
    int UVCount;
    float *UVs;
};

struct FBXModelFile
{
    FBXHeaderExtension Header;
    char CreationTime[64];
    char Creator[64];
    FBXDefinitions Definitions;
};

int SeekToTypeName(fbx_parse_info *Info, char *SeekTo)
{
    fbx_token_type TokenType;
    while(1)
    {
        TokenType = ReadToken(Info);
        if (TokenType == FBXT_TypeName)
        {
            if (strcmp(Info->Token, SeekTo) == 0)
            {
                return 0;
            }
        }
        else if (TokenType == FBXT_EndOfFile)
        {
            return -1;
        }
    }
}

FBXModel ParseModel(fbx_parse_info *Info)
{
    FBXModel Error = {0};
    FBXModel Model = {0};
    fbx_token_type TokenType;
    
    if (SeekToTypeName(Info, "Vertices") != 0)
    {
        printf("Unexpected end of file.");
        return Error;
    }
    
    Model.VertexCount = 0;
    {
        int Start = ftell(Info->File);
        while(1)
        {
            TokenType = ReadToken(Info);
            if (TokenType == FBXT_WhiteSpace)
            {
                continue;
            }
            else if (TokenType == FBXT_Float)
            {
                ++Model.VertexCount;
                TokenType = ReadToken(Info);
                
                if (TokenType == FBXT_Comma)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (TokenType == FBXT_EndOfFile)
            {
                printf("Parse error\n");
                return Error;
            }
        }
        
        fseek(Info->File, Start, SEEK_SET);
        Model.Vertices = (float *)malloc(sizeof(float)*Model.VertexCount);
        float *Pointer = Model.Vertices;
        while(1)
        {
            TokenType = ReadToken(Info);
            if (TokenType == FBXT_Float)
            {
                *Pointer++ = atof(Info->Token);
                if (TokenType == FBXT_Comma)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (TokenType == FBXT_EndOfFile)
            {
                printf("Unexpected end of file.");
                return Error;
            }
        }
    }

    if (SeekToTypeName(Info, "PolygonVertexIndex") != 0)
    {
        printf("Unexpected end of file.");
        return Error;
    }
    {
        int Start = ftell(Info->File);
        while(1)
        {
            TokenType = ReadToken(Info);
            if (TokenType == FBXT_Integer)
            {
                ++Model.IndexCount;
                TokenType = ReadToken(Info);
                if (TokenType == FBXT_Comma)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (TokenType == FBXT_EndOfFile)
            {
                printf("Unexpected end of file.\n");
                return Error;
            }
        }
        fseek(Info->File, Start, SEEK_SET);
        Model.Indices = (int *)malloc(sizeof(int)*Model.IndexCount);
        int *Pointer = Model.Indices;
        while(1)
        {
            TokenType = ReadToken(Info);
            if (TokenType == FBXT_Integer)
            {
                *Pointer++ = atoi(Info->Token);
                TokenType = ReadToken(Info);
                if (TokenType == FBXT_Comma)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (TokenType == FBXT_EndOfFile)
            {
                printf("Unexpected end of file.\n");
                return Error;
            }
        }
    }
    
    if (SeekToTypeName(Info, "Normals") != 0)
    {
        printf("Unexpected end of file.\n");
        return Error;
    }
    {
        int Start = ftell(Info->File);
        while(1)
        {
            TokenType = ReadToken(Info);
            if (TokenType == FBXT_Float)
            {
                ++Model.NormalCount;
                TokenType = ReadToken(Info);
                if (TokenType == FBXT_Comma)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else if (TokenType == FBXT_EndOfFile)
            {
                printf("Unexpected end of file.\n");
                return Error;
            }
        }
        fseek(Info->File, Start, SEEK_SET);
        Model.Normals = (float*)malloc(sizeof(float)*Model.NormalCount);
    }

    return Model;
}

void ParseObjects(fbx_parse_info *Info)
{
    printf("Start Objects Parse\n");
    fbx_token_type TokenType;
    int Go = 1;
    while(Go)
    {
        TokenType = ReadToken(Info);
        if (TokenType == FBXT_TypeName)
        {
            if (strcmp("Model", Info->Token) == 0)
            {
                FBXModel model = ParseModel(Info);
                printf("Vertex Count: %d\n", model.VertexCount); 
                printf("Index Count: %d\n", model.IndexCount); 
                printf("Normal Count: %d\n", model.NormalCount); 
                Go = 0;
            }
        }
    }
}

FBXModel ParseFBX(FILE* File)
{
    fbx_token_type TokenType;    
    char Token[MAX_TOKEN_LENGTH];
    fbx_parse_info Info = {
        File,
        Token
    };

    while(1)
    {
        TokenType = ReadToken(&Info);
        if (TokenType == FBXT_TypeName)
        {
            if (strcmp(Info.Token, "Objects") == 0)
            {
                ParseObjects(&Info);
            }
        }
        else if (TokenType == FBXT_EndOfFile)
        {
            break;
        }
    }
    
    FBXModel Result = {0};
    return Result;
}

#endif
