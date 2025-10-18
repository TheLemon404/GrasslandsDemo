#version 410 core

#include <material_data.glsl>

layout (location = 0) in vec3 pPosition;
layout (location = 1) in vec3 pNormal;
layout (location = 2) in vec2 pUV;
layout (location = 3) in vec4 fragPosLightSpace;
layout (location = 4) in vec2 terrainSpaceUV;

uniform vec3 lowerColor;
uniform vec3 upperColor;

uniform vec3 lowerColor2;
uniform vec3 upperColor2;

uniform MaterialData material;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 shadowColor;
uniform float blurDistance;
uniform vec3 cameraPosition;
uniform int receivesShadow;
uniform sampler2D shadowMap;

layout (location = 0) out vec4 fragColor;

#include <lighting_calculations.glsl>

void main() {
    vec4 color = vec4(1.0);
    vec4 color2 = vec4(1.0);

    if(material.hasBaseTexture == 1){
        color = texture(material.baseTexture, pUV);
    }
    else{
        color = vec4(mix(lowerColor, upperColor, pUV.y), 1.0);
    }

    if(material.hasBaseTexture == 1){
        color2 = texture(material.baseTexture, pUV);
    }
    else{
        color2 = vec4(mix(lowerColor2, upperColor2, pUV.y), 1.0);
    }

    float depth = distance(cameraPosition, pPosition);

    vec3 diffuse = max(dot(pNormal, normalize(-sunDirection)), 0.8f) * sunColor;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), pNormal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.8f), 32);
    vec3 finalSpecular = (1.0 - material.roughness) * spec * sunColor;

    float shadow = 0.0f;
    if(receivesShadow != 0) {
        shadow = shadowCalculation(pNormal);
    }

    float s = sin(terrainSpaceUV.x * 10.0) * cos(terrainSpaceUV.y * 10.0);
    vec3 factoredColor = mix(color.rgb, color2.rgb, s);
    vec3 lighting = (shadowColor + (1.0 - shadow)) * (diffuse + finalSpecular) * factoredColor;
    vec4 final = vec4(lighting, 1.0f);
    if(material.hasBaseTexture == 1){
        fragColor = final * color.a;
    }
    else {
        fragColor = final * mix(0.7, 1.0, smoothstep(0.0, 0.3, pUV.y));
    }
    gl_FragDepth = gl_FragCoord.z;
}