#version 450

layout(binding = 1) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} cam;

layout( push_constant ) uniform constants {
	mat4 model;
} PushConstants;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragColor;

void main() {
    gl_Position = cam.proj * cam.view * PushConstants.model * vec4(inPosition, 0.0, 1.0);
    fragUV = inUV;
    fragColor = inColor;
}