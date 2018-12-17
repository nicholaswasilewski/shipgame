#version 330 core

uniform mat4 MVP ;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec2 CircleValue;

out vec2 CircleVal;

void main() {
	gl_Position = MVP * vec4(VertexPosition, 1.0f);
	CircleVal = CircleValue;
}