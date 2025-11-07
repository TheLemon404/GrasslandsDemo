#version 410 core

#include <material_data.glsl>

layout(location = 0) in vec3 pPosition;
layout(location = 1) in vec3 pNormal;
layout(location = 2) in vec2 pUV;
layout(location = 3) in vec4 fragPosLightSpace;
layout(location = 4) in vec2 terrainSpaceUV;

uniform vec3 lowerColor;
uniform vec3 upperColor;

uniform vec3 lowerColor2;
uniform vec3 upperColor2;

uniform MaterialData material;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 cameraPosition;
uniform int receivesShadow;
uniform sampler2D shadowMap;

layout(location = 0) out vec4 fragColor;

#include <lighting_calculations.glsl>

void main() {
    vec4 color = vec4(1.0);
    vec4 color2 = vec4(1.0);

    if (material.hasBaseTexture == 1) {
        color = texture(material.baseTexture, pUV);
    }
    else {
        color = vec4(mix(lowerColor, upperColor, pUV.y), 1.0);
    }

    if (material.hasBaseTexture == 1) {
        color2 = texture(material.baseTexture, pUV);
    }
    else {
        color2 = vec4(mix(lowerColor2, upperColor2, pUV.y), 1.0);
    }

    vec3 warmSun = vec3(1.05, 0.95, 0.82); // subtle warm tint
    vec3 coolSky = vec3(0.60, 0.75, 0.90); // slightly more saturated blue

    float depth = distance(cameraPosition, pPosition);

    float diffuseStrength = 0.5 + 0.5 * max(dot(pNormal, normalize(-sunDirection)), 0.0);
    vec3 diffuse = mix(coolSky, warmSun, diffuseStrength) * 0.9;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), pNormal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 8.0);
    vec3 finalSpecular = warmSun * 0.25;

    float shadow = 0.0;
    if (receivesShadow != 0) {
        shadow = shadowCalculation(pNormal);
    }
    shadow = pow(shadow, 0.6);

    float s = sin(terrainSpaceUV.x * 20.0) * cos(terrainSpaceUV.y * 20.0);
    vec3 factoredColor = mix(color.rgb, color2.rgb, s);

    vec3 ambient = mix(coolSky * 0.6, warmSun * 0.4, 0.4);
    vec3 lighting = mix(material.shadowColor, factoredColor.rgb, (1.0 - shadow)) * diffuse + ambient * 0.45;
    vec4 final = vec4(lighting, 1.0f);
    if (material.hasBaseTexture == 1) {
        fragColor = final * color.a;
    }
    else {
        fragColor = final * mix(0.7, 1.0, smoothstep(0.0, 0.3, pUV.y));
    }
    gl_FragDepth = gl_FragCoord.z;
}
