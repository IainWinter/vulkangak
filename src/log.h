#pragma once

#include "fmt/core.h"

template<typename... _t>
void print(const fmt::format_string<_t...> fmt, const _t&... args) {
    std::string str = fmt::vformat(fmt.get(), fmt::make_format_args(args...));
    printf("%s\n", str.c_str());
}