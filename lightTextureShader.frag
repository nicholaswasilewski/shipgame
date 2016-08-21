#version 330 core

struct material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    float Shine;
};

struct light {
    float Power;
    vec3 Position;
    
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform light Light;
uniform material Material;
uniform sampler2D TextureSampler;

uniform mat4 MV;

in vec2 UV;
in vec3 Position_world;
in vec3 Normal_camera;
in vec3 LightDirection_camera;
in vec3 EyeDirection_camera;

out vec3 Color;

void main()
{
    vec3 DiffuseColor = texture(TextureSampler, vec2(UV.x, UV.y)).rgb;
    vec3 AmbientColor = Light.Ambient*DiffuseColor;
    vec3 MaterialSpecularColor = Light.Specular;

    float LightDistance = length(Light.Position - Position_world);
    float LightDistanceSquared = LightDistance*LightDistance;

    vec3 N = normalize(Normal_camera);
    vec3 L = normalize(LightDirection_camera);
    float LightAngle = clamp(dot(N,L),0,1);

    vec3 E = normalize(EyeDirection_camera);
    vec3 R = reflect(-L, N);
    float EyeLightAngle = clamp(dot(E,R),0,1);
    
    Color = AmbientColor +
	DiffuseColor * Light.Diffuse * Light.Power * LightAngle / LightDistanceSquared + 
	Material.Specular * Light.Specular * Light.Power * pow(EyeLightAngle, Material.Shine) / LightDistanceSquared;
}
