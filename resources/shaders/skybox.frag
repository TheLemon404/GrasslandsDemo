#version 430 core

layout(location = 0) in vec3 pUV;

layout(location = 0) out vec4 fragColor;

uniform vec3 groundColor;
uniform vec3 horizonColor;
uniform vec3 skyColor;

void main() {
    vec3 nUV = normalize(pUV);
    float lerpFactor = (nUV.y + 1.0f) / 2.0f;
    vec3 skyLerp = mix(horizonColor, skyColor, (lerpFactor / 2.0f) + 0.5f);
    vec3 completeLerp = mix(groundColor, skyLerp, lerpFactor);
    fragColor = vec4(completeLerp, 1.0f);
}
