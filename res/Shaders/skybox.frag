#version 330 core

uniform samplerCube SkyBox;

in vec3 UV;

out vec4 Color;

void main()
{    
	Color = vec4(UV.y, UV.y, UV.y, 1.0);
//    Color = texture(SkyBox, UV);
}