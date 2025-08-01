#version 330 core

layout (location = 0) in vec2 pUV;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D materialTexture;
uniform sampler2D depthTexture;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;

out vec4 fragColor;

uniform float weight[5] = float[] (0.382925, 0.241732, 0.060598, 0.005977, 0.000229);

vec3 blurredSample(sampler2D image) {
    vec2 texOffset = 1.0f / textureSize(image, 0);
    vec3 result = texture(image, pUV).rgb * weight[0];
    for(int i = 1; i < 5; ++i)
    {
        result += texture(image, pUV + vec2(0.0, texOffset.y * i)).rgb * weight[i];
        result += texture(image, pUV - vec2(0.0, texOffset.y * i)).rgb * weight[i];
    }
    return result;
}

void main() {
    vec3 position = texture(positionTexture, pUV).rgb;
    float depth = distance(cameraPosition, position);

    vec3 normal = vec3(0.0f);
    if(depth > 15.0f) {
        normal = texture(normalTexture, pUV).rgb;
    }
    else {
        normal = blurredSample(normalTexture);
    }

    vec3 color = texture(colorTexture, pUV).rgb;
    vec4 material = texture(materialTexture, pUV);

    vec3 diffuse = max(dot(normal, normalize(-sunDirection)), 0.0f) * sunColor;

    vec3 viewDirection = normalize(cameraPosition - position);
    vec3 reflectDirection = reflect(normalize(sunDirection), normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 finalSpecular = material.r * spec * sunColor;

    vec3 lighting = (ambientColor + diffuse + finalSpecular) * color;
    fragColor = vec4(lighting, 1.0f);
}