# version 330 core
out vec4 FragColor;

uniform vec3 viewPos;

in vec3 normal;
in vec3 fragPos;
in vec2 uv;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    int shininess;
};
uniform Material material;

struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
};
uniform DirLight dirLight;

struct PointLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;

    float constant;
    float linear;
    float quadratic;
};
#define NUMBER_POINT_LIGHTS 2
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};
#define NUMBER_SPOT_LIGHTS 2
uniform SpotLight spotLights[NUMBER_SPOT_LIGHTS];

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedoMap, vec3 specularMap) {
    vec3 ambient = albedoMap * light.ambient;

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedoMap * light.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * specularMap * light.specular;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedoMap, vec3 specularMap) {
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                               light.linear * dist +
                               light.quadratic * (dist * dist));


    vec3 ambient = albedoMap * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedoMap * light.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * specularMap * light.specular;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec3 albedoMap, vec3 specularMap) {
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                               light.linear * dist +
                               light.quadratic * (dist * dist));


    vec3 ambient = albedoMap * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedoMap * light.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * specularMap * light.specular;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    return (ambient + (diffuse * intensity) + (specular* intensity)) * attenuation;
}

void main() {
    vec3 albedoMap = vec3(texture(material.diffuse, uv));
    vec3 specularMap = vec3(texture(material.specular, uv));

    vec3 norm = normalize(normal); 
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 finalColor = calcDirectionalLight(dirLight, norm, viewDir, albedoMap, specularMap);

    for (int i = 0; i < NUMBER_POINT_LIGHTS; ++i) {
      finalColor += calcPointLight(pointLights[i], norm, viewDir, fragPos, albedoMap, specularMap);
    }

    for (int i = 0; i < NUMBER_SPOT_LIGHTS; ++i) {
      finalColor += calcSpotLight(spotLights[i], norm, viewDir, fragPos, albedoMap, specularMap);
    }

    FragColor = vec4(finalColor, 1.0);
}