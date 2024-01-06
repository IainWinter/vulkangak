#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} camera;

layout(push_constant) uniform PushConstants {
	mat4 model;
    float totalLength;
} pushConstants;

layout (location = 0) in vec3 i_pos;

layout (location = 1) in vec2  ii_pointA;
layout (location = 2) in float ii_distanceFromStartA;
layout (location = 3) in vec2  ii_pointB;
layout (location = 4) in float ii_distanceFromStartB;

void main() {
    float ratioToEnd = mix(ii_distanceFromStartA, ii_distanceFromStartB, i_pos.z) / pushConstants.totalLength;
    float width = .2 * ratioToEnd;

    vec2 xBasis = normalize(ii_pointB - ii_pointA);
    vec2 yBasis = vec2(-xBasis.y, xBasis.x);
    
    vec2 offsetA = ii_pointA + width * (i_pos.x * xBasis + i_pos.y * yBasis);
    vec2 offsetB = ii_pointB + width * (i_pos.x * xBasis + i_pos.y * yBasis);

    vec2 point = mix(offsetA, offsetB, i_pos.z);

    gl_Position = camera.viewProj * pushConstants.model * vec4(point, 0, 1);
}