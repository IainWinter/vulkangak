#version 450

layout (set = 1, binding = 0) uniform sampler2D msdfSampler;

layout(location = 0) in vec2 fragUv;

layout(location = 0) out vec4 outColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
    const float pxRange = 2.0;
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(msdfSampler, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(fragUv);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec3 msd = texture(msdfSampler, fragUv).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    //if (opacity == 0.0) {
    //    discard;
    //}

    outColor = vec4(1, 1, 1, opacity);
}