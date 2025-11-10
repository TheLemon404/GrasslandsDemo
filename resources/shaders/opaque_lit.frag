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

    vec3 warmSun = vec3(1.01, 0.95, 0.9); // subtle warm tint
    vec3 coolSky = vec3(0.60, 0.75, 0.90); // slightly more saturated blue

    float depth = distance(cameraPosition, pPosition);

    float diffuseStrength = max(dot(pNormal, normalize(-sunDirection)), 0.0);
    vec3 diffuse = mix(coolSky * material.shadowColor, warmSun, diffuseStrength) * 0.9;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), pNormal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 finalSpecular = (1.0 - material.roughness) * spec * warmSun;

    float shadow = 0.0;
    if (receivesShadow != 0) {
        shadow = shadowCalculation(pNormal) * diffuseStrength;
    }
    shadow = pow(shadow, 0.6);

    vec3 ambient = mix(coolSky * 0.6, warmSun * 0.4, 0.4) * 0.45;
    vec3 lighting = mix(material.shadowColor * coolSky, color.rgb * warmSun, (1.0 - shadow)) * (diffuse + finalSpecular + ambient);
    fragColor = vec4(lighting, 1.0f);

    gl_FragDepth = gl_FragCoord.z;
}
