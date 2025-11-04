#version 410 core

#include <material_data.glsl>

layout(location = 0) in vec3 pPosition;
layout(location = 1) in vec3 pNormal;
layout(location = 2) in vec2 pUV;
layout(location = 3) in vec4 fragPosLightSpace;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 cameraPosition;
uniform int receivesShadow;
uniform sampler2D shadowMap;

uniform MaterialData material;

layout(location = 0) out vec4 fragColor;

#include <lighting_calculations.glsl>

void main() {
    vec4 color = vec4(1.0);

    if (material.hasBaseTexture == 1) {
        color = vec4(material.albedo, 1.0f) * texture(material.baseTexture, pUV);
    }
    else {
        color = vec4(material.albedo, 1.0f);
    }

    float depth = distance(cameraPosition, pPosition);

    vec3 diffuse = max(dot(pNormal, normalize(-sunDirection)), 0.0f) * sunColor;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), pNormal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0f), 32);

    vec3 finalSpecular = (1.0 - material.roughness) * spec * sunColor;

    float shadow = 0.0f;
    if (receivesShadow != 0) {
        shadow = shadowCalculation(pNormal);
    }

    vec3 lighting = mix(material.shadowColor, color.rgb, (1.0 - shadow) * (diffuse + finalSpecular));
    fragColor = vec4(lighting, 1.0f);

    gl_FragDepth = gl_FragCoord.z;
}
