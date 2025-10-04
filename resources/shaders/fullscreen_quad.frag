#version 330 core

layout (location = 0) in vec2 pUV;

uniform sampler2D baseTexture;
uniform int hasBaseTexture;

layout (location = 0) out vec4 fragColor;

void main() {
    vec4 color = vec4(1.0);

    if(hasBaseTexture == 1) {
        color = texture(baseTexture, pUV);
    }

    fragColor = color;
}