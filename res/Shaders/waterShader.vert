#version 330 core

struct light {
    float Power;
    vec4 Position;
    
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform light Light;

//Model
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

// input
layout(location = 0) in vec3 vertexPosition;

// output
out vec2 UV;
out vec3 FragPos;
out vec3 FragNormal;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

void main()
{
    // float x = vertexPosition.x;
    // float y = 0;
    // if(x - floor(x) > 0.5f)
    // {
    //     y = 0.5f;
    // }
    // gl_Position = MVP * vec4(vertexPosition.x, y, vertexPosition.z, 1.0f);
    // Normal = mat3(M) * vec3(0.0f, 1.0f, 0.0f);

    
    gl_Position = MVP * vec4(vertexPosition, 1);
    FragPos = vec3(M*vec4(vertexPosition, 1));

    float power = 0.05f;
    float x = power * rand(vertexPosition.xz);
    float z = power * rand(vertexPosition.zx);
    FragNormal = vec3(x, 1.0f, z);
}