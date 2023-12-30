#include "defer_delete.h"

void basic_defer_deleter::add(void* ptr)  {
    queue.push_back({ptr, 1});
}

void basic_defer_deleter::flush() {
    for (int i = 0; i < queue.size(); i++) {
        auto& [ptr, count] = queue.at(i);

        if (count == 0) {
            type_erased_delete(ptr);
            pop_back(queue, i);
        }

        count -= 1;
    }
}

void basic_defer_deleter::flush_now() {
    for (auto& [ptr, count] : queue) {
        type_erased_delete(ptr);
    }

    queue.clear();
}

// dll bounds (`._.`)
static std::vector<basic_defer_deleter*> m_deleters;

void defer_delete_register_internal(basic_defer_deleter* deleter) {
    m_deleters.push_back(deleter);
}

void delete_defer_flush() {
    for (auto& deleter : m_deleters) {
        deleter->flush();
    }
}

void delete_defer_flush_now() {
    for (auto& deleter : m_deleters) {
        deleter->flush_now();
    }
}