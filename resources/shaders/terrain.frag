#version 410 core

layout (location = 0) in vec3 pPosition;
layout (location = 1) in vec3 fragWorldPos;
layout (location = 2) in vec2 pUV;
layout (location = 3) in vec4 fragPosLightSpace;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 shadowColor;
uniform float blurDistance;
uniform vec3 cameraPosition;
uniform sampler2D shadowMap;

uniform vec3 albedo;
uniform float roughness;
uniform sampler2D baseTexture;
uniform int hasBaseTexture;

layout (location = 0) out vec4 fragColor;

float shadowCalculation(vec3 normal) {
    float bias = max(0.0001 * (1.0 - dot(normal, sunDirection)), 0.005);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0) shadow = 0.0;

    return shadow;
}

void main() {
    vec4 color = vec4(1.0);

    if(hasBaseTexture == 1) {
        color = texture(baseTexture, pUV);
    }
    else {
        color = vec4(albedo, 1.0f);
    }

    float depth = distance(cameraPosition, pPosition);

    vec3 normal = normalize(cross(dFdx(fragWorldPos), dFdy(fragWorldPos)));

    vec3 diffuse = max(dot(normal, normalize(-sunDirection)), 0.0f) * sunColor;

    vec3 viewDirection = normalize(cameraPosition - pPosition);
    vec3 reflectDirection = reflect(normalize(sunDirection), normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 finalSpecular = (1.0 - roughness) * spec * sunColor;

    float shadow = shadowCalculation(normal);

    vec3 lighting = (shadowColor + (1.0 - shadow)) * (diffuse + finalSpecular) * color.rgb;
    vec4 final = vec4(lighting, 1.0f);
    fragColor = final;
    gl_FragDepth = gl_FragCoord.z;
}