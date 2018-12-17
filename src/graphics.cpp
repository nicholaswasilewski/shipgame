#ifndef _GRAPHICS_CPP__

#include "texture.cpp"

struct light
{
    v4 Position;
    v3 Ambient;
    v3 Diffuse;
    v3 Specular;
    float Power;
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

enum uniform_type {
    Uniform1f,
    Uniform1fv,
    Uniform1i,
    Uniform1iv,
    Uniform1ui,
    Uniform1uiv,
    Uniform2f,
    Unifom2fv,
    Uniform2i,
    Uniform2iv,
    Uniform2ui,
    Uniform2uiv,
    Uniform3f,
    Uniform3fv,
    Uniform3i,
    Uniform3iv,
    Uniform3ui,
    Uniform3uiv,
    Uniform4f,
    Uniform4fv,
    Uniform4i,
    Uniform4iv,
    Uniform4ui,
    Uniform4uiv,
    UniformMatrix2fv,
    UniformMatrix2x3fv,
    UniformMatrix2x4fv,
    UniformMatrix3fv,
    UniformMatrix3x2fv,
    UniformMatrix3x4fv,
    UniformMatrix4fv,
    UniformMatrix4x2fv,
    UniformMatrix4x3fv
};

struct uniform 
{
    uniform_type Type;
    GLint Location;
    void* Value;
};

struct mat_texture
{
    GLenum TextureUnit;
    texture Texture;
};

struct material
{
    int TextureCount;
    mat_texture* Textures;
    int UniformCount;
    uniform* Uniforms;
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

struct circle_shader {
    GLuint Program;
    GLuint M;
    GLuint V;
    GLuint MVP;
    GLuint Radius;
    GLuint Width;
    GLuint FilledAngle;
    GLuint FillColor;
};

struct shader
{
    GLuint Program;
    
    GLuint M;
    GLuint V;
    GLuint MVP;
    GLuint Radius;
    GLuint Width;
    GLuint FilledAngle;
    GLuint FillColor;
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
    
    GLuint NormalMap;
    GLuint NormalMapHandle;
    
    GLuint DuDvMap;
    GLuint DuDvMapHandle;
    
    float UVOffset;
    GLuint UVOffsetHandle;
    
    GLuint ReflectionMap;
    GLuint ReflectionHandle;
};

struct vertex_attrib_info 
{
    GLuint Index;
    GLint Size;
    GLenum Type;
    GLboolean Normalized;
    GLsizei Stride;
    GLvoid* Offset;
};

vertex_attrib_info STANDARD_3D_VERTEX_ATTRIB_INFO = {
    0,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    (GLvoid*)0
};

vertex_attrib_info STANDARD_NORMAL_VERTEX_ATTRIB_INFO = {
    1,
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    (GLvoid*)0
};

struct buffer_info
{
    GLuint DataBufferId;
    GLenum Target;
    vertex_attrib_info VertexAttribInfo;
    
    int BufferLength;
    void* Data;
};

struct index_buffer_info
{
    GLuint Id;
    GLsizei Count;
    GLenum Type;  // GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT
    void* Offset;
    void* Data;
};

void SetIndexBuffer(GLuint IndexBufferId)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
}

void SetVertexAttribPointerInfo(vertex_attrib_info VertexAttribInfo)
{
    glEnableVertexAttribArray(VertexAttribInfo.Index);
    glVertexAttribPointer(VertexAttribInfo.Index,
                          VertexAttribInfo.Size,
                          VertexAttribInfo.Type,
                          VertexAttribInfo.Normalized,
                          VertexAttribInfo.Stride,
                          VertexAttribInfo.Offset);
}

void EnableVertexAttributeBuffer(buffer_info BufferInfo)
{
    glBindBuffer(BufferInfo.Target, BufferInfo.DataBufferId);
    SetVertexAttribPointerInfo(BufferInfo.VertexAttribInfo);
}

void DisableVertexAttributeBuffer(buffer_info BufferInfo)
{
    glDisableVertexAttribArray(BufferInfo.VertexAttribInfo.Index);
}

struct model2
{
    GLfloat *Vertices;
    GLfloat *Normals;
    GLushort *Indices;
    GLfloat *UVs;
    GLfloat *Colors;
    
    //vertex buffer size and normal buffer size need to be the same so just keep one I guess.
    GLsizei VertexBufferSize;
    int IndexCount;
    int VertexAttributeCount;
    /*Stuff above this line should be set before the BindModel process.*/
    
    color_material* Material;
    
    GLuint VertexArrayId;
    GLuint IndexBufferId;
    union {
        struct {
            buffer_info VertexBufferInfo;
            buffer_info NormalBufferInfo;
            buffer_info IndexBufferInfo;
            buffer_info ColorBufferInfo;
            buffer_info UVBufferInfo;
        };
        buffer_info VertexAttributeBuffers[5];
    };
};

struct attribute_buffer
{
    int Size;
    GLenum Usage;
    void* Data;
};

struct model3
{
    GLuint Shader;
    
    int IndexCount;
    index_buffer_info* IndexBuffer;
    
    int VertexCount;
    int AttributeCount;
    buffer_info* Attributes;
    
    int UniformCount;
    uniform* Uniforms;
};

model3 LoadBuffersToGpu(memory_arena* Arena, int vertexCount, attribute_buffer* unloadedBuffers, int bufferCount, GLushort* indices, int indexCount)
{
    model3 Model = {0};
    Model.VertexCount = vertexCount;
    GLuint* BufferIds = PushArray(Arena, bufferCount, GLuint);
    glGenBuffers(bufferCount, BufferIds);
    buffer_info* LoadedBuffers = PushArray(Arena, bufferCount, buffer_info);
    
    for(int i = 0; i < bufferCount; i++)
    {
        glBindBuffer(1, BufferIds[i]);
        glBufferData(GL_ARRAY_BUFFER,
                     unloadedBuffers[i].Size,
                     unloadedBuffers[i].Data,
                     unloadedBuffers[i].Usage);
        
        LoadedBuffers[i].DataBufferId = BufferIds[i];
        LoadedBuffers[i].Data = unloadedBuffers[i].Data;
        LoadedBuffers[i].Target = GL_ARRAY_BUFFER;
    }
    
    Model.AttributeCount = bufferCount;
    Model.Attributes = LoadedBuffers;
    
    if (indexCount > 0 && indices != 0)
    {
        index_buffer_info* IndexBufferInfo = PushObject(Arena, index_buffer_info);
        Model.IndexBuffer = IndexBufferInfo;
        
        glGenBuffers(1, &IndexBufferInfo->Id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     IndexBufferInfo->Id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(GLushort)*indexCount,
                     indices,
                     GL_STATIC_DRAW);
        
        IndexBufferInfo->Count = indexCount;
        IndexBufferInfo->Type = GL_UNSIGNED_SHORT;
        IndexBufferInfo->Offset = 0;
        IndexBufferInfo->Data = indices;
        
    }
    
    return Model;
}

void LoadModelGpu(model2 *Model)
{
    glGenVertexArrays(1, &Model->VertexArrayId);
    glBindVertexArray(Model->VertexArrayId);
    
    buffer_info* VertexBufferInfo = &Model->VertexBufferInfo;
    VertexBufferInfo->Target = GL_ARRAY_BUFFER;
    glGenBuffers(1, &VertexBufferInfo->DataBufferId);
    glBindBuffer(VertexBufferInfo->Target,
                 VertexBufferInfo->DataBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 Model->VertexBufferSize,
                 Model->Vertices,
                 GL_STATIC_DRAW);
    VertexBufferInfo->VertexAttribInfo = STANDARD_3D_VERTEX_ATTRIB_INFO;
    SetVertexAttribPointerInfo(VertexBufferInfo->VertexAttribInfo);
    
    buffer_info* NormalBufferInfo = &Model->NormalBufferInfo;
    NormalBufferInfo->Target = GL_ARRAY_BUFFER;
    glGenBuffers(1, &NormalBufferInfo->DataBufferId);
    glBindBuffer(NormalBufferInfo->Target,
                 NormalBufferInfo->DataBufferId);
    glBufferData(GL_ARRAY_BUFFER,
                 Model->VertexBufferSize,
                 Model->Normals,
                 GL_STATIC_DRAW);
    NormalBufferInfo->VertexAttribInfo = STANDARD_NORMAL_VERTEX_ATTRIB_INFO;
    SetVertexAttribPointerInfo(NormalBufferInfo->VertexAttribInfo);
    
    glGenBuffers(1, &Model->IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 Model->IndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLushort)*Model->IndexCount,
                 Model->Indices,
                 GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

struct model
{
    GLfloat *Vertices;
    GLfloat *Normals;
    GLushort *Indices;
    GLfloat *UVs;
    GLfloat *Colors;
    
    
    //vertex buffer size and normal buffer size need to be the same so 
    GLsizei VertexBufferSize;
    
    texture_material *Material;
    
    int IndexCount;
    
    GLuint VertexArrayId;
    GLuint VertexBuffer;
    GLuint NormalBuffer;
    GLuint IndexBuffer;
    GLuint ColorBuffer;
    GLuint UVBuffer;
    
    union {
        struct {
            buffer_info VertexBufferInfo;
            buffer_info NormalBufferInfo;
            buffer_info IndexBufferInfo;
            buffer_info ColorBufferInfo;
            buffer_info UVBufferInfo;
        };
        buffer_info BufferInfo[5];
    };
};

struct skybox
{
    GLfloat *Vertices;
    GLuint VertexArrayId;
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

GLfloat QuadVertexData[] {
    0.0f, 1.0f, 0.0f, //UL
    0.0f, 0.0f, 0.0f, //LL
    1.0f, 1.0f, 0.0f, //UR
    1.0f, 0.0f, 0.0f  //LR
};

GLushort QuadIndexData[] {
    0, 1, 2, //UL
    1, 3, 2  //LR
};

GLfloat QuadCircleData[] {
    -1.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, 1.0f,
    1.0f, -1.0f
};

void DrawModel(model* Model)
{
    //TODO: Add capacity to draw unindexed values
    glBindVertexArray(Model->VertexArrayId);
    glDrawElements(GL_TRIANGLES,
                   Model->IndexCount,
                   GL_UNSIGNED_SHORT,
                   (void*)0);
    glBindVertexArray(0);
}

model CreateQuad() {
    model Quad = {};
    Quad.Vertices = &QuadVertexData[0];
    Quad.Indices = &QuadIndexData[0];
    Quad.IndexCount = 6;
    
    glGenVertexArrays(1, &Quad.VertexArrayId);
    glBindVertexArray(Quad.VertexArrayId);
    
    glGenBuffers(1, &Quad.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 
                 Quad.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(QuadVertexData),
                 &Quad.Vertices[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    GLuint CircleDataBuffer;
    glGenBuffers(1, &CircleDataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, CircleDataBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(QuadCircleData),
                 &QuadCircleData[0],
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    
    glGenBuffers(1, &Quad.IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 Quad.IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(QuadIndexData),
                 &Quad.Indices[0],
                 GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    return Quad;
}

GLuint CompileShader(char* shaderCode, GLenum shaderType)
{
    GLint result = GL_FALSE;
    int32 infoLogLength;
    
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderCode, 0);
    glCompileShader(shaderID);
    
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        DebugLog("Shader: %s\n", shaderCode);
        char* error = (char*)malloc(infoLogLength);
        glGetShaderInfoLog(shaderID, infoLogLength, 0, error);
        DebugLog("%s error:\n", error);
        free(error);
    }
    
    return shaderID;
}

GLuint CreateShaderProgram(char* vertexShaderCode, char* fragmentShaderCode)
{
    GLint result = GL_FALSE;
    int32 infoLogLength;
    int readResult;
    
    GLuint vertexShaderID = CompileShader(vertexShaderCode, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
    
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
        DebugLog("%s\n%s\n%s\n", vertexShaderCode, fragmentShaderCode, error);
        free(error);
    }
    
    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    return programID;
}

struct framebuffer_object {
    uint32 Id;
    uint32 ColorBufferId;
    uint32 ColorBufferType;
    uint32 DepthStencilBufferId;
};

framebuffer_object CreateRenderTarget(GLuint ColorBufferType, GLuint InternalFormat, GLuint ExternalFormat, GLuint DepthStencilFormat, int Samples, int Width, int Height)
{
    framebuffer_object FBO;
    glGenFramebuffers(1, &FBO.Id);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO.Id);
    
    if (Samples <= 1)
    {
        if (ColorBufferType == GL_TEXTURE_2D)
        {
            FBO.ColorBufferType = GL_TEXTURE_2D;
            glGenTextures(1, &FBO.ColorBufferId);
            glBindTexture(FBO.ColorBufferType, FBO.ColorBufferId);
            glTexParameteri(FBO.ColorBufferType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(FBO.ColorBufferType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(FBO.ColorBufferType, 0, InternalFormat, Width, Height, 0, ExternalFormat, GL_UNSIGNED_BYTE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FBO.ColorBufferType, FBO.ColorBufferId, 0);
        } 
        else if (ColorBufferType == GL_RENDERBUFFER) 
        {
            FBO.ColorBufferType = GL_RENDERBUFFER;
            glGenRenderbuffers(1, &FBO.ColorBufferId);
            glBindRenderbuffer(GL_RENDERBUFFER, FBO.ColorBufferId);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, Width, Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO.ColorBufferId);
            
        }
        
        glGenRenderbuffers(1, &FBO.DepthStencilBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, FBO.DepthStencilBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, FBO.DepthStencilBufferId);
    }
    else
    {
        if (ColorBufferType == GL_TEXTURE_2D) {
            FBO.ColorBufferType = GL_TEXTURE_2D_MULTISAMPLE;
            glGenTextures(1, &FBO.ColorBufferId);
            glBindTexture(FBO.ColorBufferType, FBO.ColorBufferId);
            glTexImage2DMultisample(FBO.ColorBufferType, Samples, InternalFormat, Width, Height, GL_TRUE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FBO.ColorBufferType, FBO.ColorBufferId, 0);
        }
        else if (ColorBufferType == GL_RENDERBUFFER)
        {
            FBO.ColorBufferType = GL_RENDERBUFFER;
            glGenRenderbuffers(1, &FBO.ColorBufferId);
            glBindRenderbuffer(GL_RENDERBUFFER, FBO.ColorBufferId);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_RGB, Width, Height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO.ColorBufferId);
        }
        
        glGenRenderbuffers(1, &FBO.DepthStencilBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, FBO.DepthStencilBufferId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH24_STENCIL8, Width, Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, FBO.DepthStencilBufferId);
    }
    GLErrorShow();
    PrintGlFBOError();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (ColorBufferType == GL_RENDERBUFFER)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    else 
    {
        glBindTexture(FBO.ColorBufferType, 0);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    GLErrorShow();
    
    return FBO;
}

GLfloat screen_quad_vertices[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    -1.0, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
};

void BindRenderTarget(framebuffer_object FBO)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO.Id);
}

struct postprocessor {
    framebuffer_object RBOFBO;
    framebuffer_object TextureFBO;
    GLuint VAO;
    GLuint Program;
};

postprocessor CreatePostprocessor(GLuint ShaderProgram, framebuffer_object RBOFBO, framebuffer_object TextureFBO) {
    postprocessor pp;
    pp.Program = ShaderProgram;
    pp.RBOFBO = RBOFBO;
    pp.TextureFBO = TextureFBO;
    
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_vertices), screen_quad_vertices, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &pp.VAO);
    glBindVertexArray(pp.VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    return pp;
}

void BeginPostprocessor(platform_data *Platform, postprocessor Postprocessor)
{
    BindRenderTarget(Postprocessor.RBOFBO);
    glViewport(0, 0, Platform->WindowWidth, Platform->WindowHeight);
}

void EndPostprocessor(platform_data *Platform, postprocessor Postprocessor)
{
    GLuint ReadBuffer = Postprocessor.RBOFBO.Id;
    GLuint DrawBuffer = Postprocessor.TextureFBO.Id;
    glBlitNamedFramebuffer(ReadBuffer, DrawBuffer,
                           0, 0, Platform->WindowWidth, Platform->WindowHeight, 
                           0, 0, Platform->WindowWidth, Platform->WindowHeight,
                           GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    glBlitNamedFramebuffer(DrawBuffer, 0, 
                           0, 0, Platform->WindowWidth, Platform->WindowHeight, 
                           0, 0, Platform->WindowWidth, Platform->WindowHeight,
                           GL_COLOR_BUFFER_BIT, GL_LINEAR);
    /*
    // Post-process scene texture into front buffer.
    glBindVertexArray(Postprocessor.VAO);
    glUseProgram(Postprocessor.Program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Postprocessor.TextureFBO.ColorBufferId);
    // Bind front buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    */
}

#define _GRAPHICS_CPP__
#endif
