#version 330 core

layout (location = 0) in vec2 pUV;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(positionTexture, pUV);
}