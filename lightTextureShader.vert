#version 330 core

//Model
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

//Light
uniform vec3 LightPosition;

layout(location = 0) in vec3 vertexPosition_model;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec2 UV;
out vec3 Position_world;
out vec3 Normal_camera;
out vec3 LightDirection_camera;
out vec3 EyeDirection_camera;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_model,1);
    Position_world = (M*vec4(vertexPosition_model, 1)).xyz;


    vec3 VertexPosition_camera = (V * M * vec4(vertexPosition_model, 1)).xyz;
    EyeDirection_camera = vec3(0,0,0) - VertexPosition_camera;
    
    vec3 LightPosition_camera = ( V*vec4(LightPosition, 1)).xyz;
    LightDirection_camera = LightPosition_camera + EyeDirection_camera;

    Normal_camera = (V * M * vec4(vertexNormal, 0)).xyz;
    
    UV = vertexUV;
}
