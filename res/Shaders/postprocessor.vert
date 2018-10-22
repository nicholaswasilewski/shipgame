#version 330 core

layout (location = 0) in vec4 vertex; //pos, uv

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
	TexCoords = vertex.zw;
}