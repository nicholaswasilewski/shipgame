#version 330 core

struct light {
    float Power;
    vec4 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct color_material {
    vec3 Diffuse;
    vec3 Specular;
    vec3 Emissive;
    float Shine;
};

uniform light Light;
uniform color_material Material;
uniform vec3 CameraPosition;

in vec3 FragPos;
in vec3 FragNormal;

out vec3 Color;

void main()
{
    vec3 LightPosition = Light.Position.xyz;
    vec3 AmbientColor = Light.Ambient*Material.Diffuse;
    vec3 DiffuseColor = Light.Diffuse*Material.Diffuse;
    vec3 SpecularColor = Light.Specular*Material.Specular;

    float LightDistance = length(LightPosition - FragPos);
    float LightDistanceSquared = LightDistance*LightDistance;

    vec3 N = normalize(FragNormal);
    vec3 L = normalize(LightPosition - FragPos);
    float LightAngle = max(dot(N,L), 0.0);

    vec3 E = normalize(CameraPosition - FragPos);
    vec3 R = reflect(-L, N);
    float EyeLightAngle = max(dot(E, R), 0.0);

	Color =
	AmbientColor +
	DiffuseColor*Light.Power*LightAngle/ LightDistanceSquared +
	SpecularColor*Light.Power*pow(EyeLightAngle, Material.Shine) / LightDistanceSquared +
	Material.Emissive;
}
