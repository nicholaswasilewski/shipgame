#ifndef _GRAPHICS_CPP__

#include "platform.h"
#include "glHelper.cpp"
#include "matrixMath.cpp"
#include "numerical_types.h"
#include "memory.h"
#include "texture.cpp"


struct light
{
    v4 Position;
    v3 Ambient;
    v3 Diffuse;
    v3 Specular;
    float Power;
};

struct texture_color_material
{
    float Shine;
    
    v3 Ambient;
    v3 Diffuse;
    v3 Specular;
    v3 Emissive;
    
    GLuint DiffuseMap;
    GLuint SpecularMap;
    GLuint EmissiveMap;
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
    MaterialBinding.Diffuse = GL(glGetUniformLocation(ShaderProgram, "Material.Diffuse"));
    MaterialBinding.Specular = GL(glGetUniformLocation(ShaderProgram, "Material.Specular"));
    MaterialBinding.Emissive = GL(glGetUniformLocation(ShaderProgram, "Material.Emissive"));
    MaterialBinding.Shine = GL(glGetUniformLocation(ShaderProgram, "Material.Shine"));
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
    Uniform2fv,
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

struct uniform_list
{
    int Count;
    uniform* Uniforms;
};

uniform CreateUniform(GLuint shader, uniform_type type, char* name)
{
    uniform Value = { type, glGetUniformLocation(shader, name), 0 };
    return Value;
}

// TODO: maybe do some macro wizardry here.
// Probably don't support the non-vectorized types.
void SetUniform(uniform Uniform)
{
    switch(Uniform.Type)
    {
        case Uniform1f: {
            glUniform1f(Uniform.Location, 1, (GLfloat)((int)Uniform.Value));
        } break;
        case Uniform1fv: {
            glUniform1fv(Uniform.Location, 1, (GLfloat*)Uniform.Value);
        } break;
        case Uniform2fv: {
            glUniform2fv(Uniform.Location, 1, (GLfloat*)Uniform.Value);
        } break;
        case Uniform3fv: {
            glUniform3fv(Uniform.Location, 1, (GLfloat*)Uniform.Value);
        } break;
        case UniformMatrix4fv: {
            glUniformMatrix4fv(Uniform.Location, 1, GL_FALSE, (GLfloat*)Uniform.Value);
        } break;
        case Uniform1i: {
            glUniform1f(Uniform.Location, (int)Uniform.Value);
        } break;
    }
}

void SetUniformList(uniform_list UniformList)
{
    for(int i = 0; i < UniformList.Count; i++)
    {
        SetUniform(UniformList.Uniforms[i]);
    }
}

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
    LightBinding.Power = GL(glGetUniformLocation(ShaderProgram, "Light.Power"));
    LightBinding.Position = GL(glGetUniformLocation(ShaderProgram, "Light.Position"));
    LightBinding.Ambient = GL(glGetUniformLocation(ShaderProgram, "Light.Ambient"));
    LightBinding.Diffuse = GL(glGetUniformLocation(ShaderProgram, "Light.Diffuse"));
    LightBinding.Specular = GL(glGetUniformLocation(ShaderProgram, "Light.Specular"));
    return LightBinding;
}

void SetPointLightUniforms(light_binding LightBinding, light Light)
{
    GL(glUniformVec4f(LightBinding.Position, Light.Position));
    GL(glUniformVec3f(LightBinding.Ambient, Light.Ambient));
    GL(glUniformVec3f(LightBinding.Diffuse, Light.Diffuse));
    GL(glUniformVec3f(LightBinding.Specular, Light.Specular));
    GL(glUniform1f(LightBinding.Power, Light.Power));
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

struct color_uniforms
{
    uniform M;
    uniform V;
    uniform MVP;
    uniform CameraPosition;
    
    uniform LightPower;
    uniform LightPosition;
    uniform LightAmbient;
    uniform LightDiffuse;
    uniform LightSpecular;
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
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId));
}

void SetVertexAttribPointerInfo(vertex_attrib_info VertexAttribInfo)
{
    GL(glEnableVertexAttribArray(VertexAttribInfo.Index));
    GL(glVertexAttribPointer(VertexAttribInfo.Index,
                             VertexAttribInfo.Size,
                             VertexAttribInfo.Type,
                             VertexAttribInfo.Normalized,
                             VertexAttribInfo.Stride,
                             VertexAttribInfo.Offset));
}

void EnableVertexAttributeBuffer(buffer_info BufferInfo)
{
    GL(glBindBuffer(BufferInfo.Target, BufferInfo.DataBufferId));
    SetVertexAttribPointerInfo(BufferInfo.VertexAttribInfo);
}

void DisableVertexAttributeBuffer(buffer_info BufferInfo)
{
    GL(glDisableVertexAttribArray(BufferInfo.VertexAttribInfo.Index));
}

struct attribute_buffer
{
    int Size;
    GLenum Usage;
    void* Data;
};

struct model
{
    GLfloat *Vertices;
    GLfloat *Normals;
    GLushort *Indices;
    GLfloat *UVs;
    GLfloat *Colors;
    
    
    //vertex buffer size and normal buffer size need to be the same so 
    GLsizei VertexBufferSize;
    texture_color_material *Material;
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

struct model2
{
    // Actual data.
    GLfloat *Vertices;
    GLfloat *Normals;
    GLushort *Indices;
    GLfloat *UVs;
    GLfloat *Colors;
    
    // vertex buffer size and normal buffer size need to be the same so just keep one I guess.
    GLsizei VertexBufferSize;
    int IndexCount;
    int VertexAttributeCount;
    /*Stuff above this line should be set before the BindModel process.*/
    
    texture_color_material* Material;
    
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

void LoadModelGpu(model2 *Model)
{
    GL(glGenVertexArrays(1, &Model->VertexArrayId));
    GL(glBindVertexArray(Model->VertexArrayId));
    
    buffer_info* VertexBufferInfo = &Model->VertexBufferInfo;
    VertexBufferInfo->Target = GL_ARRAY_BUFFER;
    GL(glGenBuffers(1, &VertexBufferInfo->DataBufferId));
    GL(glBindBuffer(VertexBufferInfo->Target,
                    VertexBufferInfo->DataBufferId));
    GL(glBufferData(GL_ARRAY_BUFFER,
                    Model->VertexBufferSize,
                    Model->Vertices,
                    GL_STATIC_DRAW));
    VertexBufferInfo->VertexAttribInfo = STANDARD_3D_VERTEX_ATTRIB_INFO;
    SetVertexAttribPointerInfo(VertexBufferInfo->VertexAttribInfo);
    
    buffer_info* NormalBufferInfo = &Model->NormalBufferInfo;
    NormalBufferInfo->Target = GL_ARRAY_BUFFER;
    GL(glGenBuffers(1, &NormalBufferInfo->DataBufferId));
    GL(glBindBuffer(NormalBufferInfo->Target,
                    NormalBufferInfo->DataBufferId));
    GL(glBufferData(GL_ARRAY_BUFFER,
                    Model->VertexBufferSize,
                    Model->Normals,
                    GL_STATIC_DRAW));
    NormalBufferInfo->VertexAttribInfo = STANDARD_NORMAL_VERTEX_ATTRIB_INFO;
    SetVertexAttribPointerInfo(NormalBufferInfo->VertexAttribInfo);
    
    GL(glGenBuffers(1, &Model->IndexBufferId));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                    Model->IndexBufferId));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(GLushort)*Model->IndexCount,
                    Model->Indices,
                    GL_STATIC_DRAW));
    
    GL(glBindVertexArray(0));
}

void LoadModelGpuWithUVs(model2 *Model)
{
    GL(glGenVertexArrays(1, &Model->VertexArrayId));
    GL(glBindVertexArray(Model->VertexArrayId));
    
    buffer_info* VertexBufferInfo = &Model->VertexBufferInfo;
    VertexBufferInfo->Target = GL_ARRAY_BUFFER;
    GL(glGenBuffers(1, &VertexBufferInfo->DataBufferId));
    GL(glBindBuffer(VertexBufferInfo->Target,
                    VertexBufferInfo->DataBufferId));
    GL(glBufferData(GL_ARRAY_BUFFER,
                    Model->VertexBufferSize,
                    Model->Vertices,
                    GL_STATIC_DRAW));
    VertexBufferInfo->VertexAttribInfo = STANDARD_3D_VERTEX_ATTRIB_INFO;
    SetVertexAttribPointerInfo(VertexBufferInfo->VertexAttribInfo);
    
    buffer_info* NormalBufferInfo = &Model->NormalBufferInfo;
    NormalBufferInfo->Target = GL_ARRAY_BUFFER;
    GL(glGenBuffers(1, &NormalBufferInfo->DataBufferId));
    GL(glBindBuffer(NormalBufferInfo->Target,
                    NormalBufferInfo->DataBufferId));
    GL(glBufferData(GL_ARRAY_BUFFER,
                    Model->VertexBufferSize,
                    Model->Normals,
                    GL_STATIC_DRAW));
    NormalBufferInfo->VertexAttribInfo = STANDARD_NORMAL_VERTEX_ATTRIB_INFO;
    SetVertexAttribPointerInfo(NormalBufferInfo->VertexAttribInfo);
    
    buffer_info* UvBufferInfo = &Model->UVBufferInfo;
    UvBufferInfo->Target = GL_ARRAY_BUFFER;
    GL(glGenBuffers(1, &UvBufferInfo->DataBufferId));
    GL(glBindBuffer(UvBufferInfo->Target,
                    UvBufferInfo->DataBufferId));
    GL(glBufferData(GL_ARRAY_BUFFER,
                    Model->VertexBufferSize*2 / 3,
                    Model->UVs,
                    GL_STATIC_DRAW));
    UvBufferInfo->VertexAttribInfo = {
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        0,
        (GLvoid*)0,
    };
    SetVertexAttribPointerInfo(UvBufferInfo->VertexAttribInfo);
    
    GL(glGenBuffers(1, &Model->IndexBufferId));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                    Model->IndexBufferId));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(GLushort)*Model->IndexCount,
                    Model->Indices,
                    GL_STATIC_DRAW));
    
    GL(glBindVertexArray(0));
}

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
    texture_color_material *Material;
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

void DrawModel(GLuint VertexArrayId, int IndexCount)
{
    //TODO: Add capacity to draw unindexed values
    GL(glBindVertexArray(VertexArrayId));
    GL(glDrawElements(GL_TRIANGLES,
                      IndexCount,
                      GL_UNSIGNED_SHORT,
                      (void*)0));
    GL(glBindVertexArray(0));
}

model2 CreateQuad() {
    
    model2 Quad = {};
    Quad.VertexBufferSize = sizeof(QuadVertexData);
    Quad.Vertices = &QuadVertexData[0];
    
    Quad.Indices = &QuadIndexData[0];
    Quad.IndexCount = 6;
    
    LoadModelGpu(&Quad);
    
    GLuint CircleDataBuffer;
    GL(glBindVertexArray(Quad.VertexArrayId));
    GL(glGenBuffers(1, &CircleDataBuffer));
    GL(glBindBuffer(GL_ARRAY_BUFFER, CircleDataBuffer));
    GL(glBufferData(GL_ARRAY_BUFFER,
                    sizeof(QuadCircleData),
                    &QuadCircleData[0],
                    GL_STATIC_DRAW));
    GL(glEnableVertexAttribArray(1));
    GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    GL(glBindVertexArray(0));
    return Quad;
}

constexpr int SHADER_TYPE_COUNT = 5;
GLenum ShaderTypes[SHADER_TYPE_COUNT] {
    GL_VERTEX_SHADER,
    GL_FRAGMENT_SHADER,
    GL_GEOMETRY_SHADER,
    GL_TESS_CONTROL_SHADER,
    GL_TESS_EVALUATION_SHADER,
};

struct shader_program_sources
{
    union
    {
        struct
        {
            char* Vertex;
            char* Fragment;
            char* Geometry;
            char* TesselationControl;
            char* TesselationEvaluation;
        };
        char* SourceFilePaths[SHADER_TYPE_COUNT];
    };
};

GLuint CompileShader(char* shaderFilePath, char* shaderCode, GLenum shaderType)
{
    GLint result = GL_FALSE;
    int32 infoLogLength;
    
    GLuint shaderID = glCreateShader(shaderType);
    GL(glShaderSource(shaderID, 1, &shaderCode, 0));
    GL(glCompileShader(shaderID));
    
    GL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result));
    GL(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength));
    if (infoLogLength > 0)
    {
        DebugLog("Shader: %s\n%s\n", shaderFilePath, shaderCode);
        char* error = (char*)malloc(infoLogLength);
        GL(glGetShaderInfoLog(shaderID, infoLogLength, 0, error));
        DebugLog("%s error:\n", error);
        free(error);
    }
    
    return shaderID;
}

struct framebuffer_desc
{
    GLuint DepthBufferId;
    GLuint RenderTextureId;
    GLuint RenderFramebufferId;
    GLuint ResolveTextureId;
    GLuint ResolveFramebufferId;
};

struct framebuffer_object {
    uint32 Id;
    uint32 ColorBufferId;
    uint32 ColorBufferType;
    uint32 DepthStencilBufferId;
};

framebuffer_object CreateRenderTarget(GLuint ColorBufferType, GLuint InternalFormat, GLuint ExternalFormat, GLuint DepthStencilFormat, int Samples, int Width, int Height)
{
    framebuffer_object FBO;
    GL(glGenFramebuffers(1, &FBO.Id));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, FBO.Id));
    
    if (Samples <= 1)
    {
        if (ColorBufferType == GL_TEXTURE_2D)
        {
            FBO.ColorBufferType = GL_TEXTURE_2D;
            GL(glGenTextures(1, &FBO.ColorBufferId));
            GL(glBindTexture(FBO.ColorBufferType, FBO.ColorBufferId));
            GL(glTexParameteri(FBO.ColorBufferType, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GL(glTexParameteri(FBO.ColorBufferType, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL(glTexImage2D(FBO.ColorBufferType, 0, InternalFormat, Width, Height, 0, ExternalFormat, GL_UNSIGNED_BYTE, 0));
            GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FBO.ColorBufferType, FBO.ColorBufferId, 0));
        } 
        else if (ColorBufferType == GL_RENDERBUFFER) 
        {
            FBO.ColorBufferType = GL_RENDERBUFFER;
            GL(glGenRenderbuffers(1, &FBO.ColorBufferId));
            GL(glBindRenderbuffer(GL_RENDERBUFFER, FBO.ColorBufferId));
            GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, Width, Height));
            GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO.ColorBufferId));
            
        }
        
        GL(glGenRenderbuffers(1, &FBO.DepthStencilBufferId));
        GL(glBindRenderbuffer(GL_RENDERBUFFER, FBO.DepthStencilBufferId));
        GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height));
        GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, FBO.DepthStencilBufferId));
    }
    else
    {
        if (ColorBufferType == GL_TEXTURE_2D) {
            FBO.ColorBufferType = GL_TEXTURE_2D_MULTISAMPLE;
            GL(glGenTextures(1, &FBO.ColorBufferId));
            GL(glBindTexture(FBO.ColorBufferType, FBO.ColorBufferId));
            GL(glTexImage2DMultisample(FBO.ColorBufferType, Samples, InternalFormat, Width, Height, GL_TRUE));
            GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, FBO.ColorBufferType, FBO.ColorBufferId, 0));
        }
        else if (ColorBufferType == GL_RENDERBUFFER)
        {
            FBO.ColorBufferType = GL_RENDERBUFFER;
            GL(glGenRenderbuffers(1, &FBO.ColorBufferId));
            GL(glBindRenderbuffer(GL_RENDERBUFFER, FBO.ColorBufferId));GL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_RGB, Width, Height));
            GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, FBO.ColorBufferId));
        }
        
        GL(glGenRenderbuffers(1, &FBO.DepthStencilBufferId));
        GL(glBindRenderbuffer(GL_RENDERBUFFER, FBO.DepthStencilBufferId));
        GL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH24_STENCIL8, Width, Height));
        GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, FBO.DepthStencilBufferId));
    }
    PrintGlFBOError();
    
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    if (ColorBufferType == GL_RENDERBUFFER)
    {
        GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
    else 
    {
        GL(glBindTexture(FBO.ColorBufferType, 0));
    }
    GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
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
    GL(glBindFramebuffer(GL_FRAMEBUFFER, FBO.Id));
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
    GL(glGenBuffers(1, &VBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_vertices), screen_quad_vertices, GL_STATIC_DRAW));
    
    GL(glGenVertexArrays(1, &pp.VAO));
    GL(glBindVertexArray(pp.VAO));
    GL(glEnableVertexAttribArray(0));
    GL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (GLvoid*)0));
    
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL(glBindVertexArray(0));
    
    return pp;
}

void BeginPostprocessor(platform_data *Platform, postprocessor Postprocessor)
{
    BindRenderTarget(Postprocessor.RBOFBO);
    GL(glViewport(0, 0, Platform->WindowWidth, Platform->WindowHeight));
}

void EndPostprocessor(platform_data *Platform, postprocessor Postprocessor)
{
    GLuint ReadBuffer = Postprocessor.RBOFBO.Id;
    GLuint DrawBuffer = Postprocessor.TextureFBO.Id;
    GL(glBlitNamedFramebuffer(ReadBuffer, DrawBuffer,
                              0, 0, Platform->WindowWidth, Platform->WindowHeight, 
                              0, 0, Platform->WindowWidth, Platform->WindowHeight,
                              GL_COLOR_BUFFER_BIT, GL_LINEAR));
    
    const bool PostprocessorEnabled = false;
    if (PostprocessorEnabled)
    {
        // Post-process scene texture into front buffer.
        GL(glBindVertexArray(Postprocessor.VAO));
        GL(glUseProgram(Postprocessor.Program));
        GL(glActiveTexture(GL_TEXTURE0));
        GL(glBindTexture(GL_TEXTURE_2D, Postprocessor.TextureFBO.ColorBufferId));
        // Bind front buffer
        GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GL(glDrawArrays(GL_TRIANGLES, 0, 6));
        GL(glBindVertexArray(0));
    }
    else
    {
        GL(glBlitNamedFramebuffer(DrawBuffer, 0, 
                                  0, 0, Platform->WindowWidth, Platform->WindowHeight, 
                                  0, 0, Platform->WindowWidth, Platform->WindowHeight,
                                  GL_COLOR_BUFFER_BIT, GL_LINEAR));
    } 
}

GLuint LoadShaders(memory_arena* tempArena, shader_program_sources ShaderProgramSources) {
    GLErrorShow();
    GLuint programId = GL(glCreateProgram());
    GLuint shaderPrograms[SHADER_TYPE_COUNT];
    int shaderCount = 0;
    for(int i = 0; i < SHADER_TYPE_COUNT; i++) {
        if (ShaderProgramSources.SourceFilePaths[i] != 0)
        {
            int readResult;
            FILE* shaderFile = fopen(ShaderProgramSources.SourceFilePaths[i], "rb");
            fseek(shaderFile, 0L, SEEK_END);
            int32 shaderFileLength = ftell(shaderFile);
            rewind(shaderFile);
            int32 shaderStringLength = shaderFileLength+1;
            char* shaderCode = PushArray(tempArena, shaderStringLength, char);
            readResult = fread(shaderCode, 1, shaderFileLength, shaderFile);
            fclose(shaderFile);
            shaderCode[shaderFileLength] = '\0';
            shaderPrograms[shaderCount++] = CompileShader(ShaderProgramSources.SourceFilePaths[i], shaderCode, ShaderTypes[i]);
            PopArray(tempArena, shaderStringLength, char);
        }
    }
    
    for(int i = 0; i < shaderCount; i++)
    {
        GL(glAttachShader(programId, shaderPrograms[i]));
    }
    glLinkProgram(programId);
    for(int i = 0; i < shaderCount; i++)
    {
        GL(glDetachShader(programId, shaderPrograms[i]));
        GL(glDeleteShader(shaderPrograms[i]));
    }
    
    return programId;
}

GLuint LoadShaders(memory_arena* tempArena, char* vertexShaderFilePath, char* fragmentShaderFilePath) {
    shader_program_sources ShaderProgramSources = {0};
    ShaderProgramSources.Vertex = vertexShaderFilePath;
    ShaderProgramSources.Fragment = fragmentShaderFilePath;
    return LoadShaders(tempArena, ShaderProgramSources);
}

GLuint LoadShaders(memory_arena* tempArena, char* vertexShaderFilePath, char* fragmentShaderFilePath, char* geometryShaderFilePath) {
    shader_program_sources ShaderProgramSources = {0};
    ShaderProgramSources.Vertex = vertexShaderFilePath;
    ShaderProgramSources.Fragment = fragmentShaderFilePath;
    ShaderProgramSources.Geometry = geometryShaderFilePath;
    return LoadShaders(tempArena, ShaderProgramSources);
}

#define _GRAPHICS_CPP__
#endif
