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
    FBXT_StartChild,
    FBXT_EndChild,
    FBXT_Comma
};

char* fbx_token_type_string [] =
{
    "FBXT_Unknown   ",
    "FBXT_EndOfFile ",
    "FBXT_Endline   ",
    "FBXT_WhiteSpace",
    "FBXT_Comment   ",
    "FBXT_TypeName  ",
    "FBXT_String    ",
    "FBXT_Float     ",
    "FBXT_Integer   ",
    "FBXT_StartChild",
    "FBXT_EndChild  ",
    "FBXT_Comma     "
};

struct fbx_parse_info
{
    FILE* File;
    char* Token;
};

fbx_token_type global_TokenType;  
fbx_parse_info global_Info = {0};

char debug_valuestr[128];
char spaces[64];
char* MakeSpaces(int depth)
{
    memset(spaces,0,sizeof(spaces));
    memset(spaces,' ', depth*4);
    return spaces;
}

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
fbx_token_type ReadToken()
{
    int charCounter = 0;
    char c;
    fbx_token_type Type;

    // always skip whitespace
    while(IsWhiteSpace(c = fgetc(global_Info.File)));

    //comment start -- skip to end
    if (c == ';')
    {
        while((c = fgetc(global_Info.File)) != '\n');
    }
    
    if (c == '\n')
    {
        Type = FBXT_Endline;
    }
    else if (c == '{')
    {
        Type = FBXT_StartChild;
    }
    else if (c == '}')
    {
        Type = FBXT_EndChild;
    }
    //string start
    else if (c == '"')
    {
        Type = FBXT_String;
        while((c = fgetc(global_Info.File)) != '"')
        {
            global_Info.Token[charCounter++] = c;
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
        global_Info.Token[charCounter++] = c;
        
        while(1)
        {
            c = fgetc(global_Info.File);
            if (IsNumeric(c))
            {
                global_Info.Token[charCounter++] = c;
            }
            else if (c == '.')
            {
                global_Info.Token[charCounter++] = c;
                Type = FBXT_Float;
            }
            else
            {
                ungetc(c, global_Info.File);
                break;
            }
        }
    }
    //Type Name
    else if (IsAlpha(c))
    {
        Type = FBXT_TypeName;
        global_Info.Token[charCounter++] = c;
        while(1)
        {
            c = fgetc(global_Info.File);
            if (IsAlphanumeric(c))
            {
                global_Info.Token[charCounter++] = c;
            }
            else if (c == ':')
            {
                break;
            }
            else
            {
                ungetc(c, global_Info.File);
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
        DebugLog("  ReadFBXToken FBXT_Unknown %c\n", c);
        Type = FBXT_Unknown;
    }
    
    global_Info.Token[charCounter] = '\0';
    return Type;
}

// merged "properties" and "children" concepts into just one "node" concept
// A node will have a Name-Value pair, where Value is 0 or more values.
// A node may or may not have children.
// ex:  CreationTime has 1 value, 0 children.
//      CreationTime: "2016-08-16 15:19:02:000"
// ex:  CreationTimeStamp has 0 values, 8 children.
//      CreationTimeStamp:  { }
// ex:  Model has 2 values, 13 children
//      Model: "Model::Suzanne", "Mesh" { }
struct FBX_Node
{
    char* Name;
    char** Values;
    int ValueCount;

    FBX_Node* Children;
    int ChildCount;
};

bool ReadValues(FBX_Node* node)
{
    int startSeek = ftell(global_Info.File);

    fbx_token_type type;
    fbx_token_type lastType;
    int valueCount = 0;
    bool hasChildren = false;

    // scan to find value count
    while(1)
    {
        type = ReadToken();
        if(type == FBXT_StartChild)
        {
            hasChildren = true;
            break;
        }
        else if(type == FBXT_Endline && valueCount == 0)
        {
            // values were on a new line for some reason
            // no action
        }
        else if(type == FBXT_Endline && lastType != FBXT_Comma)
        {
            hasChildren = false;
            break;
        }
        else if(type == FBXT_Endline || type == FBXT_Comma)
        {
        }
        else 
        {
            valueCount++;
        }

        lastType = type;
        //DebugLog("PEEK  %d [%s] %s\n", startSeek, fbx_token_type_string[type], global_Info.Token);
    }

    // reset back top
    node->ValueCount = valueCount;
    node->Values = (char**) malloc(valueCount * sizeof(char*));
    fseek(global_Info.File, startSeek, SEEK_SET);

    // iterate and record values
    for(int i = 0; i < valueCount; i++)
    {        
        type = ReadToken();
        if(type == FBXT_Comma || type == FBXT_Endline)
        {
            i--;
            continue;
        }

        node->Values[i] = (char*)malloc((strlen(global_Info.Token)+1) * sizeof(char));
        strcpy(node->Values[i], global_Info.Token);
        //DebugLog("VALUE %d [%s] %s\n", startSeek, fbx_token_type_string[type], node->Values[i]);
    }

    return hasChildren;
}

void ParseChild(FBX_Node* parentNode, int depth)
{
    // TODO -- don't statically allocate 128 children.
    DebugLog("%s[START NODE] '%s'\n", MakeSpaces(depth), parentNode->Name);
    parentNode->Children = (FBX_Node*)malloc(sizeof(FBX_Node) * 128);

    while(1)
    {
        fbx_token_type TokenType = ReadToken();
        if (TokenType == FBXT_TypeName)
        {
            // a child is just any property of this node.
            // generally, one line in fbx is one child.
            FBX_Node node = {0};
            parentNode->Children[parentNode->ChildCount] = node;
            parentNode->ChildCount++;

            node.Name = (char*)malloc((strlen(global_Info.Token)+1) * sizeof(char));
            strcpy(node.Name, global_Info.Token);
            
            // parse all values of this property
            // will fill node.Values and node.ValueCount
            bool hasChildren = ReadValues(&node);
            
            // debug out a couple values from this node
            memset(debug_valuestr, 0, sizeof(debug_valuestr));
            for(int i = 0; i < 6; i++)
            {
                if(i < node.ValueCount)
                {
                    strcat(debug_valuestr, node.Values[i]);
                    if(i != node.ValueCount - 1)
                    {
                        strcat(debug_valuestr, ", ");
                    }
                }
            }
            DebugLog("%s[PROPERTY   ] '%s': %s\n",  MakeSpaces(depth), node.Name, debug_valuestr, node.ValueCount);

            // recurse this child
            if (hasChildren)
            {
                ParseChild(&node, depth + 1);
            }
        }
        else if (TokenType == FBXT_EndChild)
        {
            DebugLog("%s[END NODE  ] '%s' PropCount: %d\n",  MakeSpaces(depth), parentNode->Name, parentNode->ChildCount);
            break;
        }
        else if (TokenType == FBXT_EndOfFile)
        {
            DebugLog("[END File] '%s' PropCount: %d\n", parentNode->Name, parentNode->ChildCount);
            break;
        }
        else if (TokenType == FBXT_Endline ||
                 TokenType == FBXT_StartChild)
        {
            continue;
        }
        else 
        {
            DebugLog("huh? [%s]%s\n", fbx_token_type_string[TokenType], global_Info.Token);
        }
    }
}

void ParseFBX(FILE* File, FBX_Node* outFbxNode)
{ 
    char Token[MAX_TOKEN_LENGTH];
    global_Info.File = File; 
    global_Info.Token = Token;

    // the outer most section of properties in the fbx doc 
    // will be the root node.
    outFbxNode->Name = "Root";

    // fill in properties and attach to the root node
    ParseChild(outFbxNode, 0);
}

#endif
