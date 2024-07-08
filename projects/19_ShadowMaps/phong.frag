# version 330 core
out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D depth_map;

in vec3 normal;
in vec3 fragPos;
in vec2 uv;
in vec4 fragPosLightSpace;

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

    vec3 position;
};
uniform DirLight dirLight;

vec3 calcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedoMap, vec3 specularMap, float shadow) {
    vec3 ambient = albedoMap * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedoMap * light.diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * specularMap * light.specular;

    return ambient + ((1.0 - shadow) * (diffuse + specular));
}

float ShadowCalculation(vec4 fragPosLightSpace, float bias) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depth_map, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    //float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depth_map, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depth_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main() {
    vec3 albedoMap = vec3(texture(material.diffuse, uv));
    vec3 specularMap = vec3(texture(material.specular, uv));

    vec3 norm = normalize(normal); 
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 lightDir = normalize(dirLight.position - fragPos);

    float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
    float shadow = ShadowCalculation(fragPosLightSpace, bias);
    vec3 finalColor = calcDirectionalLight(dirLight, norm, viewDir, albedoMap, specularMap, shadow);

    FragColor = vec4(finalColor, 1.0);
}