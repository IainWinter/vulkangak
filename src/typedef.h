#pragma once

#include <stdint.h>
#include <cstdlib>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
using namespace glm;

inline float random() {
    return rand() / (float)RAND_MAX;
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