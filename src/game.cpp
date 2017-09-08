#include "glHelper.cpp"
#include "platform.h"
#include "math.cpp"
#include "matrixMath.cpp"
#include "camera.cpp"
#include "loadFBX.cpp"
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
    glUniform1f(LightBinding.Power, 100.f);
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

struct model
{
    GLfloat *Vertices;
    GLfloat *Normals;
    GLushort *Indices;
    GLfloat *UVs;
    GLfloat *Colors;
    
    texture_material *Material;
    
    int IndexCount;
    
    GLuint VertexBuffer;
    GLuint NormalBuffer;
    GLuint IndexBuffer;
    GLuint ColorBuffer;
    GLuint UVBuffer;
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
    color_model *Model;
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;
};

struct game_data
{
    bool Initialized;

    light_texture_shader LightTextureShader;
    color_shader ColorShader;
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

    float BoxRotation;
};

void GLErrorShow()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
    {
	char* msg;
	if (error == GL_INVALID_OPERATION)
	{
	    msg = "Invalid Operation";
	}
	else if (error == GL_INVALID_ENUM)
	{
	    msg = "Invalid enum";
	}
	else if (error == GL_INVALID_VALUE)
	{
	    msg = "Invalid value";
	}
	else if (error == GL_OUT_OF_MEMORY)
	{
	    msg = "Out of memory";
	}
	else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
	{
	    msg = "Invalid framebuffer operation";
	}
	printf("OpenGL error: %d - %s\n", error, msg);
    }
}

texture LoadDDS(const char * filePath)
{
    texture NullTexture = {0};
    int8 header[124];

    FILE *fp = fopen(filePath, "rb");
    if (fp == 0)
    {
	printf("File not found: %s", filePath);
	return NullTexture;
    }

    char fileCode[4];
    fread(fileCode, 1, 4, fp);
    if (strncmp(fileCode, "DDS ", 4) != 0)
    {
	fclose(fp);
	printf("File is not DDS: %s", filePath);
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
	printf("File not DXT compressed: %s", filePath);
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
    glDisable(GL_TEXTURE_2D);
    return Result;
}

texture LoadBMP(char* filePath)
{
    texture NullTexture = { 0 };
    uint8 header[54];
    uint32 dataPos;
    uint32 width, height;
    uint32 imageSize;
    uint8* data;

    FILE * file = fopen(filePath, "rb");
    if (!file)
    {
	printf("File not found: %s\n", filePath);
	return NullTexture;
    }

    if (fread(header, 1, 54, file) != 54) {
	printf("Malformed BMP: %s\n", filePath);
	return NullTexture;
    }

    if (header[0] != 'B' || header[1]!= 'M')
    {
	printf("Malformed BMP: %s\n", filePath);
	return NullTexture;
    }

    dataPos = *(int32*)&(header[0x0a]);
    imageSize = *(int32*)&(header[0x22]);
    width = *(int32*)&(header[0x12]);
    height = *(int32*)&(header[0x16]);

    if (imageSize==0)
    {
	imageSize = width*height*3;
    }
    if (dataPos==0)
    {
	dataPos=54;
    }

    data = (uint8*)malloc(imageSize*sizeof(uint8));
    fread(data, 1, imageSize, file);
    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    //free(data);
    texture Result = {
	width,
	height,
	textureID,
	data
    };
    glDisable(GL_TEXTURE_2D);
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
    glClearColor(0.0, 0.0, 0.4, 0.0);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
//    glPolygonMode(GL_BACK, GL_LINE);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    model *BoxModel = &Game->BoxModel;
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
#define FACE					\
    0.0f, 0.0f,					\
	0.0f, 1.0f,				\
	1.0f, 0.0f,				\
	1.0f, 1.0f				\
	
	
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
    Game->BoxDiffuseMap = LoadDDS("uvtemplate.dds");
#elif defined(CONTAINER)
    Game->BoxDiffuseMap = LoadDDS("container.dds");
#endif

    Game->BoxSpecularMap = LoadDDS("containerspecular.dds");
    Game->BoxEmissiveMap = LoadDDS("containeremissive.dds");

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
    ColorMaterial->Shine = 0.0f;

    light_texture_shader Shader;
    Shader.Program = LoadShaders("Shaders/lightTextureShader.vert", "Shaders/lightTextureShader.frag");
    Shader.M = glGetUniformLocation(Shader.Program, "M");
    Shader.V = glGetUniformLocation(Shader.Program, "V");
    Shader.MVP = glGetUniformLocation(Shader.Program, "MVP");
	
    Shader.CameraPosition = glGetUniformLocation(Shader.Program, "CameraPosition");
    Shader.Light = CreateLightBinding(Shader.Program);
    
    Shader.Material = CreateMaterialBinding(Shader.Program);
    Game->LightTextureShader = Shader;

    color_shader ColorShader;
    ColorShader.Program = LoadShaders("Shaders/vertexShader.vert", "Shaders/fragmentShader.frag");
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
    Camera.FOV = PI*.5f;
    Camera.Aspect = 800.0f/600.0f;
    Camera.Near = 0.001f;
    Camera.Far = 1000.0f;
    Camera.Position = V3(0.0f, 0.0f, 5.0f);
    Camera.Forward = Normalize(V3(0.0f, 0.0f,-1.0f));
    Camera.Up = V3(0,1,0);
    Game->Camera = Camera;

    light Light = { 0 };
    Light.Position = V4(4.0f, 4.0f, 4.0f, 1.0f);
    Light.Ambient = V3(0.1f, 0.1f, 0.1f);
    Light.Diffuse = V3(0.5f, 0.5f, 0.5f);
    Light.Specular = V3(1.0f, 1.0f, 1.0f);
    Light.Power = 100.0f;
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
    ColorBox.Model = &Game->ColorBoxModel;
    ColorBox.Scale = V3(0.5f, 0.5f, 0.5f);
    ColorBox.Position = V3(Light.Position);
    ColorBox.Axis = V3(0.0f, 1.0f, 0.0f);
    Box2.Angle = 0.0f;
    Game->ColorBox = ColorBox;
    
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

    color_material *Material = GameObject.Model->Material;
    glUniformVec3f(Shader.Material.Diffuse, Material->Diffuse);
    glUniformVec3f(Shader.Material.Emissive, Material->Emissive);
    glUniformVec3f(Shader.Material.Specular, Material->Specular);
    glUniform1f(Shader.Material.Shine, Material->Shine);

    model ObjectModel = GameObject.Model->Model;
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

void RenderObject(game_object GameObject, camera Camera, light Light, mat4 Projection, mat4 View, light_texture_shader Shader)
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

void UpdateAndRender(platform_data* Platform)
{    
    game_data* Game = (game_data*)(((char*)Platform->MainMemory)+0);
//    input *LastInput = Platform->LastInput;
    input *Input = Platform->NewInput;
//    controller OldKeyboard = LastInput->Keyboard;
    controller Keyboard = Input->Keyboard;

    if (!Game->Initialized)
    {
	Init(Platform, Game);
    }
/*
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
*/
//    Game->Box.Angle += PI*(1.0f/120.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mat4 Projection = GenerateCameraPerspective(Game->Camera);
    mat4 View = GenerateCameraView(Game->Camera);

    RenderObject(Game->Box, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject(Game->Box2, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject(Game->LightBox, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    
    RenderObject(Game->ColorBox, Game->Camera, Game->Light, Projection, View, Game->ColorShader);
}
