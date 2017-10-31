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

#define _GRAPHICS_CPP__
#endif
