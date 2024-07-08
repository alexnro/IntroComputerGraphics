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

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

void main() {
    vec3 lightDir = light.position - fragPos;
    vec3 lightDirNorm = normalize(lightDir);

    float dist = length(lightDir);
    float attenuation = 1.0 / ((light.constant + light.linear * dist + light.quadratic * (dist * dist)) * 0.5);

    vec3 albedo = vec3(texture(material.diffuse, uv));
    vec3 ambient = albedo * light.ambient;
    
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * albedo * light.diffuse;
    
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDirNorm + viewDir);
    float spec =  pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * vec3(texture(material.specular, uv)) * light.specular;

    vec3 phong = (ambient + diffuse + specular) * attenuation;
    FragColor = vec4(phong, 1.0);
}