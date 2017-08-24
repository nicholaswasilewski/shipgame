#version 330 core

struct material {
    sampler2D Diffuse;
    sampler2D Specular;
    sampler2D Emissive;
    float Shine;
};

struct light {
    float Power;
    vec4 Position;
    
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
    vec3 LightPosition = vec3(Light.Position);
    float LightDistance = length(LightPosition - FragPos);
    float LightDistanceSquared = LightDistance*LightDistance;

    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPosition - FragPos);
    float Diff = max(dot(N,L),0.0);
    
    vec3 E = normalize(CameraPosition - FragPos);
    vec3 R = reflect(-L, N);
    float Spec = pow(max(dot(E,R),0.0), Material.Shine);
 
    vec3 AmbientColor = Light.Ambient*texture(Material.Diffuse, UV).rgb;
    vec3 DiffuseColor = Light.Diffuse*Diff*texture(Material.Diffuse, UV).rgb;
    vec3 SpecularColor = Light.Specular*Spec*texture(Material.Specular, UV).rgb;
    vec3 EmissiveColor = texture(Material.Emissive, UV).rgb;

    Color = AmbientColor +
	DiffuseColor*Light.Power / LightDistanceSquared + 
	SpecularColor*Light.Power / LightDistanceSquared +
	EmissiveColor;
}
