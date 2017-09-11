#version 330 core

struct light {
    float Power;
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform light Light;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 FragPos;
out vec3 FragNormal;

void main()
{
    gl_Position = MVP * vec4(vertexPosition, 1);
    FragPos = vec3(M*vec4(vertexPosition, 1));
    FragNormal = mat3(M) * vertexNormal;
}
