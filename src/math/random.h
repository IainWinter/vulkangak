#pragma once

#include "typedef.h"
#include "math/math.h"

void random_set_seed(u32 seed);
u32 random_pcg_hash(u32 input);
u32 random_next();

// Generate values between (0, 1)
bool  random_bool ();
s32   random_s32  ();
float random_float();
vec2  random_vec2 ();
vec3  random_vec3 ();
vec4  random_vec4 ();

// Return true if a roll between (0, 1) is below ratioTrue
bool random_bool_ratio_true(float ratioTrue);

// Generate values between (0, max)
s32   random_s32_max  (s32 max);
float random_float_max(float max);
vec2  random_vec2_max (vec2 max);
vec2  random_vec2_max (float maxX, float maxY);
vec3  random_vec3_max (vec3 max);
vec3  random_vec3_max (float maxX, float maxY, float maxZ);
vec4  random_vec4_max (vec4 max);
vec4  random_vec4_max (float maxX, float maxY, float maxZ, float maxW);

template<typename _enum>
_enum random_enum(_enum max) {
	return static_cast<_enum>(random_s32_max((s32)max));
}

// Generate values between (min, min + add)
s32   random_s32_min_add  (s32 min, s32 add);
float random_float_min_add(float min, float add);
vec2  random_vec2_min_add (vec2 min, vec2 add);
vec2  random_vec2_min_add (float minX, float minY, float addX, float addY);
vec3  random_vec3_min_add (vec3 min, vec3 add);
vec3  random_vec3_min_add (float minX, float minY, float minZ, float addX, float addY, float addZ);
vec4  random_vec4_min_add (vec4 min, vec4 add);
vec4  random_vec4_min_add (float minX, float minY, float minZ, float minW, float addX, float addY, float addZ, float addW);

// Generate values between (min, max)
s32   random_s32_min_max  (s32 min, s32 max);
float random_float_min_max(float min, float max);
vec2  random_vec2_min_max (vec2 min, vec2 max);
vec2  random_vec2_min_max (float minX, float minY, float maxX, float maxY);
vec3  random_vec3_min_max (vec3 min, vec3 max);
vec3  random_vec3_min_max (float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
vec4  random_vec4_min_max (vec4 min, vec4 max);
vec4  random_vec4_min_max (float minX, float minY, float minZ, float minW, float maxX, float maxY, float maxZ, float maxW);

// Generate values between (-1, 1)
s32   random_s32_centered  ();
float random_float_centered();
vec2  random_vec2_centered ();
vec3  random_vec3_centered ();
vec4  random_vec4_centered ();

s32   random_s32_centered_extent  (u32 extent);
float random_float_centered_extent(float extent);
vec2  random_vec2_centered_extent (vec2 extent);
vec2  random_vec2_centered_extent (float extentX, float extentY);
vec3  random_vec3_centered_extent (vec3 extent);
vec3  random_vec3_centered_extent (float extentX, float extentY, float extentZ);
vec4  random_vec4_centered_extent (vec4 extent);
vec4  random_vec4_centered_extent (float extentX, float extentY, float extentZ, float extentW);

// Generate vectors on the edge of a circle
vec2 random_vec2_ring();
vec3 random_vec3_ring();

// Generate vectors inside a circle with radius 1
vec2 random_vec2_circle();
vec3 random_vec3_circle();

// Generate vectors inside a circle with a radius
vec2 random_vec2_circle_radius(float radius);
vec3 random_vec3_circle_radius(float radius);

// Generate vectors between (min, thickness) radius
vec2 random_vec2_ring_thick(float min, float thickness);
vec3 random_vec3_ring_thick(float min, float thickness);

// Generate points on the edge of a box with a thickness
vec2 random_vec2_outside_of_box(float extentX, float extentY, float paddingX, float paddingY);

struct RandomBool {
    float odds;
    operator bool() const { return get(); }
    bool get() const { return random_bool_ratio_true(odds); }
};

struct RandomInt {
    int min;
    int max;
    operator int() const { return get(); }
    int get() const { return random_s32_min_max(min, max); }
};

struct RandomFloat {
    float min;
    float max;
    operator float() const { return get(); }
    float get() const { return random_float_min_max(min, max); }
};

struct RandomFloat2 {
    vec2 min;
    vec2 max;
    operator vec2() const { return get(); }
    vec2 get() const { return random_vec2_min_max(min, max);  }
};

struct RandomFloat3 {
    vec3 min;
    vec3 max;
    operator vec3() const { return get(); }
    vec3 get() const { return random_vec3_min_max(min, max);  }
};

struct RandomFloat4 {
    vec4 min;
    vec4 max;
    operator vec4() const { return get(); }
    vec4 get() const { return random_vec4_min_max(min, max); }
};

struct RandomCircle {
    vec2 radius;
    operator vec2() const { return get(); }
    vec2 get() const { return random_vec2_circle() * radius;  }
};

struct RandomSphere {
    vec3 radius;
    operator vec3() const { return get(); }
    vec3 get() const { return random_vec3_circle() * radius;  }
};