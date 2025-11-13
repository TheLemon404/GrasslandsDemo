#version 430 core

layout(location = 0) in vec3 pUV;

layout(location = 0) out vec4 fragColor;

uniform samplerCube skybox;

void main() {
    fragColor = texture(skybox, pUV);
}
