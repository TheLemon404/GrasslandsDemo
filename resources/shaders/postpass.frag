#version 410 core

layout (location = 0) in vec2 pUV;

uniform sampler2D debugTexture;

out vec4 fragColor;

void main() {
    float depthValue = texture(debugTexture, pUV).r;
    fragColor = vec4(vec3(depthValue), 1.0);
}