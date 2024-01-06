#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} camera;

layout(location = 0) in vec2  vertPos;
layout(location = 1) in vec2  vertUv;

layout(location = 2) in vec2  instPos;
layout(location = 3) in vec2  instScale;
layout(location = 4) in float instRotation;
layout(location = 5) in vec4  instColor;
layout(location = 6) in vec2  instUvMin;
layout(location = 7) in vec2  instUvSize;
layout(location = 8) in int   instTextureIndex;

layout(location = 0) out vec2 fragUv;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out flat int fragTextureIndex;

mat4 calcModel2d(vec2 pos, vec2 scale, float rotation) {
    float sr = sin(rotation);
    float cr = cos(rotation);

    float m00 = scale.x *  cr;
    float m10 = scale.x *  sr;
    float m01 = scale.y * -sr;
    float m11 = scale.y *  cr;
    float m03 = pos.x;
    float m13 = pos.y;
    float m23 = 0.0;

    return mat4(
        m00, m10, 0.0, 0.0,
        m01, m11, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        m03, m13, m23, 1.0
    );
}

void main() {
    mat4 transform = calcModel2d(instPos, instScale, instRotation);

    fragUv = vertUv * instUvSize + instUvMin;
    fragColor = instColor;
    fragTextureIndex = instTextureIndex;
    gl_Position = camera.viewProj * transform * vec4(vertPos, 0.0, 1.0);
}