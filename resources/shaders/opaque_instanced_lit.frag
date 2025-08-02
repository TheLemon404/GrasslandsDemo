#version 330 core

layout (location = 0) in vec3 pPosition;
layout (location = 1) in vec3 pNormal;
layout (location = 2) in vec2 pUV;

uniform vec3 albedo;
uniform float specular;
uniform sampler2D baseTexture;
uniform int hasBaseTexture;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 shadowColor;
uniform float blurDistance;
uniform vec3 cameraPosition;

layout (location = 0) out vec4 fragColor;

void main()
{
    vec4 color = vec4(1.0);

    if(hasBaseTexture == 1) {
        color = vec4(albedo, 1.0f) * texture(baseTexture, pUV);
    }
    else {
        color = vec4(albedo, 1.0f);
    }

    float depth = distance(cameraPosition, pPosition);

    vec3 diffuse = max(dot(pNormal, normalize(-sunDirection)), 0.0f) * sunColor;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), pNormal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 finalSpecular = specular * spec * sunColor;

    vec3 lighting = (shadowColor + diffuse + finalSpecular) * color.rgb;
    vec4 final = vec4(lighting, 1.0f);
    fragColor = final;
}