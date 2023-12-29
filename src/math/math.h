#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
using namespace glm;

// Return: x where x is clamped to [min, max]
float clamp(float x, float min, float max);

// Return: damp * (1.0 - deltaTime) clamped to [0, 1]
float damping(float damp, float deltaTime);

float lerp(const float& a, const float& b, float w);
vec2  lerp(const vec2& a, const vec2& b, float w);
vec3  lerp(const vec3& a, const vec3& b, float w);
vec4  lerp(const vec4& a, const vec4& b, float w);

// Return: v where ||v|| is clamped to [min, max]
vec2 clamp_length(vec2 v, float min, float max);

// Return: v / ||v|| or v if ||v|| == 0
vec2 normalize_safe(vec2 v);