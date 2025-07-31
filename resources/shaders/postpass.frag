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

void main() {
    vec3 position = texture(positionTexture, pUV).rgb;
    vec3 normal = texture(normalTexture, pUV).rgb;
    vec3 color = texture(colorTexture, pUV).rgb;
    vec3 specular = texture(specularTexture, pUV).rgb;

    vec3 lighting = color * ambientColor;
    vec3 diffuse = max(dot(normal, normalize(-sunDirection)), 0.0f) * sunColor;
    vec3 viewDirection = normalize(cameraPosition - position);
    vec3 reflectDirection = reflect(-sunDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 finalSpecular = specular * spec * sunColor;
    lighting += diffuse + finalSpecular;
    fragColor = vec4(lighting, 1.0f);
}