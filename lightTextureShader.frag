#version 330 core

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float LightPower;

uniform sampler2D TextureSampler;
uniform vec3 AmbientCoefficient = vec3(0.1,0.1,0.1);
uniform vec3 SpecularColor = vec3(0.3,0.3,0.3);

in vec2 UV;
in vec3 Position_world;
in vec3 Normal_camera;
in vec3 LightDirection_camera;
in vec3 EyeDirection_camera;

out vec3 Color;

void main()
{

    vec3 DiffuseColor = texture(TextureSampler, vec2(UV.x, UV.y)).rgb;
    vec3 AmbientColor = AmbientCoefficient*DiffuseColor;

    float LightDistance = length(LightPosition - Position_world);
    float LightDistanceSquared = LightDistance*LightDistance;

    vec3 N = normalize(Normal_camera);
    vec3 L = normalize(LightDirection_camera);
    float LightAngle = clamp(dot(N,L),0,1);
    
    //Color = texture(TextureSampler, vec2(UV.x, UV.y)).rgb;
    Color = AmbientColor +
	DiffuseColor * LightColor * LightPower * LightAngle / LightDistanceSquared;
    //Specularcolor * LightColor * LightPower * pow(EyeLightAngle, 5) / LightDistanceSquared;
}
