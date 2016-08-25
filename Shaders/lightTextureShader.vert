#version 330 core

struct light {
    float Power;
    vec3 Position;
    
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform light Light;

//Model
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 vertexPosition_model;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;
out vec3 FragPos;
out vec3 Normal;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_model,1);
    FragPos = vec3(M*vec4(vertexPosition_model, 1));
    Normal = mat3(transpose(inverse(M))) * vertexNormal;
    UV = vertexUV;
}
