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

in vec2 UV;
in vec3 FragPos;
in vec3 FragNormal;

out vec4 FragColor;

void main()
{
    /*
    float x = FragPos.x;
    float y = 0;
    if(x - floor(x) > 0.5f)
    {
        y = 0.5f;
    }
    FragColor = vec4(0.0f, y, 1.0f, 1.0f);
    */

    vec3 objectColor = vec3(0.3f, 0.7f, 1.0f);
    vec3 lightColor =  vec3(1.0f, 1.0f, 1.0f);

    vec3 lightPosition = vec3(Light.Position);

    float ambientPower = 0.1f;
    vec3 ambient = ambientPower * lightColor;

    float diffusePower = 1f;
    vec3 normal = normalize(FragNormal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diffuseCoefficient = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = diffuseCoefficient * lightColor * diffusePower;

    float specularPower = 5f;
    vec3 lightReflect = normalize(reflect(-lightDir, normal));
    vec3 cameraDir = normalize(CameraPosition - FragPos);
    float specCoefficient = pow(max(dot(lightReflect, cameraDir), 0.0f), 256);
    vec3 specular = specCoefficient * lightColor * specularPower;

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}