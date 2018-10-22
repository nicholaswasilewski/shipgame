#ifndef _GRAPHICS_CPP__

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
    
    GLuint NormalMap;
    GLuint NormalMapHandle;
    
    GLuint DuDvMap;
    GLuint DuDvMapHandle;
    
    float UVOffset;
    GLuint UVOffsetHandle;
    
    GLuint ReflectionMap;
    GLuint ReflectionHandle;
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
        //printf("%s\n", error);
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

void BindRenderTarget(framebuffer_object FBO)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO.Id);
}

void BindModel(model *Model)
{
    glGenVertexArrays(1, &Model->VertexArrayID);
    glBindVertexArray(Model->VertexArrayID);
    glGenBuffers(1, &Model->VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 Model->VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 Model->VertexBufferSize,
                 Model->Vertices,
                 GL_STATIC_DRAW);
    
    glGenBuffers(1, &Model->NormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,
                 Model->NormalBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 Model->VertexBufferSize,
                 Model->Normals,
                 GL_STATIC_DRAW);
    glGenBuffers(1, &Model->IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 Model->IndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLushort)*Model->IndexCount,
                 Model->Indices,
                 GL_STATIC_DRAW);
    /*
    for(int a = 0; a < Model->IndexCount; a+=3)
    {
    printf("index %i: %i, %i, %i\n",a/3,Model->Indices[a],Model->Indices[a+1],Model->Indices[a+2]);
    }
    
    for(int a = 0; a < Model->VertexBufferSize/sizeof(float); a+=3)
    {
    printf("vertex %i: %f, %f, %f\n",a/3,Model->Vertices[a],Model->Vertices[a+1],Model->Vertices[a+2]);
    }
    
    for(int a = 0; a < Model->VertexBufferSize/sizeof(float); a+=3)
    {
    printf("normal %i: %f, %f, %f\n",a/3,Model->Normals[a],Model->Normals[a+1],Model->Normals[a+2]);
    }
    */
}

#define _GRAPHICS_CPP__
#endif
