# version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D depth_map;

void main() {
    float depthValue = texture(depth_map, uv).r;
    FragColor = vec4(depthValue, depthValue, depthValue, 1.0);
}