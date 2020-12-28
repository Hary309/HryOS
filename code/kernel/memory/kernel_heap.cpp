#include "kernel_heap.hpp"

#include "logger/logger.hpp"

#include "allocator.hpp"
#include "assert.hpp"
#include "memory_map.hpp"

static allocator* allocator;

void kheap::init()
{
    auto entries = mmap::get_entries();

    mmap::entry largest{};

    for (auto& entry : entries)
    {
        if (entry.type == mmap::entry::type::available)
        {
            if (entry.length > largest.length)
            {
                largest = entry;
            }
        }
    }

    HRY_ASSERT(largest.type == mmap::entry::type::available, "No available memory segment to use");
    HRY_ASSERT(largest.length > 10 * 1024 * 1024, "Not enough memory");

    auto kernel_offset = 2 * 1024 * 1024;

    allocator = allocator::create(
        static_cast<uintptr_t>(largest.addr + kernel_offset), largest.length - kernel_offset);

    allocator->print();

    logger::info(
        "Kernel heap initialized! (available {} MB)",
        static_cast<uint32_t>(largest.length - kernel_offset) / 1024 / 1024);
}

void* kheap::allocate(size_t size)
{
    void* result = reinterpret_cast<void*>(allocator->allocate(size));

    HRY_ASSERT(result != nullptr, "Not enough memory!");

    return result;
}

void kheap::free(void* addr)
{
    allocator->free(reinterpret_cast<uintptr_t>(addr));
}

void kheap::print()
{
    allocator->print();
}
