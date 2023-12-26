#include "random.h"

// using rand for now
#include <stdlib.h>

void random_set_seed(u32 seed) {
    srand(seed);
}

bool random_bool() {
    return rand() % 2 == 0;
}

s32 random_s32() {
    return rand() % 2;
}

float random_float() {
    return rand() / (float)RAND_MAX;
}

vec2 random_vec2() {
    return vec2(random_float(), random_float());
}

vec3 random_vec3() {
    return vec3(random_float(), random_float(), random_float());
}

vec4 random_vec4() {
    return vec4(random_float(), random_float(), random_float(), random_float());
}

bool random_bool_ratio_true(bool ratioTrue) {
    return random_float() < ratioTrue;
}

s32 random_s32_max(s32 max) {
    return rand() % (max + 1);
}

float random_float_max(float max) {
    return random_float() * max;    
}

vec2 random_vec2_max(vec2 max) {
    return random_vec2() * max;
}

vec2 random_vec2_max(float maxX, float maxY) {
    return random_vec2_max(vec2(maxX, maxY));
}

vec3 random_vec3_max(vec3 max) {
    return random_vec3() * max;
}

vec3 random_vec3_max(float maxX, float maxY, float maxZ) {
    return random_vec3_max(vec3(maxX, maxY, maxZ));
}

vec4 random_vec4_max(vec4 max) {
    return random_vec4() * max;
}

vec4 random_vec4_max(float maxX, float maxY, float maxZ, float maxW) {
    return random_vec4_max(vec4(maxX, maxY, maxZ, maxW));
}

s32 random_s32_min_add(s32 min, s32 add) {
    return min + random_s32_max(add);
}

float random_float_min_add(float min, float add) {
    return min + random_float_max(add);
}

vec2 random_vec2_min_add(vec2 min, vec2 add) {
    return min + random_vec2_max(add);
}

vec2 random_vec2_min_add(float minX, float minY, float addX, float addY) {
    return random_vec2_min_add(vec2(minX, minY), vec2(addX, addY));
}

vec3 random_vec3_min_add(vec3 min, vec3 add) {
    return min + random_vec3_max(add);
}

vec3 random_vec3_min_add(float minX, float minY, float minZ, float addX, float addY, float addZ) {
    return random_vec3_min_add(vec3(minX, minY, minZ), vec3(addX, addY, addZ));
}

vec4 random_vec4_min_add(vec4 min, vec4 add) {
    return min + random_vec4_max(add);
}

vec4 random_vec4_min_add(float minX, float minY, float minZ, float minW, float addX, float addY, float addZ, float addW) {
    return random_vec4_min_add(vec4(minX, minY, minZ, minW), vec4(addX, addY, addZ, addW));
}

s32 random_s32_min_max(s32 min, s32 max) {
    return random_s32_min_add(min, max - min);
}

float random_float_min_max(float min, float max) {
    return random_float_min_add(min, max - min);
}

vec2 random_vec2_min_max(vec2 min, vec2 max) {
    return random_vec2_min_add(min, max - min);
}

vec2 random_vec2_min_max(float minX, float minY, float maxX, float maxY) {
    return random_vec2_min_max(vec2(minX, minY), vec2(maxX, maxY));
}

vec3 random_vec3_min_max(vec3 min, vec3 max) {
    return random_vec3_min_add(min, max - min);
}

vec3 random_vec3_min_max(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
    return random_vec3_min_max(vec3(minX, minY, minZ), vec3(maxX, maxY, maxZ));
}

vec4 random_vec4_min_max(vec4 min, vec4 max) {
    return random_vec4_min_add(min, max - min);
}

vec4 random_vec4_min_max(float minX, float minY, float minZ, float minW, float maxX, float maxY, float maxZ, float maxW) {
    return random_vec4_min_max(vec4(minX, minY, minZ, minW), vec4(maxX, maxY, maxZ, maxW));
}

s32 random_s32_centered() {
    return random_s32_min_max(-1, 1);
}

float random_float_centered() {
    return random_float_min_max(-1.f, 1.f);
}

vec2 random_vec2_centered() {
    return random_vec2_min_max(vec2(-1.f), vec2(1.f));
}

vec3 random_vec3_centered() {
    return random_vec3_min_max(vec3(-1.f), vec3(1.f));
}

vec4 random_vec4_centered() {
    return random_vec4_min_max(vec4(-1.f), vec4(1.f));
}

s32 random_s32_centered_extent(u32 extent) {
    return random_s32_centered() * extent;
}

float random_float_centered_extent(float extent) {
    return random_float_centered() * extent;
}

vec2 random_vec2_centered_extent(vec2 extent) {
    return random_vec2_centered() * extent;
}

vec2 random_vec2_centered_extent(float extentX, float extentY) {
    return random_vec2_centered_extent(vec2(extentX, extentY));
}

vec3 random_vec3_centered_extent(vec3 extent) {
    return random_vec3_centered() * extent;
}

vec3 random_vec3_centered_extent(float extentX, float extentY, float extentZ) {
    return random_vec3_centered_extent(vec3(extentX, extentY, extentZ));
}

vec4 random_vec4_centered_extent(vec4 extent) {
    return random_vec4_centered() * extent;
}

vec4 random_vec4_centered_extent(float extentX, float extentY, float extentZ, float extentW) {
    return random_vec4_centered_extent(vec4(extentX, extentY, extentZ, extentW));
}

#define PI 3.14159265359
#define PI2 6.28318530718

vec2 random_vec2_ring() {
    float angle = random_float_max(PI2);
    
    return vec2(
        cos(angle), 
        sin(angle)
    );
}

vec3 random_vec3_ring() {
    float theta = random_float_max(PI);
    float phi = random_float_max(PI2);

    return vec3(
        sin(theta) * cos(phi),
        sin(theta) * sin(theta),
        cos(theta)
    );
}

vec2 random_vec2_circle() {
    return random_vec2_ring() * random_float();
}

vec3 random_vec3_circle() {
    return random_vec3_ring() * random_float();
}

vec2 random_vec2_circle_radius(float radius) {
    return random_vec2_ring() * random_float_max(radius);
}

vec3 random_vec3_circle_radius(float radius) {
    return random_vec3_ring() * random_float_max(radius);
}

vec2 random_vec2_ring_thick(float min, float thickness) {
    return random_vec2_ring() * random_float_min_add(min, thickness);
}

vec3 random_vec3_ring_thick(float min, float thickness) {
    return random_vec3_ring() * random_float_min_add(min, thickness);
}

vec2 rand_outside_box(float extentX, float extentY, float paddingX, float paddingY) {
	// this seems complex because if you treat the corners as a part of one of the
	// side sections, the probability is off
	
	vec2 insidePadding;

	// push to edge based on sign

	float areaHorizontal = 2 * extentX  * paddingY;
	float areaVertical   = 2 * extentY  * paddingX;
	float areaCorner     =     paddingX * paddingY;

	float pickArea = random_float_max(areaHorizontal + areaVertical + areaCorner);

	if (pickArea < areaHorizontal)
	{
		insidePadding = random_vec2_centered_extent(extentX, paddingY);

		bool top = insidePadding.y > 0;

		if (top) insidePadding.y += extentY;
		else     insidePadding.y -= extentY;
	}

	else if (pickArea < (areaHorizontal + areaVertical))
	{
		insidePadding = random_vec2_centered_extent(paddingX, extentY);

		bool right = insidePadding.x > 0;

		if (right) insidePadding.x += extentX;
		else       insidePadding.x -= extentX;
	}

	else 
	{
		insidePadding = random_vec2_centered_extent(paddingX, paddingY);

		bool right = insidePadding.x > 0;
		bool top = insidePadding.y > 0;

		if (right) insidePadding.x += extentX;
		else       insidePadding.x -= extentX;

		if (top) insidePadding.y += extentY;
		else     insidePadding.y -= extentY;
	}

	return insidePadding;
}
