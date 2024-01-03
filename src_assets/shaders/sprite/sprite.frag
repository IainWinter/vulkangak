#version 450

layout (binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in flat int fragTextureIndex;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = fragColor * texture(texSampler, fragUV);
    outColor = color;
}