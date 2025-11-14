#version 410 core

#include <material_data.glsl>

layout(location = 0) in vec3 pPosition;
layout(location = 1) in vec3 pNormal;
layout(location = 2) in vec2 pUV;
layout(location = 3) in vec4 fragPosLightSpace;
layout(location = 4) in float opacity;

uniform vec3 sunDirection;
uniform vec3 warmSun;
uniform vec3 coolSky;
uniform vec3 cameraPosition;
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

    float diffuseStrength = max(dot(pNormal, normalize(-sunDirection)), 0.0);
    vec3 diffuse = mix(coolSky * material.shadowColor, warmSun, diffuseStrength) * 0.9;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), pNormal);

    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
    vec3 finalSpecular = (1.0 - material.roughness) * spec * warmSun;

    float shadow = shadowCalculation(pNormal);

    vec3 ambient = mix(coolSky * 0.6, warmSun * 0.4, 0.4) * 0.45;
    vec3 lighting = mix(material.shadowColor * coolSky, color.rgb * warmSun, (1.0 - shadow)) * (diffuse + finalSpecular + ambient);
    vec4 final = vec4(lighting, opacity);
    fragColor = final;
    gl_FragDepth = gl_FragCoord.z;
}
