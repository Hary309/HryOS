#pragma once

#include <stddef.h>

namespace kheap
{
    void init();

    void* allocate(size_t size);

    void free(void* addr);

    void print();
} // namespace kheap
