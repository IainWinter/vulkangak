#pragma once

#include "vec2.hpp"
using namespace glm;

struct FontGlyph {
    vec2 posMin;
    vec2 posMax;
    vec2 uvMin;
    vec2 uvMax;

    float advance;
    int index;
    int character;
};

