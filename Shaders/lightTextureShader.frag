#version 330 core

struct material {
    sampler2D Diffuse;
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
uniform vec3 CameraPosition;

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;

out vec3 Color;

void main()
{
    vec3 AmbientColor = Light.Ambient*texture(Material.Diffuse, UV).rgb;
    vec3 DiffuseColor = Light.Diffuse*texture(Material.Diffuse, UV).rgb;
    vec3 SpecularColor = Light.Specular*Material.Specular;

    float LightDistance = length(Light.Position - FragPos);
    float LightDistanceSquared = LightDistance*LightDistance;

    vec3 N = normalize(Normal);
    vec3 L = normalize(Light.Position - FragPos);
    float LightAngle = max(dot(N,L),0.0);
    
    vec3 E = normalize(CameraPosition - FragPos);
    vec3 R = reflect(-L, N);
    float Spec = pow(max(dot(E,R),0.0), Material.Shine);
    
    Color = AmbientColor +
	DiffuseColor*Light.Power*LightAngle / LightDistanceSquared + 
	SpecularColor*Light.Power*Spec / LightDistanceSquared;
}
