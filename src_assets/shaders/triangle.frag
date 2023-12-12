#version 450

layout (binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragUV);

    if (texColor.a < 0.01) {
        discard;
    }

    outColor = fragColor * texColor;
}