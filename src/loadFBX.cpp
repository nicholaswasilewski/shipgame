#ifndef _LOADFBX_CPP__
#define _LOADFBX_CPP__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "matrixMath.cpp"
#include "graphics.cpp"

#define MAX_TOKEN_LENGTH 1024

#define FBX_TOKEN_LIST \
FTL(FBXT_Unknown   ) \
FTL(FBXT_EndOfFile ) \
FTL(FBXT_Endline   ) \
FTL(FBXT_Whitespace) \
FTL(FBXT_Comment   ) \
FTL(FBXT_TypeName  ) \
FTL(FBXT_String    ) \
FTL(FBXT_Float     ) \
FTL(FBXT_Integer   ) \
FTL(FBXT_StartChild) \
FTL(FBXT_EndChild  ) \
FTL(FBXT_Comma     )

#define FTL(TokenName) TokenName,
enum fbx_token_type
{
    FBX_TOKEN_LIST
};

#undef FTL
#define FTL(TokenName) #TokenName,
char* fbx_token_type_string [] =
{
    FBX_TOKEN_LIST
};
#undef FTL

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
    
    if (c == '\n' || c == '\r')
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
    
    FBX_Node** Children;
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

void ParseChild(memory_arena *Memory, FBX_Node* parentNode, int depth)
{
    // TODO -- don't statically allocate 128 children.
    //DebugLog("%s[START NODE] '%s'\n", MakeSpaces(depth), parentNode->Name);
    parentNode->Children = (FBX_Node**)malloc(sizeof(FBX_Node*) * 128);
    parentNode->ChildCount = 0;
    
    while(1)
    {
        fbx_token_type TokenType = ReadToken();
        if (TokenType == FBXT_TypeName)
        {
            // a child is just any property of this node.
            // generally, one line in fbx is one child.
            FBX_Node* node = (FBX_Node*) malloc(sizeof(FBX_Node));
            parentNode->Children[parentNode->ChildCount] = node;
            parentNode->ChildCount++;
            
            node->Name = (char*)malloc((strlen(global_Info.Token)+1) * sizeof(char));
            strcpy(node->Name, global_Info.Token);
            
            // parse all values of this property
            // will fill node.Values and node.ValueCount
            bool hasChildren = ReadValues(node);
            
            // debug out a couple values from this node
            memset(debug_valuestr, 0, sizeof(debug_valuestr));
            for(int i = 0; i < 6; i++)
            {
                if(i < node->ValueCount)
                {
                    strcat(debug_valuestr, node->Values[i]);
                    if(i != node->ValueCount - 1)
                    {
                        strcat(debug_valuestr, ", ");
                    }
                }
            }
            //DebugLog("%s[PROPERTY   ] '%s': %s\n",  MakeSpaces(depth), node->Name, debug_valuestr, node->ValueCount);
            
            // recurse this child
            if (hasChildren)
            {
                ParseChild(Memory, node, depth + 1);
            }
        }
        else if (TokenType == FBXT_EndChild)
        {
            //DebugLog("%s[END NODE  ] '%s' PropCount: %d\n",  MakeSpaces(depth), parentNode->Name, parentNode->ChildCount);
            break;
        }
        else if (TokenType == FBXT_EndOfFile)
        {
            //DebugLog("[END File] '%s' PropCount: %d\n", parentNode->Name, parentNode->ChildCount);
            break;
        }
        else if (TokenType == FBXT_Endline ||
                 TokenType == FBXT_StartChild)
        {
            continue;
        }
        else 
        {
            //DebugLog("huh? [%s]%s\n", fbx_token_type_string[TokenType], global_Info.Token);
        }
    }
}

FBX_Node* FBX_GetChildByName(FBX_Node* node, char* name)
{
    for(int i = 0; i < node->ChildCount; i++)
    {
        FBX_Node* child = node->Children[i];
        if (strcmp(name, child->Name) == 0)
        {
            return child;
        }
    }
    
    return NULL;
}

FBX_Node* ParseFBX(memory_arena *MainMemory, memory_arena *TempMemory, FILE* File)
{
    FBX_Node *outFbxNode = PushObject(TempMemory, FBX_Node);
    char Token[MAX_TOKEN_LENGTH];
    global_Info.File = File; 
    global_Info.Token = Token;
    
    // the outer most section of properties in the fbx doc 
    // will be the root node.
    outFbxNode->Name = "Root";
    
    // fill in properties and attach to the root node
    ParseChild(TempMemory, outFbxNode, 0);
    
    return outFbxNode;
}

void ProcessModelNode(memory_arena *Memory, model2 *Model, FBX_Node *modelNode)
{
    
    FBX_Node *vertexNode = FBX_GetChildByName(modelNode, "Vertices");
    if (vertexNode != NULL)
    {
        Model->VertexAttributeCount += 1;
    }
    Model->Vertices = (float*)PushArray(Memory, vertexNode->ValueCount, float);
    for (int i = 0; i < vertexNode->ValueCount; i++)
    {
        float value = atof(vertexNode->Values[i]);
        Model->Vertices[i] = value;
    }
    
    FBX_Node *indexNode = FBX_GetChildByName(modelNode, "PolygonVertexIndex");
    FBX_Node *normalNode = FBX_GetChildByName(FBX_GetChildByName(modelNode, "LayerElementNormal"), "Normals");
    
    //Count up how many triangles you need, double count quads
    int faceIndex = 0;
    int triangleCount = 0;
    int originalFaceCount = 0;
    int originalTriangleCount = 0;
    int originalQuadCount = 0;
    
    while(faceIndex < indexNode->ValueCount)
    {
        faceIndex++;
        faceIndex++;
        triangleCount += 1;
        if (atoi(indexNode->Values[faceIndex++]) > 0)
        {
            faceIndex++;
            triangleCount += 1;
            originalQuadCount += 1;
        }
        else
        {
            originalTriangleCount += 1;
        }
        originalFaceCount += 1;
    }
    int vertexCount = originalQuadCount*4+originalTriangleCount*3;
    Model->Vertices = PushArray(Memory, vertexCount*3, float);
    Model->VertexBufferSize = vertexCount*3*sizeof(float);
    Model->Normals = PushArray(Memory, vertexCount*3, float);
    
    Model->Indices = PushArray(Memory, triangleCount*3, uint16);
    Model->IndexCount = triangleCount*3;
    
    int i = 0;
    int vertexWriteIndex = 0;
    int normalWriteIndex = 0;
    int indexWriteIndex = 0;
    int normalIndex = 0;
    int indexIndex = 0;
    int indexCount = 0;
    while (i < triangleCount)
    {
        int quad = 1;
        
        int32 indices[4];
        float normals[12];
        float vertices[12];
        indices[0] = atoi(indexNode->Values[indexIndex++]);
        vertices[0] = atof(vertexNode->Values[indices[0]*3+0]);
        vertices[1] = atof(vertexNode->Values[indices[0]*3+1]);
        vertices[2] = atof(vertexNode->Values[indices[0]*3+2]);
        normals[0] = atof(normalNode->Values[normalIndex++]);
        normals[1] = atof(normalNode->Values[normalIndex++]);
        normals[2] = atof(normalNode->Values[normalIndex++]);
        
        indices[1] = atoi(indexNode->Values[indexIndex++]);
        vertices[3] = atof(vertexNode->Values[indices[1]*3+0]);
        vertices[4] = atof(vertexNode->Values[indices[1]*3+1]);
        vertices[5] = atof(vertexNode->Values[indices[1]*3+2]);
        normals[3] = atof(normalNode->Values[normalIndex++]);
        normals[4] = atof(normalNode->Values[normalIndex++]);
        normals[5] = atof(normalNode->Values[normalIndex++]);
        
        int thirdIndex = atoi(indexNode->Values[indexIndex++]);
        if (thirdIndex < 0) { quad = 0; thirdIndex = -thirdIndex-1;}
        indices[2] = thirdIndex;
        vertices[6] = atof(vertexNode->Values[indices[2]*3+0]);
        vertices[7] = atof(vertexNode->Values[indices[2]*3+1]);
        vertices[8] = atof(vertexNode->Values[indices[2]*3+2]);
        normals[6] = atof(normalNode->Values[normalIndex++]);
        normals[7] = atof(normalNode->Values[normalIndex++]);
        normals[8] = atof(normalNode->Values[normalIndex++]);
        
        uint16 indicesToWrite[4] = {0};
        indicesToWrite[0] = indexCount++;
        indicesToWrite[1] = indexCount++;
        indicesToWrite[2] = indexCount++;
        
        Model->Indices[indexWriteIndex++] = indicesToWrite[1];
        Model->Vertices[vertexWriteIndex++] = vertices[3];
        Model->Vertices[vertexWriteIndex++] = vertices[4];
        Model->Vertices[vertexWriteIndex++] = vertices[5];
        Model->Normals[normalWriteIndex++] = normals[3];
        Model->Normals[normalWriteIndex++] = normals[4];
        Model->Normals[normalWriteIndex++] = normals[5];
        
        Model->Indices[indexWriteIndex++] = indicesToWrite[0];
        Model->Vertices[vertexWriteIndex++] = vertices[0];
        Model->Vertices[vertexWriteIndex++] = vertices[1];
        Model->Vertices[vertexWriteIndex++] = vertices[2];
        Model->Normals[normalWriteIndex++] = normals[0];
        Model->Normals[normalWriteIndex++] = normals[1];
        Model->Normals[normalWriteIndex++] = normals[2];
        
        Model->Indices[indexWriteIndex++] = indicesToWrite[2];
        Model->Vertices[vertexWriteIndex++] = vertices[6];
        Model->Vertices[vertexWriteIndex++] = vertices[7];
        Model->Vertices[vertexWriteIndex++] = vertices[8];
        Model->Normals[normalWriteIndex++] = normals[6];
        Model->Normals[normalWriteIndex++] = normals[7];
        Model->Normals[normalWriteIndex++] = normals[8];
        i++;
        
        if (quad)
        {
            Model->Indices[indexWriteIndex++] = indicesToWrite[1];
            Model->Indices[indexWriteIndex++] = indicesToWrite[2];
            
            indices[3] = -atoi(indexNode->Values[indexIndex++])-1;
            vertices[9] = atof(vertexNode->Values[indices[3]*3+0]);
            vertices[10] = atof(vertexNode->Values[indices[3]*3+1]);
            vertices[11] = atof(vertexNode->Values[indices[3]*3+2]);
            normals[9] = atof(normalNode->Values[normalIndex++]);
            normals[10] = atof(normalNode->Values[normalIndex++]);
            normals[11] = atof(normalNode->Values[normalIndex++]);
            
            indicesToWrite[3] = indexCount++;
            
            Model->Indices[indexWriteIndex++] = indicesToWrite[3];
            Model->Vertices[vertexWriteIndex++] = vertices[9];
            Model->Vertices[vertexWriteIndex++] = vertices[10];
            Model->Vertices[vertexWriteIndex++] = vertices[11];
            Model->Normals[normalWriteIndex++] = normals[9];
            Model->Normals[normalWriteIndex++] = normals[10];
            Model->Normals[normalWriteIndex++] = normals[11];
            i++;
        }
    }
}

model2 LoadModel(memory_arena *MainMemory, memory_arena *TempMemory, FILE* file)
{
    FBX_Node *fbxNode = ParseFBX(MainMemory, TempMemory, file);
    FBX_Node *modelNode = FBX_GetChildByName(FBX_GetChildByName(fbxNode, "Objects"), "Model");
    model2 Model = {0};
    ProcessModelNode(MainMemory, &Model, modelNode);
    return Model;
}

#endif
