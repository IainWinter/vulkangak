#version 450

layout(binding = 1) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} cam;

layout( push_constant ) uniform constants {
	mat4 model;
} PushConstants;

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inInstancePos;
layout (location = 3) in vec2 inInstanceScale;
layout (location = 4) in float inInstanceRotation;
layout (location = 5) in vec4 inInstanceColor;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

mat4 calcModel(vec3 pos, vec2 scale, float rotation) {
    float sr = sin(rotation);
    float cr = cos(rotation);

    float m00 = scale.x *  cr;
    float m10 = scale.x *  sr;
    float m01 = scale.y * -sr;
    float m11 = scale.y *  cr;
    float m03 = pos.x;
    float m13 = pos.y;
    float m23 = pos.z;

    return mat4(
        m00, m10, 0.0, 0.0,
        m01, m11, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        m03, m13, m23, 1.0
    );
}

void main() {
    mat4 transform = calcModel(inInstancePos, inInstanceScale, inInstanceRotation);

    gl_Position = cam.viewProj * PushConstants.model * transform * vec4(inPosition, 0.0, 1.0);
    fragUV = inUV;
    fragColor = inInstanceColor;
}