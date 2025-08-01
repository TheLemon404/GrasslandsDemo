#version 330 core

layout (location = 0) in vec2 pUV;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D specularTexture;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform vec3 ambientColor;
uniform vec3 cameraPosition;

out vec4 fragColor;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

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
    vec3 normal = blurredSample(normalTexture);
    vec3 color = blurredSample(colorTexture);
    vec3 specular = texture(specularTexture, pUV).rgb;

    vec3 diffuse = max(dot(normal, normalize(-sunDirection)), 0.0f) * sunColor;

    vec3 viewDirection = normalize(cameraPosition - position);
    vec3 reflectDirection = reflect(normalize(sunDirection), normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 finalSpecular = specular * spec * sunColor;

    vec3 lighting = (ambientColor + diffuse + finalSpecular) * color;
    fragColor = vec4(lighting, 1.0f);
}