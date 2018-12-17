#version 330 core

in vec2 TexCoords;
out vec3 Color;

uniform sampler2D TextureSampler;

void main()
{
	vec3 TexColor = texture(TextureSampler, TexCoords).rgb;
	float average = (TexColor.r + TexColor.g + TexColor.b)/3.0;
	Color = vec3(average);

//	Color = TexColor;
}