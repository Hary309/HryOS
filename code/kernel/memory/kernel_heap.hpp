#pragma once

#include <stddef.h>

namespace kheap
{
    void init();

    void* allocate(size_t size);

    void free(void* addr);

    template<typename T>
    [[nodiscard]] T* allocate()
    {
        return reinterpret_cast<T*>(allocate(sizeof(T)));
    }

    template<typename T>
    void free(T* addr)
    {
        free(reinterpret_cast<void*>(addr));
    }

    void print();
} // namespace kheap
