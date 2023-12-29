#pragma once

#include <format>
#include <string>

template<typename... _t>
void print(const std::format_string<_t...> fmt, const _t&... args) {
    std::string str = std::vformat(fmt.get(), std::make_format_args(args...));
    printf("%s\n", str.c_str());
}