#include "memory/kernel_heap.hpp"

void* operator new(size_t size)
{
    return kheap::allocate(size);
}

void* operator new[](size_t size)
{
    return kheap::allocate(size);
}

void operator delete(void* ptr)
{
    kheap::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
    kheap::free(ptr);
}