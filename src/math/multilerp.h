#pragma once

#include "typedef.h"
#include <vector>

template<typename _t>
_t lerp_multi(std::vector<_t>& list, float w) {
    w = clamp(w, 0.f, 1.f);

    if (list.size() == 0) {
        return _t();
    }

    if (list.size() == 1 || w >= 1.f) {
        return list.back();
    }
    
    float scaled = w * (float)(list.size() - 1);

    u32 indexA = (u32)floor(scaled);
    u32 indexB = indexA + 1;

    _t a = list[indexA];
    _t b = list[indexB];

    float r = scaled - floor(scaled);

    return lerp(a, b, r);
}