#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} camera;

layout(push_constant) uniform PushConstants {
	mat4 model;
} pushConstants;

layout (location = 0) in vec2 vertPos;
layout (location = 1) in vec2 vertUv;

layout(location = 0) out vec2 fragUv;

void main() {
    fragUv = vertUv;
    gl_Position = camera.viewProj * pushConstants.model * vec4(vertPos, 0, 1);
}