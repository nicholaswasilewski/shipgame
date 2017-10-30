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
uniform sampler2D DuDvMap;
uniform sampler2D ReflectionMap;
uniform float UVOffset;

in vec2 UV;
in vec3 FragPos;
in vec3 FragNormal;
in vec4 ClipSpace;

out vec3 Color;

void main()
{    
    // ******************
    // reflection + DuDv

    // DuDv sampling, to add ripples to the reflection
    float distortionPower = 0.05f;
    vec2 distortedUVs = texture(DuDvMap, vec2(UV.x + UVOffset, UV.y)).rg * 0.1f;
    distortedUVs = UV + vec2(distortedUVs.x, distortedUVs.y + UVOffset);
    vec2 dudvDistortion = (texture(DuDvMap, distortedUVs).rg * 2.0f - 1.0f) * distortionPower;

    // reflection
    vec2 normalizedDeviceSpace = (ClipSpace.xy/ClipSpace.w)/2.0f + 0.5f;
    vec2 reflectUVs = vec2(normalizedDeviceSpace.x, -normalizedDeviceSpace.y);
    reflectUVs += dudvDistortion;
    reflectUVs.x = clamp(reflectUVs.x, 0.001f, 0.999f);
    reflectUVs.y = clamp(reflectUVs.y, -0.999f, -0.001f);
    vec3 reflectColor = texture(ReflectionMap, reflectUVs).rgb;

    // ******************
    // Phong

    // calculate normals, should be distorted similarly to match DuDv distortion.
    vec3 fragmentNormal = texture(NormalMap, distortedUVs).xyz;
    float normalPower = 2.0f;
    fragmentNormal = vec3(fragmentNormal.r * 2.0f - 1.0f, fragmentNormal.b * normalPower, fragmentNormal.g * 2.0f - 1.0f);
    vec3 normal = normalize(fragmentNormal);

    // colors
    vec3 AmbientColor = Light.Ambient*Material.Diffuse;
    vec3 DiffuseColor = Light.Diffuse*Material.Diffuse;
    vec3 SpecularColor = Light.Specular*Material.Specular;

    // point-light values
    vec3 lightPosition = vec3(Light.Position);
    float lightDistance = length(lightPosition - FragPos);
    float lightDistanceSquared = lightDistance*lightDistance;
    float attenuation = Light.Power / (1.0f + (0.09f * lightDistance) + (0.032f * lightDistanceSquared));
    attenuation = Light.Power / lightDistanceSquared;

    // ambient
    vec3 ambient = AmbientColor * attenuation;

    // diffuse
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diffuseCoefficient = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diffuseCoefficient * DiffuseColor * attenuation;

    // specular
    vec3 lightReflect = normalize(reflect(-lightDir, normal));
    vec3 cameraDir = normalize(CameraPosition - FragPos);
    float specCoefficient = pow(max(dot(lightReflect, cameraDir), 0.0f), Material.Shine);
    vec3 specular = specCoefficient * SpecularColor; // no damping by distance

    // phong color w/ distorted normals
    vec3 phongColor = ambient + diffuse + specular;

    Color = mix(phongColor, reflectColor, 0.5f);
    //Color = reflectColor;
    //Color = texture(DuDvMap, UV+UVOffset).rgb;
    //Color = texture(ReflectionMap, UV).rgb;
}