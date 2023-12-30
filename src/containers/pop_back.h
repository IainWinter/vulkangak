#pragma once

#include <vector>

template<typename _t, typename _i>
void pop_back(std::vector<_t>& vec, _i& i) {
    // if (vec.size() - 1 == i) {
    //     vec.pop_back();
    //     return;
    // }

    vec.at(i) = vec.back();
    vec.pop_back();
    i--;
}