#version 330 core

in vec2 UV;
out vec3 Color;

uniform sampler2D TextureSampler;

void main()
{
    Color = texture(TextureSampler, vec2(UV.x, UV.y)).rgb;
}
