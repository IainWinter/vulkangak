#version 450

layout(binding = 1) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} cam;

layout(push_constant) uniform constants {
	mat4 model;
} PushConstants;

layout (location = 0) in vec2 vertPos;
layout (location = 1) in vec2 vertUv;

layout(location = 0) out vec2 fragUv;

void main() {
    fragUv = vertUv;
    gl_Position = cam.viewProj * PushConstants.model * vec4(vertPos, 0, 1);
}