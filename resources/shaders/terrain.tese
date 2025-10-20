#version 430 core

layout(quads, fractional_odd_spacing, ccw) in;

#include <transformation_data.glsl>

layout(location = 0) in vec2 teUV[];

uniform TransformationData transformationData;

uniform sampler2D heightMap;
uniform float heightMapStrength;

layout(location = 0) out vec3 pPosition;
layout(location = 1) out vec3 pNormal;
layout(location = 2) out vec2 pUV;
layout(location = 3) out vec4 fragPosLightSpace;

void main() {
    vec2 texelSize = 1.0 / textureSize(heightMap, 0);

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    //bilinear interpolation of uvs
    vec2 t00 = teUV[0];
    vec2 t01 = teUV[1];
    vec2 t10 = teUV[2];
    vec2 t11 = teUV[3];

    vec2 t0 = (t01 - t00) * u + t00;
    vec2 t1 = (t11 - t10) * u + t10;
    vec2 uv = (t1 - t0) * v + t0;

    //bilinear interpolation of positions
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // bilinearly interpolate position coordinate across patch
    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0;

    float height = texture(heightMap, uv).r * heightMapStrength;
    vec4 worldPosition = transformationData.transform * p;
    vec4 modifiedPosition = (worldPosition + vec4(0.0f, height, 0.0f, 0.0f));

    // compute patch surface normal
    float neightborHeight1 = texture(heightMap, uv + vec2(texelSize.x, 0)).r * heightMapStrength;
    float neightborHeight2 = texture(heightMap, uv + vec2(0, texelSize.y)).r * heightMapStrength;
    vec3 neighborVec1 = vec3(1, neightborHeight1, 0) - vec3(0, height, 0);
    vec3 neighborVec2 = vec3(0, neightborHeight2, 1) - vec3(0, height, 0);
    vec4 normal = normalize(vec4(cross(neighborVec2, neighborVec1), 0));

    gl_Position = transformationData.projection * transformationData.view * modifiedPosition;
    pUV = uv;
    pPosition = modifiedPosition.xyz;
    pNormal = normal.xyz;
    fragPosLightSpace = transformationData.lightProjection * transformationData.lightView * modifiedPosition;
}
