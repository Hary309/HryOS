#include "memory/kernel_heap.hpp"

void* operator new(size_t size)
{
    auto* ret = kheap::allocate(size);
    return ret;
}

void* operator new[](size_t size)
{
    auto* ret = kheap::allocate(size);
    return ret;
}

void operator delete(void* ptr)
{
    kheap::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
    kheap::free(ptr);
}