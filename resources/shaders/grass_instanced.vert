#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in mat4 aiTransform;

uniform vec2 terrainSpaceUVBounds;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightView;
uniform mat4 lightProjection;

uniform sampler2D heightMap;
uniform float heightMapStrength;

uniform float time;

layout (location = 0) out vec3 pPosition;
layout (location = 1) out vec3 pNormal;
layout (location = 2) out vec2 pUV;
layout (location = 3) out vec4 fragPosLightSpace;
layout (location = 4) out vec2 terrainSpaceUV;

mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, c,   s, 0.0,
    0.0, -s,  c, 0.0,
    0.0, 0.0, 0.0, 1.0
    );
}

float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    //grass curving
    vec4 tempWorldPosition = aiTransform * vec4(aPosition, 1.0f);
    float n = cos(time + tempWorldPosition.x + tempWorldPosition.z + 25 * random(tempWorldPosition.xz));
    float curveAmount = aUV.y * 0.5 + (n / 15);
    mat4 curveMatrix = rotateX(curveAmount);

    vec4 worldPosition = aiTransform * (vec4(aPosition, 1.0f) * curveMatrix);
    terrainSpaceUV = ((vec2(worldPosition.x, worldPosition.z) + terrainSpaceUVBounds) / (terrainSpaceUVBounds * 2.0));

    mat3 normalMatrix = mat3(transpose(inverse(aiTransform)));

    vec4 modifiedPosition = (worldPosition + vec4(0.0f, texture(heightMap, terrainSpaceUV).r * heightMapStrength, 0.0f, 0.0f));

    gl_Position = projection * view * modifiedPosition;
    pNormal = normalize(normalMatrix * aNormal).xyz;
    pUV = aUV;
    pPosition = modifiedPosition.xyz;
    fragPosLightSpace = lightProjection * lightView * modifiedPosition;
}