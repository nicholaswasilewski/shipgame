#include "glHelper.cpp"
#include "platform.h"
#include "math.cpp"
#include "matrixMath.cpp"
#include "camera.cpp"
#include "loadFBX.cpp"
#include "fbxHelpers.cpp"
#include "game.h"

#include <stdlib.h>
#include <string.h>

#define CONTAINER

struct light
{
    v4 Position;
    v3 Ambient;
    v3 Diffuse;
    v3 Specular;
    float Power;
};

struct texture
{
    uint32 Height;
    uint32 Width;
    GLuint Handle;
    uint8* Data;
};

struct color_material
{
    v3 Ambient;
    v3 Diffuse;
    v3 Specular;
    v3 Emissive;
    float Shine;
};

struct material_binding
{
    GLuint Diffuse;
    GLuint Specular;
    GLuint Emissive;
    GLuint Shine;
};

material_binding CreateMaterialBinding(GLuint ShaderProgram)
{
    material_binding MaterialBinding = {0};
    MaterialBinding.Diffuse = glGetUniformLocation(ShaderProgram, "Material.Diffuse");
    MaterialBinding.Specular = glGetUniformLocation(ShaderProgram, "Material.Specular");
    MaterialBinding.Emissive = glGetUniformLocation(ShaderProgram, "Material.Emissive");
    MaterialBinding.Shine = glGetUniformLocation(ShaderProgram, "Material.Shine");
    return MaterialBinding;
};

struct light_binding
{
    GLuint Position;
    GLuint Ambient;
    GLuint Diffuse;
    GLuint Specular;
    GLuint Power;
};

light_binding CreateLightBinding(GLuint ShaderProgram)
{
    light_binding LightBinding = { 0 };
    LightBinding.Power = glGetUniformLocation(ShaderProgram, "Light.Power");
    LightBinding.Position = glGetUniformLocation(ShaderProgram, "Light.Position");
    LightBinding.Ambient = glGetUniformLocation(ShaderProgram, "Light.Ambient");
    LightBinding.Diffuse = glGetUniformLocation(ShaderProgram, "Light.Diffuse");
    LightBinding.Specular = glGetUniformLocation(ShaderProgram, "Light.Specular");
    return LightBinding;
}

void SetPointLightUniforms(light_binding LightBinding, light Light)
{
    glUniformVec4f(LightBinding.Position, Light.Position);
    glUniformVec3f(LightBinding.Ambient, Light.Ambient);
    glUniformVec3f(LightBinding.Diffuse, Light.Diffuse);
    glUniformVec3f(LightBinding.Specular, Light.Specular);
    glUniform1f(LightBinding.Power, Light.Power);
}

struct color_shader
{
    GLuint Program;
    GLuint M;
    GLuint V;
    GLuint MVP;

    GLuint CameraPosition;

    light_binding Light;
    material_binding Material;
};

struct texture_material
{
    //some sort of texture thing
    GLuint DiffuseMap;
    GLuint SpecularMap;
    GLuint EmissiveMap;
    float Shine;
};

struct light_texture_shader
{
    GLuint Program;
    GLuint M;
    GLuint V;
    GLuint MVP;
    
    GLuint CameraPosition;

    light_binding Light;
    material_binding Material;
};

struct skybox_shader
{
    GLuint Program;
    GLuint M;
    GLuint V;
    GLuint MVP;
    
    GLuint SkyBox;
};

struct water_shader
{
    GLuint Program;
    GLuint M;
    GLuint V;
    GLuint MVP;
    
    GLuint CameraPosition;

    light_binding Light;
    material_binding Material;
    GLuint NormalMapHandle;

    float UVOffset;
    GLuint UVOffsetHandle;
};

struct model
{
    GLfloat *Vertices;
    GLfloat *Normals;
    GLushort *Indices;
    GLfloat *UVs;
    GLfloat *Colors;
    
    texture_material *Material;
    
    int IndexCount;
    
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint NormalBuffer;
    GLuint IndexBuffer;
    GLuint ColorBuffer;
    GLuint UVBuffer;
};

struct skybox
{
    GLfloat *Vertices;
    GLuint VertexArrayID;
    GLuint VertexBuffer;

    texture Texture;
};

struct color_model
{
    model Model;
    color_material *Material;
};

struct texture_model
{
    model Model;
    texture_material *Material;
};

struct game_object
{
    model *Model;
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;
};

struct color_game_object
{
    color_model *ColorModel;
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;
};

struct game_data
{
    bool Initialized;
    memory_arena MainArena;
    memory_arena TempArena;

    light_texture_shader LightTextureShader;
    color_shader ColorShader;
    water_shader WaterShader;
    skybox_shader SkyBoxShader;
    camera Camera;

    texture BoxDiffuseMap;
    texture BoxSpecularMap;
    texture BoxEmissiveMap;
    texture_material BoxMaterial;
    color_material ColorMaterial;
    model BoxModel;
    color_model ColorBoxModel;
    
    //Scene
    light Light;
    game_object Box;
    game_object Box2;
    game_object LightBox;
    color_game_object ColorBox;

    //New Scene
    game_object Player;
    color_game_object Water;
    color_model WaterColorModel;
    color_material WaterColorMaterial;
    texture WaterNormalMap;
    skybox SkyBox;


    float BoxRotation;
};

texture LoadDDS(memory_arena *Memory, const char * filePath)
{
    texture NullTexture = {0};
    int8 header[124];

    FILE *fp = fopen(filePath, "rb");
    if (fp == 0)
    {
        DebugLog("File not found: %s", filePath);
        return NullTexture;
    }

    char fileCode[4];
    fread(fileCode, 1, 4, fp);
    if (strncmp(fileCode, "DDS ", 4) != 0)
    {
        fclose(fp);
        DebugLog("File is not DDS: %s", filePath);
        return NullTexture;
    }

    fread(&header, 124, 1, fp);

    uint32 height = *(uint32*)&(header[8]);
    uint32 width = *(uint32*)&(header[12]);
    uint32 linearSize = *(uint32*)&(header[16]);
    uint32 mipMapCount = *(uint32*)&(header[24]);
    char fourCC[5];
    fourCC[0] = header[80];
    fourCC[1] = header[81];
    fourCC[2] = header[82];
    fourCC[3] = header[83];
    fourCC[4] = '\0';
    
    uint32 bufferSize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    uint8* buffer = (uint8*)malloc(bufferSize * sizeof(uint8));
    fread(buffer, 1, bufferSize, fp);
    fclose(fp);

    uint32 format;
    
    format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    if (strncmp(fourCC, "DXT1", 4) == 0)
    {
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    }
    else if (strncmp (fourCC, "DXT3", 4) == 0)
    {
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    }
    else if (strncmp(fourCC, "DXT5", 4) == 0)
    {
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    }
    else
    {
        free(buffer);
//        PopSize(Memory, bufferSize*sizeof(uint8));
        DebugLog("File not DXT compressed: %s", filePath);
        return NullTexture;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    uint32 blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    uint32 offset = 0;
    int w = width;
    int h = height;
    for(uint32 level = 0; level < mipMapCount && (w || h); ++level)
    {
        uint32 size = ((w+3)/4)*((h+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, w, h, 0, size, buffer + offset);
        offset += size;
        w /= 2;
        h /= 2;
    }

    texture Result = {
        width,
        height,
        textureID,
        buffer
    };
    return Result;
}

struct BMPData
{    
    uint32 dataPos;
    uint32 width;
    uint32 height;
    uint32 imageSize;
    uint8* data;
};

#pragma pack(push,1)
struct BMPHeader
{
    char tag[2];
    uint32 imageSize;
    uint16 reserved;
    uint16 reserved2;
    uint32 pixelArrayOffset;
};

struct BMPInfoHeader {
    uint32 biSize;
    int32 biWidth;
    int32  biHeight;
    uint16  biPlanes;
    uint16  biBitCount;
    uint32 biCompression;
    uint32 biSizeImage;
    int32  biXPelsPerMeter;
    int32  biYPelsPerMeter;
    uint32 biClrUsed;
    uint32 biClrImportant;
};

struct BMPInfoHeaderV5 {
    DWORD        biSize;
    LONG         biWidth;
    LONG         biHeight;
    WORD         bV5Planes;
    WORD         bV5BitCount;
    DWORD        bV5Compression;
    DWORD        bV5SizeImage;
    LONG         bV5XPelsPerMeter;
    LONG         bV5YPelsPerMeter;
    DWORD        bV5ClrUsed;
    DWORD        bV5ClrImportant;
    DWORD        bV5RedMask;
    DWORD        bV5GreenMask;
    DWORD        bV5BlueMask;
    DWORD        bV5AlphaMask;
    DWORD        bV5CSType;
    CIEXYZTRIPLE bV5Endpoints;
    DWORD        bV5GammaRed;
    DWORD        bV5GammaGreen;
    DWORD        bV5GammaBlue;
    DWORD        bV5Intent;
    DWORD        bV5ProfileData;
    DWORD        bV5ProfileSize;
    DWORD        bV5Reserved;
};

#pragma pack(pop)

BMPData LoadBMP(memory_arena *Memory, char* filePath)
{
    BMPData NullBMP = { 0 };
    BMPHeader bmpHeader = {0};
    BMPInfoHeader bmpInfoHeader = {0};
    uint32 dataPos;
    uint32 width, height;
    uint32 imageSize;
    uint8* data;

    FILE * file = fopen(filePath, "rb");
    if (!file)
    {
        DebugLog("File not found: %s\n", filePath);
        return NullBMP;
    }
    
    if (fread(&bmpHeader, sizeof(bmpHeader), 1, file) != 1) {
        DebugLog("Malformed BMP Header: %s\n", filePath);
        return NullBMP;
    }

    if (fread(&bmpInfoHeader, sizeof(bmpInfoHeader), 1, file) != 1) {
        DebugLog("Malformed BMP Info Header: %s\n", filePath);
        return NullBMP;
    }

    if (bmpHeader.tag[0] != 'B' || bmpHeader.tag[1] != 'M') {
        DebugLog("Not a BMP: %s\n", filePath);
        return NullBMP;
    }

    dataPos = bmpHeader.pixelArrayOffset;
    imageSize = bmpHeader.imageSize;
    width = bmpInfoHeader.biWidth;
    height = bmpInfoHeader.biWidth;

    data = (uint8*)PushSize(Memory, imageSize*sizeof(uint8));

    // read bmp from bottom to top
    fseek(file, bmpHeader.pixelArrayOffset, SEEK_SET);
    for(int i = 0; i < height; i++)
    {
        int offset = (height-i-1)*width*3;
        fread(data+offset, 1, width*3, file);
    }

    fclose(file);

    BMPData result = {
        dataPos,
        width,
        height,
        imageSize,
        data
    };
    return result;
}

texture GenCubeMapFromBMP(
    memory_arena *Memory,
    char* rightFile,
    char* leftFile,
    char* topFile,
    char* bottomFile,
    char* backFile,
    char* frontFile)
{

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    BMPData rightBmpData = LoadBMP(Memory, rightFile);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, rightBmpData.width, rightBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, rightBmpData.data);
//    free(rightBmpData.data);
    PopSize(Memory, rightBmpData.imageSize);
 
    BMPData leftBmpData = LoadBMP(Memory, leftFile);   
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, leftBmpData.width, leftBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, leftBmpData.data);
//    free(leftBmpData.data);
    PopSize(Memory, leftBmpData.imageSize);
 
    BMPData topBmpData = LoadBMP(Memory, topFile);   
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, topBmpData.width, topBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, topBmpData.data);
//    free(topBmpData.data);
    PopSize(Memory, topBmpData.imageSize);
    
    BMPData bottomBmpData = LoadBMP(Memory, bottomFile);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, bottomBmpData.width, bottomBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bottomBmpData.data);
//    free(bottomBmpData.data);
    PopSize(Memory, bottomBmpData.imageSize);
    
    BMPData backBmpData = LoadBMP(Memory, backFile);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, backBmpData.width, backBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, backBmpData.data);
//    free(backBmpData.data);
    PopSize(Memory, backBmpData.imageSize);
    
    BMPData frontBmpData = LoadBMP(Memory, frontFile);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, frontBmpData.width, frontBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, frontBmpData.data);
//    free(frontBmpData.data);
    PopSize(Memory, frontBmpData.imageSize);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

    /*
    free(rightBmpData.data);
    free(leftBmpData.data);
    free(topBmpData.data);
    free(bottomBmpData.data);
    free(backBmpData.data);
    free(frontBmpData.data);
    */

    texture Result = {
        rightBmpData.width,
        rightBmpData.height,
        textureID,
        NULL
    };
    
    return Result;
}

texture GenTextureFromBMP(memory_arena *Memory, char* filePath)
{
    BMPData bmpData = LoadBMP(Memory, filePath);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmpData.width, bmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bmpData.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    //free(data);
    texture Result = {
        bmpData.width,
        bmpData.height,
        textureID,
        bmpData.data
    };
    
    return Result;
}

GLuint LoadShaders(char* vertexShaderFilePath, char* fragmentShaderFilePath)
{
    DebugLog("Loading %s & %s\n", vertexShaderFilePath, fragmentShaderFilePath);
    char* vertexShaderCode;
    GLint result = GL_FALSE;
    int32 infoLogLength;
    int readResult;
    
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    {
        FILE* vertexShaderFile = fopen(vertexShaderFilePath, "rb");
        fseek(vertexShaderFile, 0L, SEEK_END);
        int32 vertexShaderFileLength = ftell(vertexShaderFile);
        rewind(vertexShaderFile);
 
        vertexShaderCode = (char*)malloc(vertexShaderFileLength+1);
        readResult = fread(vertexShaderCode, 1, vertexShaderFileLength, vertexShaderFile);
        fclose(vertexShaderFile);
        vertexShaderCode[vertexShaderFileLength] = '\0';
    
        glShaderSource(vertexShaderID, 1, &vertexShaderCode, 0);
        glCompileShader(vertexShaderID);

        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            DebugLog("Vertex Shader: %s:%ld:\n%s\n", vertexShaderFilePath, vertexShaderFileLength, vertexShaderCode);
            char* error = (char*)malloc(infoLogLength);
            glGetShaderInfoLog(vertexShaderID, infoLogLength, 0, error);
            DebugLog("%s error:\n%s\n", vertexShaderFilePath, error);
            free(error);
        }
        free(vertexShaderCode);
    }

    {
        FILE* fragmentShaderFile = fopen(fragmentShaderFilePath, "rb");
        fseek(fragmentShaderFile, 0L, SEEK_END);
        long fragmentShaderFileLength = ftell(fragmentShaderFile);
        rewind(fragmentShaderFile);

        char *fragmentShaderCode = (char*)malloc(fragmentShaderFileLength+1);
        readResult = fread(fragmentShaderCode, 1, fragmentShaderFileLength, fragmentShaderFile);
        fragmentShaderCode[fragmentShaderFileLength] = '\0';
    
        GLint glFragmentShaderFileLength = fragmentShaderFileLength;
        glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, &glFragmentShaderFileLength);
        fclose(fragmentShaderFile);
        glCompileShader(fragmentShaderID);

        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            DebugLog("Fragment Shader: %s:%ld:\n%s\n", fragmentShaderFilePath, fragmentShaderFileLength, fragmentShaderCode);
            char* error = (char*)malloc(infoLogLength+1);
            glGetShaderInfoLog(fragmentShaderID, infoLogLength, 0, error);
            DebugLog("%s error:\n%s\n", fragmentShaderFilePath, error);
            free(error);
        }
        free(fragmentShaderCode);
    }
    
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        char* error = (char*)malloc(infoLogLength+1);
        glGetProgramInfoLog(programID, infoLogLength, 0, error);
        DebugLog("%s\n%s\n%s\n", vertexShaderFilePath, fragmentShaderFilePath, error);
        //printf("%s\n", error);
        free(error);
    }
    
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

void Init(platform_data* Platform, game_data *Game)
{
    InitArena(&Game->MainArena,
              Platform->MainMemorySize,
              (uint8*)Platform->MainMemory+sizeof(game_data));
    
    glClearColor(0.6, 0.4, 0.2, 0.0);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    model *BoxModel = &Game->BoxModel;
    glGenVertexArrays(1, &BoxModel->VertexArrayID);
    glBindVertexArray(BoxModel->VertexArrayID);
    GLfloat vertexBufferData[] = {
        //Front
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        //Back
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        //Top
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        //Bottom
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        //Left
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        //Right
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f
    };
    size_t vertexBufferSize = sizeof(vertexBufferData);
    BoxModel->Vertices = (GLfloat*)malloc(vertexBufferSize);
    memcpy(BoxModel->Vertices, vertexBufferData, vertexBufferSize);
    glGenBuffers(1, &BoxModel->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 BoxModel->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBufferSize,
                 &BoxModel->Vertices[0],
                 GL_STATIC_DRAW);

#define FrontNormal 0.0f, 0.0f, 1.0f
#define BackNormal 0.0f, 0.0f, -1.0f
#define UpNormal 0.0f, 1.0f, 0.0f
#define DownNormal 0.0, -1.0f, 0.0f
#define LeftNormal -1.0f, 0.0f, 0.0f
#define RightNormal 1.0f, 0.0f, 0.0f
#define Zero 0.0f, 0.0f, 0.0f
    GLfloat normalBufferData[] = {
        FrontNormal, FrontNormal, FrontNormal,FrontNormal,
        BackNormal, BackNormal, BackNormal,BackNormal,
        UpNormal, UpNormal, UpNormal,UpNormal,
        DownNormal, DownNormal, DownNormal,DownNormal,
        LeftNormal, LeftNormal, LeftNormal,LeftNormal,
        RightNormal, RightNormal, RightNormal,RightNormal,
    };

    size_t normalBufferSize = sizeof(normalBufferData);
    BoxModel->Normals = (GLfloat*)malloc(normalBufferSize);
    memcpy(BoxModel->Normals, normalBufferData, normalBufferSize);
        
    glGenBuffers(1, &BoxModel->NormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 BoxModel->NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 normalBufferSize,
                 &BoxModel->Normals[0],
                 GL_STATIC_DRAW);
        
    GLushort indexBufferData[] = {
        0,1,2,
        1,3,2,
        4,5,6,
        5,7,6,
        8,9,10,
        9,11,10,
        12,13,14,
        13,15,14,
        16,17,18,
        17,19,18,
        20,21,22,
        21,23,22
    };
    BoxModel->IndexCount = sizeof(indexBufferData)/sizeof(GLushort);

    size_t indexBufferSize = sizeof(indexBufferData);
    BoxModel->Indices = (GLushort*)malloc(indexBufferSize);
    memcpy(BoxModel->Indices, indexBufferData, indexBufferSize);

    glGenBuffers(1, &BoxModel->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 BoxModel->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferSize,
                 &BoxModel->Indices[0],
                 GL_STATIC_DRAW);

    // make water model
    color_game_object Water = {0};

    //Water.ColorModel = &WaterColorModel;
    //Water.ColorModel->Material = &WaterColorMaterial;
    Water.ColorModel = &Game->WaterColorModel;
    Water.ColorModel->Material = &Game->WaterColorMaterial;

    Water.Scale = V3(1.0f, 1.0f, 1.0f);
    Water.Position = V3(0.0f, 0.0f, 0.0f);
    Water.Axis = V3(0.25f, 1.0f, .5f);
    Water.Angle = 0.0f;

    Water.ColorModel->Material->Diffuse = V3(0.1f, 0.5f, 0.9f);
    Water.ColorModel->Material->Specular = V3(0.4f, 0.7f, 1);
    Water.ColorModel->Material->Emissive = V3(0.1f, 0.5f, 0.9f);
    Water.ColorModel->Material->Shine = 100.0f;
    
    GLuint WaterVertexID;
    glGenVertexArrays(1, &Water.ColorModel->Model.VertexArrayID);
    glBindVertexArray(Water.ColorModel->Model.VertexArrayID);
    GLfloat waterVertices[] = {
        -1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f, -1.0f,
         1.0f, 0.0f,  1.0f,
         1.0f, 0.0f, -1.0f
    };

    // Water Vertices
    int vertCount = 20;
    float vertWidth = 20.0f; 
    Water.Position = V3(0.0f, 0.0f,0.0f);
    Water.Position = V3(-0.5f*vertCount*vertWidth, 0.0f, 0.5f*vertCount*vertWidth);
    size_t waterBufferSize = sizeof(GLfloat)*(vertCount)*(vertCount)*3;
    Water.ColorModel->Model.Vertices = (GLfloat*)malloc(waterBufferSize); 
    for(int x = 0; x < vertCount; x++)
    {
        for(int z = 0; z < vertCount; z++)
        {
            int start = ((x * vertCount) + z) * 3;
            Water.ColorModel->Model.Vertices[start] = x * vertWidth;
            Water.ColorModel->Model.Vertices[start+1] = 0.0f;
            Water.ColorModel->Model.Vertices[start+2] = -z * vertWidth;
            // DebugLog("Vertex %i\n", (x * vertCount) + z);
            // DebugLog("Vertices %i: %f\n", start, Water.ColorModel->Model.Vertices[start]);
            // DebugLog("Vertices %i: %f\n", start+1, Water.ColorModel->Model.Vertices[start+1]);
            // DebugLog("Vertices %i: %f\n", start+2, Water.ColorModel->Model.Vertices[start+2]);
        }
    }

    glGenBuffers(1, &Water.ColorModel->Model.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Water.ColorModel->Model.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, waterBufferSize, &Water.ColorModel->Model.Vertices[0], GL_STATIC_DRAW);
    
    // Water Indices
    int segmentCount = vertCount-1;
    size_t waterIndexSize = sizeof(GLushort)*(segmentCount)*(segmentCount)*6;
    Water.ColorModel->Model.Indices = (GLushort*)malloc(waterIndexSize);
    for(int x = 0; x < segmentCount; x++)
    {
        for(int z = 0; z < segmentCount; z++)
        {
            int start = ((x * segmentCount) + z) * 6;
            Water.ColorModel->Model.Indices[start] = (x * vertCount) + z;
            Water.ColorModel->Model.Indices[start+1] = ((x+1) * vertCount) + (z+1);
            Water.ColorModel->Model.Indices[start+2] = (x * vertCount) + (z+1);
            Water.ColorModel->Model.Indices[start+3] = (x * vertCount) + z;
            Water.ColorModel->Model.Indices[start+4] = ((x+1) * vertCount) + z;
            Water.ColorModel->Model.Indices[start+5] = ((x+1) * vertCount) + (z+1);
            // DebugLog("Index %i\n", (x * segmentCount) + z);
            // DebugLog("Indices %i: %i\n", start, Water.ColorModel->Model.Indices[start]);
            // DebugLog("Indices %i: %i\n", start+1, Water.ColorModel->Model.Indices[start+1]);
            // DebugLog("Indices %i: %i\n", start+2, Water.ColorModel->Model.Indices[start+2]);
            // DebugLog("Indices %i: %i\n", start+3, Water.ColorModel->Model.Indices[start+3]);
            // DebugLog("Indices %i: %i\n", start+4, Water.ColorModel->Model.Indices[start+4]);
            // DebugLog("Indices %i: %i\n", start+5, Water.ColorModel->Model.Indices[start+5]);
        }
    } 

    Water.ColorModel->Model.IndexCount = waterIndexSize / sizeof(GLushort);
    glGenBuffers(1, &Water.ColorModel->Model.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Water.ColorModel->Model.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndexSize, Water.ColorModel->Model.Indices, GL_STATIC_DRAW);

    // Water UVs
    size_t waterUVBufferSize = sizeof(GLfloat)*(vertCount)*(vertCount)*2;
    Water.ColorModel->Model.UVs = (GLfloat*)malloc(waterUVBufferSize); 
    for(int x = 0; x < vertCount; x++)
    {
        for(int z = 0; z < vertCount; z++)
        {
            int start = ((x * vertCount) + z) * 2;
            Water.ColorModel->Model.UVs[start] = x;
            Water.ColorModel->Model.UVs[start+1] = z;
            // DebugLog("UV %i\n", (x * vertCount) + z);
            // DebugLog("UVs %i: %f\n", start, Water.ColorModel->Model.UVs[start]);
            // DebugLog("UVs %i: %f\n", start+1, Water.ColorModel->Model.UVs[start+1]);
        }
    }

    glGenBuffers(1, &Water.ColorModel->Model.UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Water.ColorModel->Model.UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, waterUVBufferSize, &Water.ColorModel->Model.UVs[0], GL_STATIC_DRAW);
    
    Game->Water = Water;

#if DIE
    GLfloat OneThird = 1.0f/3.0f;
    GLfloat TwoThirds = 2.0f/3.0f;
    GLfloat uvBufferData[] = {
        0.0f, 0.0f,
        0.0f, OneThird,
        OneThird, 0.0f,
        OneThird, OneThird,

        TwoThirds, OneThird,
        TwoThirds, TwoThirds,
        1.0f, OneThird,
        1.0f, TwoThirds,

        OneThird, 0.0f,
        OneThird, OneThird,
        TwoThirds, 0.0f,
        TwoThirds, OneThird,

        OneThird, OneThird,
        OneThird, TwoThirds,
        TwoThirds, OneThird,
        TwoThirds, TwoThirds,

        TwoThirds,0.0f,
        TwoThirds,OneThird,
        1.0f, 0.0f,
        1.0f, OneThird,

        0.0f, OneThird,
        0.0f, TwoThirds,
        OneThird, OneThird,
        OneThird, TwoThirds,
    };
#elif defined(CONTAINER)
#define FACE                                        \
    0.0f, 0.0f,                                        \
        0.0f, 1.0f,                                \
        1.0f, 0.0f,                                \
        1.0f, 1.0f                                \
        
        
    GLfloat uvBufferData[] = {
        FACE, FACE, FACE, FACE, FACE, FACE
    };
#undef FACE

#endif
    size_t uvBufferSize = sizeof(uvBufferData);
    BoxModel->UVs = (GLfloat*)malloc(uvBufferSize);
    memcpy(BoxModel->UVs, uvBufferData, uvBufferSize);
        
    glGenBuffers(1, &BoxModel->UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, BoxModel->UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, uvBufferSize, BoxModel->UVs, GL_STATIC_DRAW);

#if DIE
    Game->BoxDiffuseMap = LoadDDS(&Game->MainArena, "../res/Textures/uvtemplate.dds");
#elif defined(CONTAINER)
    Game->BoxDiffuseMap = LoadDDS(&Game->MainArena, "../res/Textures/container.dds");
#endif

    Game->BoxSpecularMap = LoadDDS(&Game->MainArena, "../res/Textures/containerspecular.dds");
    Game->BoxEmissiveMap = LoadDDS(&Game->MainArena, "../res/Textures/containeremissive.dds");
    Game->WaterNormalMap = GenTextureFromBMP(&Game->MainArena, "../res/Textures/matchingNormalMap.bmp");
    //Game->WaterNormalMap = GenTextureFromBMP("../res/Textures/normalMap.bmp");
    
    texture_material *BoxMaterial = &Game->BoxMaterial;
    BoxModel->Material = BoxMaterial;
    BoxMaterial->DiffuseMap = Game->BoxDiffuseMap.Handle;
    BoxMaterial->SpecularMap = Game->BoxSpecularMap.Handle;
//    BoxMaterial->EmissiveMap = Game->BoxEmissiveMap.Handle;
    BoxMaterial->Shine = 60.0f;
    
    color_model *ColorBoxModel = &Game->ColorBoxModel;
    ColorBoxModel->Model = *BoxModel;
    color_material *ColorMaterial = &Game->ColorMaterial;
    ColorBoxModel->Material = ColorMaterial;
    ColorMaterial->Diffuse = V3(0.0f, 0.0f, 0.0f);
    ColorMaterial->Specular = V3(0.0f, 0.0f, 0.0f);
    ColorMaterial->Emissive = V3(1.0f, 1.0f, 1.0f);
    ColorMaterial->Shine = 1.0f;

    light_texture_shader Shader;
    Shader.Program = LoadShaders("../res/Shaders/lightTextureShader.vert", "../res/Shaders/lightTextureShader.frag");
    Shader.M = glGetUniformLocation(Shader.Program, "M");
    Shader.V = glGetUniformLocation(Shader.Program, "V");
    Shader.MVP = glGetUniformLocation(Shader.Program, "MVP");
    
    Shader.CameraPosition = glGetUniformLocation(Shader.Program, "CameraPosition");
    Shader.Light = CreateLightBinding(Shader.Program);
    Shader.Material = CreateMaterialBinding(Shader.Program);
    Game->LightTextureShader = Shader;

    water_shader WaterShader;
    WaterShader.Program = LoadShaders("../res/Shaders/waterShader.vert", "../res/Shaders/waterShader.frag");
    WaterShader.M = glGetUniformLocation(WaterShader.Program, "M");
    WaterShader.V = glGetUniformLocation(WaterShader.Program, "V");
    WaterShader.MVP = glGetUniformLocation(WaterShader.Program, "MVP");
    WaterShader.CameraPosition = glGetUniformLocation(WaterShader.Program, "CameraPosition");
    WaterShader.Light = CreateLightBinding(WaterShader.Program);
    WaterShader.Material = CreateMaterialBinding(WaterShader.Program);
    WaterShader.NormalMapHandle = Game->WaterNormalMap.Handle;
    WaterShader.UVOffsetHandle = glGetUniformLocation(WaterShader.Program, "UVOffset");
    Game->WaterShader = WaterShader;

    color_shader ColorShader;
    ColorShader.Program = LoadShaders("../res/Shaders/vertexShader.vert", "../res/Shaders/fragmentShader.frag");
    ColorShader.M = glGetUniformLocation(ColorShader.Program, "M");
    ColorShader.V = glGetUniformLocation(ColorShader.Program, "V");
    ColorShader.MVP = glGetUniformLocation(ColorShader.Program, "MVP");

    ColorShader.CameraPosition = glGetUniformLocation(ColorShader.Program, "CameraPosition");
    ColorShader.Light = CreateLightBinding(ColorShader.Program);

    ColorShader.Material.Diffuse = glGetUniformLocation(ColorShader.Program, "Material.Diffuse");
    ColorShader.Material.Specular = glGetUniformLocation(ColorShader.Program, "Material.Specular");
    ColorShader.Material.Emissive = glGetUniformLocation(ColorShader.Program, "Material.Emissive");
    ColorShader.Material.Shine = glGetUniformLocation(ColorShader.Program, "Material.Shine");
    Game->ColorShader = ColorShader;
        
    camera Camera = {0};
    Camera.FOV = PI*.25f;
    Camera.Aspect = 800.0f/600.0f;
    Camera.Near = 0.001f;
    Camera.Far = 1000.0f;
    Camera.Position = V3(0.0f, 5.0f, 5.0f);
    Camera.Forward = Normalize(V3(0.0f, -0.2f,-1.0f));
    Camera.Up = V3(0,1,0);
    Game->Camera = Camera;

    light Light = { 0 };
    Light.Position = V4(4.0f, 3.0f, -40.0f, 1.0f);
    Light.Ambient = V3(0.1f, 0.1f, 0.1f);
    Light.Diffuse = V3(0.5f, 0.5f, 0.5f);
    Light.Specular = V3(1.0f, 1.0f, 1.0f);
    Light.Power = 50.0f;
    Game->Light = Light;
    
    game_object Box = { 0 };
    Box.Model = &Game->BoxModel;

    Box.Scale = V3(1.0f, 1.0f, 1.0f);
    Box.Position = V3(0.0f, 0.0f, 0.0f);
    Box.Axis = V3(0.25f, 1.0f, .5f);
    Box.Angle = 0.0f;
    Game->Box = Box;

    game_object Box2 = { 0 };
    Box2.Model = &Game->BoxModel;
    Box2.Scale = V3(0.5f, 0.5f, 0.5f);
    Box2.Position = V3(4.0f, 0.0f, 0.0f);
    Box2.Axis = V3(0.0f, 1.0f, 0.0f);
    Box2.Angle = PI*0.25f;
    Game->Box2 = Box2;

    game_object LightBox = { 0 };
    LightBox.Model = &Game->BoxModel;
    LightBox.Scale = V3(0.5f, 0.5f, 0.5f);
    LightBox.Position = V3(-4.0f, 0.0f, 0.0f);
    LightBox.Axis = V3(0.0f, 1.0f, 0.0f);
    LightBox.Angle = 0.0f;
    Game->LightBox = LightBox;

    color_game_object ColorBox = { 0 };
    ColorBox.ColorModel = &Game->ColorBoxModel;
    ColorBox.Scale = V3(0.5f, 0.5f, 0.5f);
    ColorBox.Position = V3(Light.Position);
    ColorBox.Axis = V3(0.0f, 1.0f, 0.0f);
    ColorBox.Angle = 0.0f;
    Game->ColorBox = ColorBox;

    game_object Player = { 0 };
    Player.Model = &Game->BoxModel;
    Player.Scale = V3(0.5f, 2.0f, 0.5f);
    Player.Position = V3(0.0f, 0.0f, 0.0f);
    Player.Axis = V3(0.0f, 1.0f, 0.0f);
    Player.Angle = 0.0f;
    Game->Player = Player;

    
    // ********************************
    // Start SkyBox
    // ********************************
    skybox *SkyBox = &Game->SkyBox;
    glGenVertexArrays(1, &SkyBox->VertexArrayID);
    glBindVertexArray(SkyBox->VertexArrayID);
    GLfloat skyboxVertexBufferData[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
    
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
    
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
    
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    size_t skyboxVertexBufferSize = sizeof(skyboxVertexBufferData);
    SkyBox->Vertices = (GLfloat*)malloc(skyboxVertexBufferSize);
    memcpy(SkyBox->Vertices, skyboxVertexBufferData, skyboxVertexBufferSize);
    glGenBuffers(1, &SkyBox->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 SkyBox->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 skyboxVertexBufferSize,
                 &SkyBox->Vertices[0],
                 GL_STATIC_DRAW);

    SkyBox->Texture = GenCubeMapFromBMP(
        &Game->MainArena,
        "../res/Textures/skybox/right.bmp",
        "../res/Textures/skybox/left.bmp",
        "../res/Textures/skybox/top.bmp",
        "../res/Textures/skybox/bottom.bmp",
        "../res/Textures/skybox/back.bmp",
        "../res/Textures/skybox/front.bmp");
        
    skybox_shader SkyBoxShader;
    SkyBoxShader.Program = LoadShaders("../res/Shaders/skybox.vert", "../res/Shaders/skybox.frag");
    SkyBoxShader.M = glGetUniformLocation(SkyBoxShader.Program, "M");
    SkyBoxShader.V = glGetUniformLocation(SkyBoxShader.Program, "V");
    SkyBoxShader.MVP = glGetUniformLocation(SkyBoxShader.Program, "MVP");
    SkyBoxShader.SkyBox = SkyBox->Texture.Handle;
    Game->SkyBoxShader = SkyBoxShader;

    // load model from FBX
    // FILE* monkeyFile =  fopen("../res/Models/Rock_Medium_SPR.fbx", "r");
     FILE* monkeyFile =  fopen("../res/Models/monkey.fbx", "r");
     FBX_Node* monkey = (FBX_Node*)malloc(sizeof(FBX_Node));
     ParseFBX(monkeyFile, monkey);

    // // get model info
    // FBX_Node* fbx_objects = FBX_GetChildByName(monkey, "Objects");
    // FBX_Node* fbx_model = FBX_GetChildByName(fbx_objects, "Model");
    // FBX_Node* fbx_vertices = FBX_GetChildByName(fbx_model, "Vertices");
    // DebugLog("Vertex Count: %d\n", fbx_vertices->ValueCount);
    // DebugLog("Vertex0: %s\n", fbx_vertices->Values[0]);

    Game->Initialized = true;
}

void RenderObject(color_game_object GameObject, camera Camera, light Light, mat4 Projection, mat4 View, color_shader Shader)
{
    mat4 Rotation = MakeRotation(GameObject.Axis, GameObject.Angle);
    mat4 Scale = MakeScale(GameObject.Scale);
    mat4 Translation = MakeTranslation(GameObject.Position);
    mat4 Model = Translation * Rotation * Scale;
    mat4 MVP = Projection * View * Model;

    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.M, 1, GL_FALSE, &Model.E[0][0]);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);

    glUniformVec3f(Shader.CameraPosition, Camera.Position);
    
    SetPointLightUniforms(Shader.Light, Light);

    color_material *Material = GameObject.ColorModel->Material;
    glUniformVec3f(Shader.Material.Diffuse, Material->Diffuse);
    glUniformVec3f(Shader.Material.Emissive, Material->Emissive);
    glUniformVec3f(Shader.Material.Specular, Material->Specular);
    glUniform1f(Shader.Material.Shine, Material->Shine);

    model ObjectModel = GameObject.ColorModel->Model;
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, ObjectModel.VertexBuffer);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, ObjectModel.NormalBuffer);
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ObjectModel.IndexBuffer);
    glDrawElements(GL_TRIANGLES,
                   ObjectModel.IndexCount,
                   GL_UNSIGNED_SHORT,
                   (void*)0
        );
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void RenderSkyBox(skybox SkyBox, camera Camera, light Light, mat4 Projection, mat4 View, skybox_shader Shader)
{
    // remove translation part of view matrix
    View = Mat4(Mat3(View));
    mat4 MVP = Projection * View;

    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Shader.SkyBox);

    glBindBuffer(GL_ARRAY_BUFFER, SkyBox.VertexBuffer);
    glBindVertexArray(SkyBox.VertexArrayID);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableVertexAttribArray(0);
    
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void RenderWater(color_game_object GameObject, camera Camera, light Light, mat4 Projection, mat4 View, water_shader Shader)
{

    mat4 Rotation = MakeRotation(GameObject.Axis, GameObject.Angle);
    mat4 Scale = MakeScale(GameObject.Scale);
    mat4 Translation = MakeTranslation(GameObject.Position);
    mat4 Model = Translation * Rotation * Scale;
    mat4 MVP = Projection * View * Model;

    glBindVertexArray(GameObject.ColorModel->Model.VertexArrayID);
    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.M, 1, GL_FALSE, &Model.E[0][0]);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);

    glUniformVec3f(Shader.CameraPosition, Camera.Position);
    SetPointLightUniforms(Shader.Light, Light);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Shader.NormalMapHandle);

    color_material *Material = GameObject.ColorModel->Material;
    glUniformVec3f(Shader.Material.Diffuse, Material->Diffuse);
    glUniformVec3f(Shader.Material.Emissive, Material->Emissive);
    glUniformVec3f(Shader.Material.Specular, Material->Specular);
    glUniform1f(Shader.Material.Shine, Material->Shine);
    
    glUniform1f(Shader.UVOffsetHandle, Shader.UVOffset);

    model ObjectModel = GameObject.ColorModel->Model;

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, ObjectModel.VertexBuffer);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );

    //DebugLog("ObjectModel.UVs[1] %f,%f\n", ObjectModel.UVs[6],ObjectModel.UVs[7]);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, ObjectModel.UVBuffer);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ObjectModel.IndexBuffer);
    glDrawElements(GL_TRIANGLES,
                   ObjectModel.IndexCount,
                   GL_UNSIGNED_SHORT,
                   (void*)0
        );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void RenderObject(game_object GameObject, camera Camera, light Light, mat4 Projection, mat4 View, light_texture_shader Shader)
{
    mat4 Rotation = MakeRotation(GameObject.Axis, GameObject.Angle);
    mat4 Scale = MakeScale(GameObject.Scale);
    mat4 Translation = MakeTranslation(GameObject.Position);
    mat4 Model = Translation * Rotation * Scale;
    mat4 MVP = Projection * View * Model;
    
    glBindVertexArray(GameObject.Model->VertexArrayID);

    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.M, 1, GL_FALSE, &Model.E[0][0]);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);

    glUniformVec3f(Shader.CameraPosition, Camera.Position);

    SetPointLightUniforms(Shader.Light, Light);

    texture_material *Material = GameObject.Model->Material;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Material->DiffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Material->SpecularMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Material->EmissiveMap);
    
    glUniform1i(Shader.Material.Diffuse, 0);
    glUniform1i(Shader.Material.Specular, 1);
    glUniform1f(Shader.Material.Shine, Material->Shine);
    glUniform1i(Shader.Material.Emissive, 2);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, GameObject.Model->VertexBuffer);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, GameObject.Model->UVBuffer);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, GameObject.Model->NormalBuffer);
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
        );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GameObject.Model->IndexBuffer);
    glDrawElements(GL_TRIANGLES,
                   GameObject.Model->IndexCount,
                   GL_UNSIGNED_SHORT,
                   (void*)0
        );
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Update(platform_data *Platform, game_data *Game)
{
    input *LastInput = Platform->LastInput;
    input *Input = Platform->NewInput;
    controller OldKeyboard = LastInput->Keyboard;
    controller Keyboard = Input->Keyboard;
    
    if (Keyboard.Left.Down)
    {
        CameraStrafe(&Game->Camera, Input->dT, -3.0f);
    }
    else if (Keyboard.Right.Down)        
    {
        CameraStrafe(&Game->Camera, Input->dT, 3.0f);
    }

    if (Keyboard.Forward.Down)
    {
        CameraMoveForward(&Game->Camera, Input->dT, 3.0f);
    }
    else if (Keyboard.Back.Down)
    {
        CameraMoveForward(&Game->Camera, Input->dT, -3.0f);
    }
    
    if (Keyboard.Up.Down)
    {
        CameraMoveUp(&Game->Camera, Input->dT, 3.0f);
    }
    else if (Keyboard.Down.Down)
    {
        CameraMoveUp(&Game->Camera, Input->dT, -3.0f);
    }

    if (Keyboard.UpperLeft.Down)
    {
        RollCamera(&Game->Camera, Input->dT, 0.5f);
    }
    else if (Keyboard.UpperRight.Down)
    {
        RollCamera(&Game->Camera, Input->dT, -0.5f);
    }

    TurnCamera(&Game->Camera,
               Keyboard.RightStick.X / 100.0f,
               Keyboard.RightStick.Y / 100.0f,
               Input->dT*1.0f);

    Game->Box.Angle += PI*(1.0f/120.0f);

    Game->WaterShader.UVOffset += 0.0005f;
    if(Game->WaterShader.UVOffset >= 1.0f)
    {
        Game->WaterShader.UVOffset -= 1.0f;
    }
}

void RenderScene(game_data *Game, mat4 Projection, mat4 View)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //RenderObject(Game->Box, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    //RenderObject(Game->Box2, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject(Game->LightBox, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject(Game->ColorBox, Game->Camera, Game->Light, Projection, View, Game->ColorShader);

    // player and water
    RenderWater(Game->Water, Game->Camera, Game->Light, Projection, View, Game->WaterShader);
    RenderObject(Game->Player, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);

    // skybox comes last, to save on performance
    RenderSkyBox(Game->SkyBox, Game->Camera, Game->Light, Projection, View, Game->SkyBoxShader);
}

void RenderToTarget(platform_data *Platform, game_data *Game, mat4 Projection, mat4 View, FramebufferDesc *TargetBuffer, int BufferWidth, int BufferHeight)
{
    
    glEnable(GL_MULTISAMPLE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, TargetBuffer->RenderFramebufferId);
    glViewport(0, 0, BufferWidth, BufferHeight);
    RenderScene(Game, Projection, View);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glDisable(GL_MULTISAMPLE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, TargetBuffer->RenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, TargetBuffer->ResolveFramebufferId);
    glBlitFramebuffer(0, 0, BufferWidth, BufferHeight,
                      0, 0, BufferWidth, BufferHeight,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);    
}

void Render(platform_data *Platform, game_data *Game)
{

    mat4 Projection = GenerateCameraPerspective(Game->Camera);
    mat4 View = GenerateCameraView(Game->Camera);
#if OPEN_VR
    float EyeDistance = 1.0f;
    camera LeftEyeCamera = Game->Camera;
    LeftEyeCamera.Position = LeftEyeCamera.Position + -EyeDistance*Cross(LeftEyeCamera.Forward,
                                                                        LeftEyeCamera.Up);
    camera RightEyeCamera = Game->Camera;
    RightEyeCamera.Position = RightEyeCamera.Position + EyeDistance*Cross(RightEyeCamera.Forward,
                                                                           RightEyeCamera.Up);
    
    mat4 LeftEyeView = GenerateCameraView(LeftEyeCamera);
    mat4 RightEyeView = GenerateCameraView(RightEyeCamera);
    
    if (Platform->LeftEye && Platform->RightEye)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        RenderToTarget(Platform, Game, Projection, LeftEyeView, Platform->LeftEye, Platform->VRBufferWidth, Platform->VRBufferHeight);
        RenderToTarget(Platform, Game, Projection, RightEyeView, Platform->RightEye, Platform->VRBufferWidth, Platform->VRBufferHeight);
    }
#endif
    
    glViewport(0, 0, Platform->WindowWidth, Platform->WindowHeight);
    RenderScene(Game, Projection, View);
}

void UpdateAndRender(platform_data *Platform)
{
    game_data* Game = (game_data*)(((char*)Platform->MainMemory)+0);
    InitArena(&Game->TempArena, Platform->TempMemorySize, (uint8*)Platform->TempMemory);
    
    if (!Game->Initialized)
    {
        Init(Platform, Game);
        printf("GLInit Errors:\n");
        GLErrorShow();
    }
    Update(Platform, Game);
    Render(Platform, Game);
}
