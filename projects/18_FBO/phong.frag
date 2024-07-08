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

void main() {
    vec3 albedoMap = vec3(texture(material.diffuse, uv));
    vec3 specularMap = vec3(texture(material.specular, uv));

    vec3 norm = normalize(normal); 
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 finalColor = calcDirectionalLight(dirLight, norm, viewDir, albedoMap, specularMap);

    FragColor = vec4(finalColor, 1.0);
}