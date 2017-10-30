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

uniform sampler2D NormalMap;
uniform sampler2D ReflectionMap;
uniform float UVOffset;

in vec2 UV;
in vec3 FragPos;
in vec3 FragNormal;
in vec4 ClipSpace;

out vec3 Color;

void main()
{    
    // calculate normals
    vec3 fragmentNormal = texture(NormalMap, UV+UVOffset).xyz;
    float normalPower = 2.0f;
    fragmentNormal = vec3( (fragmentNormal.r-0.5f)*2, fragmentNormal.b*normalPower, (fragmentNormal.g-0.5f)*2);
    //fragmentNormal = vec3(1, 1, 0);

    // colors
    vec3 AmbientColor = Light.Ambient*Material.Diffuse;
    vec3 DiffuseColor = Light.Diffuse*Material.Diffuse;
    vec3 SpecularColor = Light.Specular*Material.Specular;

    // point-light values
    vec3 lightPosition = vec3(Light.Position);
    float lightDistance = length(lightPosition - FragPos);
    float lightDistanceSquared = lightDistance*lightDistance;
    float attenuation = Light.Power / (1.0 + (0.09 * lightDistance) + (0.032 * lightDistanceSquared));
    attenuation = Light.Power / lightDistanceSquared;

    // ambient
    vec3 ambient = AmbientColor * attenuation;

    // diffuse
    vec3 normal = normalize(fragmentNormal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diffuseCoefficient = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diffuseCoefficient * DiffuseColor * attenuation;

    // specular
    vec3 lightReflect = normalize(reflect(-lightDir, normal));
    vec3 cameraDir = normalize(CameraPosition - FragPos);
    float specCoefficient = pow(max(dot(lightReflect, cameraDir), 0.0f), Material.Shine);
    vec3 specular = specCoefficient * SpecularColor; // no damping by distance

    // light color
    vec3 resultColor = ambient + diffuse + specular;

    // reflection
    vec2 normalizedDeviceSpace = (ClipSpace.xy/ClipSpace.w)/2.0f + 0.5f;
    normalizedDeviceSpace.y = -normalizedDeviceSpace.y;
    vec3 reflectColor = texture(ReflectionMap, normalizedDeviceSpace).rgb;

    Color = mix(resultColor, reflectColor, 0.3f);
    //Color = texture(NormalMap, UV+UVOffset).rgb;
    //Color = texture(ReflectionMap, UV).rgb;
}