#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMat;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec2 uv;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;

void main() {
    uv = aUV;
    vec3 fragPos = vec3(model * vec4(aPos, 1.0));

    vec3 T = normalize(normalMat * aTangent);
    vec3 N = normalize(normalMat * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));

    tangentLightPos = TBN * lightPos;
    tangentViewPos = TBN * viewPos;
    tangentFragPos = TBN * fragPos;

    gl_Position = proj * view * model * vec4(aPos, 1.0);
}
