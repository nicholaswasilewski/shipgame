#version 330 core

//Model
uniform mat4 MVP;
uniform mat4 V;

// input
layout(location = 0) in vec3 vertexPosition;

// output
out vec3 UV;

void main()
{
    UV = vertexPosition;
    
    gl_Position = MVP * vec4(vertexPosition, 1.0);
    // trick it into thinking z is now max distance (1.0)
    gl_Position = gl_Position.xyww;
}