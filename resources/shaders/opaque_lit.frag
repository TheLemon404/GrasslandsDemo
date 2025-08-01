#version 330 core

layout (location = 0) in vec3 pPosition;
layout (location = 1) in vec3 pNormal;
layout (location = 2) in vec2 pUV;

uniform vec3 albedo;
uniform float specular;
uniform sampler2D baseTexture;
uniform int hasBaseTexture;

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 position;
layout (location = 3) out vec4 spec;

void main()
{
    if(hasBaseTexture == 1) {
        color = vec4(albedo, 1.0f) * texture(baseTexture, pUV);
    }
    else {
        color = vec4(albedo, 1.0f);
    }
    normal = vec4(pNormal, 1.0f);
    position = vec4(pPosition, 1.0f);
    spec = vec4(specular);
}