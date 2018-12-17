#include "glHelper.cpp"
#include "platform.h"
#include "math.cpp"
#include "matrixMath.cpp"
#include "camera.cpp"
#include "loadFBX.cpp"
#include "graphics.cpp"

#include <stdlib.h>
#include <string.h>

struct game_object
{
    model *Model;
    v3 Scale;
    v3 Position;
    v3 Axis;
    float Angle;
};

struct game_object2
{
    model2 *Model;
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
    circle_shader CircleShader;
    camera Camera;
    
    texture BoxDiffuseMap;
    texture BoxSpecularMap;
    texture BoxEmissiveMap;
    texture_material BoxMaterial;
    color_material ColorMaterial;
    model BoxModel;
    model2 BoxModel2;
    model2 MonkeyModel;
    
    model3 BoxModel3;
    
    //Scene
    light Light;
    game_object Box2;
    game_object LightBox;
    
    game_object2 Box3;
    game_object2 Monkey;
 
    GLuint CircleDataBufferBinding;
    model QuadModel;
    game_object Quad;
    
    //New Scene
    game_object Player;
    color_game_object Water;
    color_model WaterColorModel;
    color_material WaterColorMaterial;
    texture WaterNormalMap;
    texture WaterDuDvMap;
    skybox SkyBox;
    GLuint ReflectionFBO;
    
    postprocessor Postprocessor;
};

GLuint LoadShaders(memory_arena* tempArena, char* vertexShaderFilePath, char* fragmentShaderFilePath)
{
    DebugLog("Loading %s & %s\n", vertexShaderFilePath, fragmentShaderFilePath);
    int readResult;
    FILE* vertexShaderFile = fopen(vertexShaderFilePath, "rb");
    fseek(vertexShaderFile, 0L, SEEK_END);
    int32 vertexShaderFileLength = ftell(vertexShaderFile);
    rewind(vertexShaderFile);
    
    int vertexShaderStringLength = vertexShaderFileLength+1;
    char* vertexShaderCode = PushArray(tempArena, vertexShaderStringLength, char);
    readResult = fread(vertexShaderCode, 1, vertexShaderFileLength, vertexShaderFile);
    fclose(vertexShaderFile);
    vertexShaderCode[vertexShaderFileLength] = '\0';
    
    FILE* fragmentShaderFile = fopen(fragmentShaderFilePath, "rb");
    fseek(fragmentShaderFile, 0L, SEEK_END);
    long fragmentShaderFileLength = ftell(fragmentShaderFile);
    rewind(fragmentShaderFile);
    
    int fragmentShaderStringLength = fragmentShaderFileLength+1;
    char *fragmentShaderCode = PushArray(tempArena, fragmentShaderStringLength, char);
    readResult = fread(fragmentShaderCode, 1, fragmentShaderFileLength, fragmentShaderFile);
    fragmentShaderCode[fragmentShaderFileLength] = '\0';
    
    GLuint ShaderProgram = CreateShaderProgram(vertexShaderCode, fragmentShaderCode);
    
    PopArray(tempArena, vertexShaderStringLength, char);
    PopArray(tempArena, fragmentShaderStringLength, char);
    
    return ShaderProgram;
}

void Init(platform_data* Platform, game_data *Game)
{
    InitArena(&Game->MainArena,
              Platform->MainMemorySize,
              (uint8*)Platform->MainMemory+sizeof(game_data));
    
    glClearColor(0.6, 0.4, 0.2, 0.0);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    GLuint PostprocessorProgram = LoadShaders(&Game->TempArena, "../res/Shaders/postprocessor.vert", "../res/Shaders/postprocessor.frag");

    framebuffer_object RBOFBO = CreateRenderTarget(GL_RENDERBUFFER, GL_RGB, GL_RGB, GL_DEPTH24_STENCIL8, 4, Platform->WindowWidth, Platform->WindowHeight);
    framebuffer_object TextureFBO = CreateRenderTarget(GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_DEPTH24_STENCIL8, 1, Platform->WindowWidth, Platform->WindowHeight);
    Game->Postprocessor = CreatePostprocessor(PostprocessorProgram, RBOFBO, TextureFBO);
    
    texture_material *BoxMaterial = &Game->BoxMaterial;
    BoxMaterial->DiffuseMap = Game->BoxDiffuseMap.Handle;
    BoxMaterial->SpecularMap = Game->BoxSpecularMap.Handle;
    //    BoxMaterial->EmissiveMap = Game->BoxEmissiveMap.Handle;
    BoxMaterial->Shine = 60.0f;
    
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
#define FrontNormal 0.0f, 0.0f, 1.0f
#define BackNormal 0.0f, 0.0f, -1.0f
#define UpNormal 0.0f, 1.0f, 0.0f
#define DownNormal 0.0, -1.0f, 0.0f
#define LeftNormal -1.0f, 0.0f, 0.0f
#define RightNormal 1.0f, 0.0f, 0.0f
#define Zero 0.0f, 0.0f, 0.0f
    GLfloat normalBufferData[] = {
        FrontNormal, FrontNormal, FrontNormal, FrontNormal,
        BackNormal, BackNormal, BackNormal, BackNormal,
        UpNormal, UpNormal, UpNormal, UpNormal,
        DownNormal, DownNormal, DownNormal, DownNormal,
        LeftNormal, LeftNormal, LeftNormal, LeftNormal,
        RightNormal, RightNormal, RightNormal, RightNormal,
    };
    
    model *BoxModel = &Game->BoxModel;
    glGenVertexArrays(1, &BoxModel->VertexArrayId);
    glBindVertexArray(BoxModel->VertexArrayId);
    size_t vertexBufferSize = sizeof(vertexBufferData);
    BoxModel->Vertices = (GLfloat*)PushSize(&Game->MainArena, vertexBufferSize);
    memcpy(BoxModel->Vertices, vertexBufferData, vertexBufferSize);
    glGenBuffers(1, &BoxModel->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 BoxModel->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexBufferSize,
                 &BoxModel->Vertices[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    size_t normalBufferSize = sizeof(normalBufferData);
    BoxModel->Normals = (GLfloat*)PushSize(&Game->MainArena, normalBufferSize);
    memcpy(BoxModel->Normals, normalBufferData, normalBufferSize);
    glGenBuffers(1, &BoxModel->NormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 BoxModel->NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 normalBufferSize,
                 &BoxModel->Normals[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    int IndexCount = sizeof(indexBufferData)/sizeof(GLushort);
    BoxModel->IndexCount = IndexCount;
    size_t indexBufferSize = sizeof(indexBufferData);
    BoxModel->Indices = (GLushort*)PushSize(&Game->MainArena, indexBufferSize);
    memcpy(BoxModel->Indices, indexBufferData, indexBufferSize);
    glGenBuffers(1, &BoxModel->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 BoxModel->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexBufferSize,
                 &BoxModel->Indices[0],
                 GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    {
        model2 *BoxModel2 = &Game->BoxModel2;
        BoxModel2->VertexBufferSize = sizeof(vertexBufferData);
        BoxModel2->Vertices = (GLfloat*)PushSize(&Game->MainArena, BoxModel2->VertexBufferSize);
        memcpy(BoxModel2->Vertices, vertexBufferData, BoxModel2->VertexBufferSize);
        
        BoxModel2->Normals = (GLfloat*)PushSize(&Game->MainArena, BoxModel2->VertexBufferSize);
        memcpy(BoxModel2->Normals, normalBufferData, normalBufferSize);
        
        BoxModel2->IndexCount = sizeof(indexBufferData)/sizeof(GLushort);
        BoxModel2->Indices = (GLushort*)PushSize(&Game->MainArena, BoxModel->IndexCount*sizeof(GLushort));
        memcpy(BoxModel2->Indices, indexBufferData, sizeof(indexBufferData));
        
        LoadModelGpu(BoxModel2);
        BoxModel2->VertexAttributeCount = 2;
    }
    
    if (false)
    {
        //attribute_buffer objects
        attribute_buffer AttributeBuffers[2];
        AttributeBuffers[0].Size = sizeof(vertexBufferData);
        AttributeBuffers[0].Usage = GL_STATIC_DRAW;
        AttributeBuffers[0].Data = (GLfloat *)PushSize(&Game->MainArena, sizeof(vertexBufferData));
        memcpy(AttributeBuffers[0].Data, vertexBufferData, sizeof(vertexBufferData));
        
        AttributeBuffers[1].Size = sizeof(normalBufferData);
        AttributeBuffers[1].Usage = GL_STATIC_DRAW;
        AttributeBuffers[1].Data = (GLfloat *)PushSize(&Game->MainArena, sizeof(normalBufferData));
        memcpy(AttributeBuffers[1].Data, normalBufferData, sizeof(normalBufferData));
        
        GLushort* indices = PushArray(&Game->MainArena, IndexCount, GLushort);
        int VertexCount = sizeof(vertexBufferData)/(sizeof(vertexBufferData[0]));
        model3 BoxModel3 = LoadBuffersToGpu(&Game->MainArena, VertexCount, AttributeBuffers, 2, indices, IndexCount);
        Game->BoxModel3 = BoxModel3;
    }
    
    // make water model
    color_game_object Water = {0};
    
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
    
    // Water Vertices
    int vertCount = 20;
    float vertWidth = 20.0f;
    Water.Position = V3(-0.5f*vertCount*vertWidth, 0.0f, 0.5f*vertCount*vertWidth);
    size_t waterBufferSize = sizeof(GLfloat)*(vertCount)*(vertCount)*3;
    Water.ColorModel->Model.Vertices = (GLfloat*)PushSize(&Game->MainArena, waterBufferSize);
    for(int x = 0; x < vertCount; x++)
    {
        for(int z = 0; z < vertCount; z++)
        {
            int start = ((x * vertCount) + z) * 3;
            Water.ColorModel->Model.Vertices[start] = x * vertWidth;
            Water.ColorModel->Model.Vertices[start+1] = 0.0f;
            Water.ColorModel->Model.Vertices[start+2] = -z * vertWidth;
        }
    }
    
    // Water Indices
    int segmentCount = vertCount-1;
    size_t waterIndexSize = sizeof(GLushort)*(segmentCount)*(segmentCount)*6;
    Water.ColorModel->Model.Indices = (GLushort*)PushSize(&Game->MainArena, waterIndexSize);
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
        }
    }
    
    Water.ColorModel->Model.IndexCount = waterIndexSize / sizeof(GLushort);
    
    // Water UVs
    size_t waterUVBufferSize = sizeof(GLfloat)*(vertCount)*(vertCount)*2;
    Water.ColorModel->Model.UVs = (GLfloat*)PushSize(&Game->MainArena, waterUVBufferSize); 
    for(int x = 0; x < vertCount; x++)
    {
        for(int z = 0; z < vertCount; z++)
        {
            int start = ((x * vertCount) + z) * 2;
            Water.ColorModel->Model.UVs[start] = x;
            Water.ColorModel->Model.UVs[start+1] = z;
        }
    }
    
    glGenVertexArrays(1, &Water.ColorModel->Model.VertexArrayId);
    glBindVertexArray(Water.ColorModel->Model.VertexArrayId);
    glGenBuffers(1, &Water.ColorModel->Model.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Water.ColorModel->Model.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, waterBufferSize, &Water.ColorModel->Model.Vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
                          );
    
    
    glGenBuffers(1, &Water.ColorModel->Model.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Water.ColorModel->Model.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, waterIndexSize, Water.ColorModel->Model.Indices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &Water.ColorModel->Model.UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Water.ColorModel->Model.UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, waterUVBufferSize, &Water.ColorModel->Model.UVs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0
                          );
    glBindVertexArray(0);
    
    Game->Water = Water;

#define FACE                                   \
    0.0f, 0.0f,                                \
    0.0f, 1.0f,                                \
    1.0f, 0.0f,                                \
    1.0f, 1.0f
    
    GLfloat uvBufferData[] = {
        FACE, FACE, FACE, FACE, FACE, FACE
    };
#undef FACE

    size_t uvBufferSize = sizeof(uvBufferData);
    BoxModel->UVs = (GLfloat*)PushSize(&Game->MainArena, uvBufferSize);
    memcpy(BoxModel->UVs, uvBufferData, uvBufferSize);
    
    glGenBuffers(1, &BoxModel->UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, BoxModel->UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, uvBufferSize, BoxModel->UVs, GL_STATIC_DRAW);

    Game->BoxDiffuseMap = LoadDDS(&Game->MainArena, "../res/Textures/container.dds");
    
    Game->BoxSpecularMap = LoadDDS(&Game->MainArena, "../res/Textures/containerspecular.dds");
    Game->BoxEmissiveMap = LoadDDS(&Game->MainArena, "../res/Textures/containeremissive.dds");
    Game->WaterNormalMap = GenTextureFromBMP(&Game->MainArena, "../res/Textures/matchingNormalMap.bmp");
    //Game->WaterNormalMap = GenTextureFromBMP("../res/Textures/normalMap.bmp");
    Game->WaterDuDvMap = GenTextureFromBMP(&Game->MainArena, "../res/Textures/waterDUDV.bmp");
    /*
    texture_material *BoxMaterial = &Game->BoxMaterial;
    BoxMaterial->DiffuseMap = Game->BoxDiffuseMap.Handle;
    BoxMaterial->SpecularMap = Game->BoxSpecularMap.Handle;
//    BoxMaterial->EmissiveMap = Game->BoxEmissiveMap.Handle;
    BoxMaterial->Shine = 60.0f;*/
    BoxModel->Material = BoxMaterial;
    
    color_material *MonkeyMaterial = PushObject(&Game->MainArena, color_material);
    MonkeyMaterial->Diffuse = V3(0.5f, 0.5f, 0.5f);
    MonkeyMaterial->Specular = V3(0.0f, 0.0f, 0.0f);
    MonkeyMaterial->Emissive = V3(0.0f, 0.0f, 0.0f);
    MonkeyMaterial->Shine = 60.0f;
    FILE* monkeyFile =  fopen("../res/Models/hull.fbx", "rb");
    model2 MonkeyModel = LoadModel(&Game->MainArena, &Game->TempArena, monkeyFile);
    LoadModelGpu(&MonkeyModel);
    Game->MonkeyModel = MonkeyModel;
    Game->MonkeyModel.Material = MonkeyMaterial;
    
    Game->BoxModel2.Material = MonkeyMaterial;
    
    light_texture_shader Shader;
    Shader.Program = LoadShaders(&Game->TempArena, "../res/Shaders/lightTextureShader.vert", "../res/Shaders/lightTextureShader.frag");
    Shader.M = glGetUniformLocation(Shader.Program, "M");
    Shader.V = glGetUniformLocation(Shader.Program, "V");
    Shader.MVP = glGetUniformLocation(Shader.Program, "MVP");
    
    Shader.CameraPosition = glGetUniformLocation(Shader.Program, "CameraPosition");
    Shader.Light = CreateLightBinding(Shader.Program);
    Shader.Material = CreateMaterialBinding(Shader.Program);
    Game->LightTextureShader = Shader;
    
    water_shader WaterShader;
    WaterShader.Program = LoadShaders(&Game->TempArena, "../res/Shaders/waterShader.vert", "../res/Shaders/waterShader.frag");
    WaterShader.M = glGetUniformLocation(WaterShader.Program, "M");
    WaterShader.V = glGetUniformLocation(WaterShader.Program, "V");
    WaterShader.MVP = glGetUniformLocation(WaterShader.Program, "MVP");
    WaterShader.CameraPosition = glGetUniformLocation(WaterShader.Program, "CameraPosition");
    WaterShader.Light = CreateLightBinding(WaterShader.Program);
    WaterShader.Material = CreateMaterialBinding(WaterShader.Program);
    WaterShader.NormalMap = glGetUniformLocation(WaterShader.Program, "NormalMap");
    WaterShader.ReflectionMap = glGetUniformLocation(WaterShader.Program, "ReflectionMap");
    WaterShader.DuDvMap = glGetUniformLocation(WaterShader.Program, "DuDvMap");
    WaterShader.NormalMapHandle = Game->WaterNormalMap.Handle;
    WaterShader.DuDvMapHandle = Game->WaterDuDvMap.Handle;
    WaterShader.UVOffsetHandle = glGetUniformLocation(WaterShader.Program, "UVOffset");
    WaterShader.UVOffset = 0;
    Game->WaterShader = WaterShader;
    
    color_shader ColorShader;
    ColorShader.Program = LoadShaders(&Game->TempArena, "../res/Shaders/vertexShader.vert", "../res/Shaders/fragmentShader.frag");
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
    
    circle_shader CircleShader;
    CircleShader.Program = LoadShaders(&Game->TempArena, "../res/Shaders/circleShader.vert", "../res/Shaders/circleShader.frag");
    CircleShader.M = glGetUniformLocation(CircleShader.Program, "M");
    CircleShader.V = glGetUniformLocation(CircleShader.Program, "V");
    CircleShader.MVP = glGetUniformLocation(CircleShader.Program, "MVP");
    CircleShader.Radius = glGetUniformLocation(CircleShader.Program, "Radius");
    CircleShader.Width = glGetUniformLocation(CircleShader.Program, "Width");
    CircleShader.FilledAngle = glGetUniformLocation(CircleShader.Program, "FilledAngle");
    CircleShader.FillColor = glGetUniformLocation(CircleShader.Program, "FillColor");
    Game->CircleShader = CircleShader;
    
    Game->QuadModel = CreateQuad();
    glGenBuffers(1, &Game->CircleDataBufferBinding);
    glBindBuffer(GL_ARRAY_BUFFER, Game->CircleDataBufferBinding);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadCircleData), &QuadCircleData[0], GL_STATIC_DRAW);
    Game->Quad.Model = &Game->QuadModel;
    Game->Quad.Scale = V3(1.0, 1.0, 1.0);
    Game->Quad.Position = V3(1.0, 1.0, 1.0);
    Game->Quad.Axis = V3(1.0, 1.0, 1.0);
    Game->Quad.Angle = 0.0f;
    
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
    Light.Ambient = V3(0.1f, 0.0f, 0.0f);
    Light.Diffuse = V3(0.5f, 0.0f, 0.0f);
    Light.Specular = V3(1.0f, 0.0f, 0.0f);
    Light.Power = 50.0f;
    Game->Light = Light;
    
    game_object2 Monkey = { 0 };
    Monkey.Model = &Game->MonkeyModel;
    Monkey.Scale = V3(1.0f, 1.0f, 1.0f);
    Monkey.Position = V3(4.0f, 0.0f, -40.0f);
    Monkey.Axis = V3(0.25f, 1.0f, .5f);
    Monkey.Angle = 0.0f;
    Game->Monkey = Monkey;
    
    game_object Box2 = { 0 };
    Box2.Model = &Game->BoxModel;
    Box2.Scale = V3(0.5f, 0.5f, 0.5f);
    Box2.Position = V3(4.0f, 0.0f, 0.0f);
    Box2.Axis = V3(0.0f, 1.0f, 0.0f);
    Box2.Angle = PI*0.25f;
    Game->Box2 = Box2;
    
    game_object2 Box3 = { 0 };
    Box3.Model = &Game->BoxModel2;
    Box3.Scale = V3(0.5f, 0.5f, 0.5f);
    Box3.Position = V3(5.0f, 2.0f, 0.0f);
    Box3.Axis = V3(0.0f, 1.0f, 0.0f);
    Box2.Angle = PI;
    Game->Box3 = Box3;
    
    game_object LightBox = { 0 };
    LightBox.Model = &Game->BoxModel;
    LightBox.Scale = V3(0.5f, 0.5f, 0.5f);
    LightBox.Position = V3(-4.0f, 0.0f, 0.0f);
    LightBox.Axis = V3(0.0f, 1.0f, 0.0f);
    LightBox.Angle = 0.0f;
    Game->LightBox = LightBox;
    
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
    SkyBox->Vertices = (GLfloat*)PushSize(&Game->MainArena, skyboxVertexBufferSize);
    memcpy(SkyBox->Vertices, skyboxVertexBufferData, skyboxVertexBufferSize);
    
    glGenVertexArrays(1, &SkyBox->VertexArrayId);
    glBindVertexArray(SkyBox->VertexArrayId);
    glGenBuffers(1, &SkyBox->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 SkyBox->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 skyboxVertexBufferSize,
                 &SkyBox->Vertices[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glBindVertexArray(0);
    
    
    SkyBox->Texture = GenCubeMapFromBMP(
        &Game->MainArena,
        "../res/Textures/skybox/right.bmp",
        "../res/Textures/skybox/left.bmp",
        "../res/Textures/skybox/top.bmp",
        "../res/Textures/skybox/bottom.bmp",
        "../res/Textures/skybox/back.bmp",
        "../res/Textures/skybox/front.bmp");
    
    skybox_shader SkyBoxShader;
    SkyBoxShader.Program = LoadShaders(&Game->TempArena, "../res/Shaders/skybox.vert", "../res/Shaders/skybox.frag");
    SkyBoxShader.M = glGetUniformLocation(SkyBoxShader.Program, "M");
    SkyBoxShader.V = glGetUniformLocation(SkyBoxShader.Program, "V");
    SkyBoxShader.MVP = glGetUniformLocation(SkyBoxShader.Program, "MVP");
    SkyBoxShader.SkyBox = SkyBox->Texture.Handle;
    Game->SkyBoxShader = SkyBoxShader;
    
    glGenFramebuffers(1, &Game->ReflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, Game->ReflectionFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    glGenTextures(1, &Game->WaterShader.ReflectionHandle);
    glBindTexture(GL_TEXTURE_2D, Game->WaterShader.ReflectionHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Game->WaterShader.ReflectionHandle, 0);
    
    PrintGlFBOError();
    
    Game->Initialized = true;
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
    
    glBindVertexArray(SkyBox.VertexArrayId);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
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
    
    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.M, 1, GL_FALSE, &Model.E[0][0]);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);
    
    glUniformVec3f(Shader.CameraPosition, Camera.Position);
    SetPointLightUniforms(Shader.Light, Light);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Shader.NormalMapHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Shader.DuDvMapHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Shader.ReflectionHandle);
    glUniform1i(Shader.NormalMap, 0);
    glUniform1i(Shader.DuDvMap, 1);
    glUniform1i(Shader.ReflectionMap, 2);
    
    color_material *Material = GameObject.ColorModel->Material;
    glUniformVec3f(Shader.Material.Diffuse, Material->Diffuse);
    glUniformVec3f(Shader.Material.Emissive, Material->Emissive);
    glUniformVec3f(Shader.Material.Specular, Material->Specular);
    glUniform1f(Shader.Material.Shine, Material->Shine);
    
    glUniform1f(Shader.UVOffsetHandle, Shader.UVOffset);
    
    model ObjectModel = GameObject.ColorModel->Model;
    glBindVertexArray(ObjectModel.VertexArrayId);
    glDrawElements(GL_TRIANGLES,
                   ObjectModel.IndexCount,
                   GL_UNSIGNED_SHORT,
                   (void*)0
                   );
    glBindVertexArray(0);
}

float t = -PI;

void RenderObject(game_data* Game, game_object GameObject, camera Camera, mat4 Projection, mat4 View, circle_shader Shader)
{
    mat4 Rotation = MakeRotation(GameObject.Axis, GameObject.Angle);
    mat4 Scale = MakeScale(GameObject.Scale);
    mat4 Translation = MakeTranslation(GameObject.Position);
    mat4 ModelTransform = Translation * Rotation * Scale;
    mat4 MVP = Projection * View * ModelTransform;

    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.M, 1, GL_FALSE, &ModelTransform.E[0][0]);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);
    
    glUniform1f(Shader.Radius, 0.5f);
    glUniform1f(Shader.Width, 0.1f);
    glUniform1f(Shader.FilledAngle, t);
    t+= PI/100;
    if (t >= PI) {
        t = -PI;
    }
    glUniform3f(Shader.FillColor, 1.0f, 0.0f, 0.0f);
    
    DrawModel(GameObject.Model);
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
    
    
    DrawModel(GameObject.Model);
}

void RenderObject2(game_object2 GameObject, camera Camera, light Light, mat4 Projection, mat4 View, color_shader Shader)
{
    mat4 Rotation = MakeRotation(GameObject.Axis, GameObject.Angle);
    mat4 Scale = MakeScale(GameObject.Scale);
    mat4 Translation = MakeTranslation(GameObject.Position);
    mat4 ModelTransform = Translation * Rotation * Scale;
    mat4 MVP = Projection * View * ModelTransform;

    glUseProgram(Shader.Program);
    glUniformMatrix4fv(Shader.M, 1, GL_FALSE, &ModelTransform.E[0][0]);
    glUniformMatrix4fv(Shader.V, 1, GL_FALSE, &View.E[0][0]);
    glUniformMatrix4fv(Shader.MVP, 1, GL_FALSE, &MVP.E[0][0]);
    
    glUniformVec3f(Shader.CameraPosition, Camera.Position);
    SetPointLightUniforms(Shader.Light, Light);
    
    color_material *Material = GameObject.Model->Material;
    glUniformVec3f(Shader.Material.Diffuse, Material->Diffuse);
    glUniformVec3f(Shader.Material.Emissive, Material->Emissive);
    glUniformVec3f(Shader.Material.Specular, Material->Specular);
    glUniform1f(Shader.Material.Shine, Material->Shine);
    
//    DrawModel(GameObject.Model);
    model2* Model = GameObject.Model;
    
    glBindVertexArray(Model->VertexArrayId);
    glDrawElements(GL_TRIANGLES,
                   Model->IndexCount,
                   GL_UNSIGNED_SHORT,
                   (void*)0
                   );
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
    
    Game->WaterShader.UVOffset += 0.0005f;
    if(Game->WaterShader.UVOffset >= 1.0f)
    {
        Game->WaterShader.UVOffset -= 1.0f;
    }
}

void RenderScene(game_data *Game, mat4 Projection, mat4 View, bool includeWater)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    RenderObject(Game->Box2, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject(Game->LightBox, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject(Game->Player, Game->Camera, Game->Light, Projection, View, Game->LightTextureShader);
    RenderObject2(Game->Monkey, Game->Camera, Game->Light, Projection, View, Game->ColorShader);
    RenderObject2(Game->Box3, Game->Camera, Game->Light, Projection, View, Game->ColorShader);
    
    RenderObject(Game, Game->Quad, Game->Camera, Projection, View, Game->CircleShader);
    
    // player and water
    if(includeWater)
    {
        RenderWater(Game->Water, Game->Camera, Game->Light, Projection, View, Game->WaterShader);
    }
    
    // skybox comes last, to save on performance
    RenderSkyBox(Game->SkyBox, Game->Camera, Game->Light, Projection, View, Game->SkyBoxShader);
}

void RenderToTarget(platform_data *Platform, game_data *Game, mat4 Projection, mat4 View, FramebufferDesc *TargetBuffer, int BufferWidth, int BufferHeight)
{
    
    glEnable(GL_MULTISAMPLE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, TargetBuffer->RenderFramebufferId);
    glViewport(0, 0, BufferWidth, BufferHeight);
    RenderScene(Game, Projection, View, true);
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
    
    // render reflection
    mat4 ReflectView = GenerateReflectionCameraView(Game->Camera);
    glBindFramebuffer(GL_FRAMEBUFFER, Game->ReflectionFBO);
    glViewport(0, 0, 800, 600);
    RenderScene(Game, Projection, ReflectView, false);
    
    BeginPostprocessor(Platform, Game->Postprocessor);
    RenderScene(Game, Projection, View, true);
    EndPostprocessor(Platform, Game->Postprocessor);
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
        printf("End errors\n");
    }
    Update(Platform, Game);
    GLErrorShow();
    Render(Platform, Game);
}
