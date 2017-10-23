#version 330 core

struct light {
    float Power;
    vec4 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct color_material {
    vec3 Diffuse;
    vec3 Specular;
    vec3 Emissive;
    float Shine;
};

uniform light Light;
uniform color_material Material;
uniform vec3 CameraPosition;

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    float x = FragPos.x;
    float y = 0;
    if(x - floor(x) > 0.5f)
    {
        y = 0.5f;
    }
    FragColor = vec4(0.0f, y, 1.0f, 1.0f);
}