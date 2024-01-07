#version 450

layout (set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragUv;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in flat int fragTextureIndex;

layout(location = 0) out vec4 outColor;

float sdfCircle(vec2 fromCenter, float radius) {
    return length(fromCenter) - radius;
}

void main() {
    float sdf = 0.0 - sdfCircle(fragUv * 2 - 1, 1);

    vec4 color = fragColor * texture(texSampler, fragUv);
    outColor = sdf * color;
}