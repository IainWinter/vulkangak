#pragma once

#include <stdint.h>

using u8 = uint8_t;
using u32 = uint32_t;
using s32 = int32_t;
using u64 = uint64_t;

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/common.hpp"
using namespace glm;

// move these to a math file

inline float damping(float deltaTime, float damp) {
    return clamp(1.f - deltaTime * damp, 0.f, 1.f);
}

inline float lerp(const float& a, const float& b, float w) {
    return a * (1.f - w) + b * w;
}
inline vec2 lerp(const vec2& a, const vec2& b, float w) {
    return a * (1.f - w) + b * w;
}

inline vec3 lerp(const vec3& a, const vec3& b, float w) {
    return a * (1.f - w) + b * w;
}

inline vec4 lerp(const vec4& a, const vec4& b, float w) {
    return a * (1.f - w) + b * w;
}