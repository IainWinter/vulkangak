#include "math/math.h"

#include "glm/geometric.hpp"

float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float damping(float deltaTime, float damp) {
    return clamp(1.f - deltaTime * damp, 0.f, 1.f);
}

float lerp(const float& a, const float& b, float w) {
    return a * (1.f - w) + b * w;
}

vec2 lerp(const vec2& a, const vec2& b, float w) {
    return a * (1.f - w) + b * w;
}

vec3 lerp(const vec3& a, const vec3& b, float w) {
    return a * (1.f - w) + b * w;
}

vec4 lerp(const vec4& a, const vec4& b, float w) {
    return a * (1.f - w) + b * w;
}

vec2 clamp_length(vec2 v, float min, float max) {
    float len = length(v);
    if (len < min) return v * (min / len);
    if (len > max) return v * (max / len);
    return v;
}

vec2 normalize_safe(vec2 v) {
    float len = length(v);
    if (len == 0.f) return v;
    return v / len;
}