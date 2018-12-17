#version 330 core

uniform samplerCube SkyBox;

in vec3 UV;

out vec4 Color;

void main()
{
    Color = texture(SkyBox, UV);
}