#ifndef _TEXTURE_CPP__

#include "platform.h"

struct texture
{
    uint32 Height;
    uint32 Width;
    
    GLenum TextureType;
    GLuint Handle;
    
    uint8* Data;
};

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
    GL(glGenTextures(1, &textureID));
    GL(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));

    BMPData rightBmpData = LoadBMP(Memory, rightFile);
    GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, rightBmpData.width, rightBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, rightBmpData.data));
    //    free(rightBmpData.data);
    PopSize(Memory, rightBmpData.imageSize);
    
    BMPData leftBmpData = LoadBMP(Memory, leftFile);   
    GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, leftBmpData.width, leftBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, leftBmpData.data));
    //    free(leftBmpData.data);
    PopSize(Memory, leftBmpData.imageSize);
    
    BMPData topBmpData = LoadBMP(Memory, topFile);   
    GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, topBmpData.width, topBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, topBmpData.data));
    //    free(topBmpData.data);
    PopSize(Memory, topBmpData.imageSize);
    
    BMPData bottomBmpData = LoadBMP(Memory, bottomFile);
    GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, bottomBmpData.width, bottomBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bottomBmpData.data));
    //    free(bottomBmpData.data);
    PopSize(Memory, bottomBmpData.imageSize);
    
    BMPData backBmpData = LoadBMP(Memory, backFile);
    GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, backBmpData.width, backBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, backBmpData.data));
    //    free(backBmpData.data);
    PopSize(Memory, backBmpData.imageSize);
    
    BMPData frontBmpData = LoadBMP(Memory, frontFile);
    GL(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, frontBmpData.width, frontBmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, frontBmpData.data));
    //    free(frontBmpData.data);
    PopSize(Memory, frontBmpData.imageSize);
    
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);) 
    
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
        GL_TEXTURE_CUBE_MAP,
        textureID,
        NULL
    };
    
    return Result;
}

texture GenTextureFromBMPData(BMPData bmpData) {
    GLuint textureID;
    GL(glGenTextures(1, &textureID));
    GLenum textureType = GL_TEXTURE_2D;
    GL(glBindTexture(textureType, textureID));
    
    GL(glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL(glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL(glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL(glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexImage2D(textureType, 0, GL_RGB, bmpData.width, bmpData.height, 0, GL_BGR, GL_UNSIGNED_BYTE, bmpData.data));
    GL(glGenerateMipmap(textureType));
    
    texture Result = {
        bmpData.width,
        bmpData.height,
        GL_TEXTURE_2D,
        textureID,
        bmpData.data
    };
    
    return Result;
}

texture GenTextureFromBMPFile(memory_arena *Memory, char* filePath)
{
    BMPData bmpData = LoadBMP(Memory, filePath);
    return GenTextureFromBMPData(bmpData);
}

texture LoadDDS(memory_arena *Memory, const char * filePath)
{
    texture NullTexture = {0};
    int8 header[124];
    
    FILE *fp = fopen(filePath, "rb");
    if (fp == 0)
    {
        DebugLog("File not found: %s\n", filePath);
        return NullTexture;
    }
    
    char fileCode[4];
    fread(fileCode, 1, 4, fp);
    if (strncmp(fileCode, "DDS ", 4) != 0)
    {
        fclose(fp);
        DebugLog("File is not DDS: %s\n", filePath);
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
    uint8* buffer = (uint8*)PushSize(Memory, bufferSize * sizeof(uint8));
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
        PopSize(Memory, bufferSize*sizeof(uint8));
        DebugLog("File not DXT compressed: %s\n", filePath);
        return NullTexture;
    }
    
    GLuint textureID;
    GL(glGenTextures(1, &textureID));
    GL(glBindTexture(GL_TEXTURE_2D, textureID));
    
    uint32 blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    uint32 offset = 0;
    int w = width;
    int h = height;
    for(uint32 level = 0; level < mipMapCount && (w || h); ++level)
    {
        uint32 size = ((w+3)/4)*((h+3)/4)*blockSize;
        GL(glCompressedTexImage2D(GL_TEXTURE_2D, level, format, w, h, 0, size, buffer + offset));
        offset += size;
        w /= 2;
        h /= 2;
    }
    
    texture Result = {
        width,
        height,
        GL_TEXTURE_2D,
        textureID,
        buffer
    };
    return Result;
}

#define _TEXTURE_CPP__
#endif