#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
using namespace glm;

float clamp(float x, float min, float max);

float damping(float deltaTime, float damp);

float lerp(const float& a, const float& b, float w);
vec2  lerp(const vec2& a, const vec2& b, float w);
vec3  lerp(const vec3& a, const vec3& b, float w);
vec4  lerp(const vec4& a, const vec4& b, float w);