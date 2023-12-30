#pragma once

#include <vector>
#include <utility>
#include <typeinfo>

#include "pop_back.h"

struct basic_defer_deleter {
    void add(void* ptr);
    void flush();
    void flush_now();

    virtual void type_erased_delete(void* ptr) = 0;

    std::vector<std::pair<void*, int>> queue;
};

template<typename _t>
struct defer_deleter : public basic_defer_deleter {
    void type_erased_delete(void* ptr) override {
        delete (_t*)ptr;
    }
};

template<typename _t>
struct defer_deleter<_t[]> : public basic_defer_deleter {
    void type_erased_delete(void* ptr) override {
        delete[] (_t*)ptr;
    }
};

void defer_delete_register_internal(basic_defer_deleter* deleter);

void delete_defer_flush();
void delete_defer_flush_now();

template<typename _t>
void delete_defer(_t* ptr) {
    static defer_deleter<_t>* deleter = nullptr;
    if (deleter == nullptr) {
        deleter = new defer_deleter<_t>();
        defer_delete_register_internal(deleter);
    }

    deleter->add(ptr);
}