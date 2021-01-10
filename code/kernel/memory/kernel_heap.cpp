#include "kernel_heap.hpp"

#include "logger/logger.hpp"
#include "scheduler/int_lock.hpp"
#include "terminal/command_line.hpp"

#include "allocator.hpp"
#include "assert.hpp"
#include "kernel.hpp"
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

    const uint32_t kernel_start = reinterpret_cast<uint32_t>(&_kernel_start);
    const uint32_t kernel_end = reinterpret_cast<uint32_t>(&_kernel_end);

    uint32_t heap_start = largest.addr;
    uint32_t heap_end = largest.addr = largest.length;

    uint32_t kernel_offset = 0;

    if (heap_start >= kernel_start && heap_start <= kernel_end)
    {
        heap_start += align_to(kernel_end, 16);
    }

    allocator = allocator::create(heap_start, heap_end);

    allocator->print();

    logger::info(
        "Kernel heap initialized! (available {} MB)",
        static_cast<uint32_t>(largest.length - kernel_offset) / 1024 / 1024);

    command_line::register_command("mem", kheap::print);
}

void* kheap::allocate(size_t size)
{
    HRY_ASSERT(allocator != nullptr, "Kernel Heap not initialized!");

    direct_int_lock lock;
    void* result = reinterpret_cast<void*>(allocator->allocate(size));

    HRY_ASSERT(result != nullptr, "Not enough memory!");

    return result;
}

void kheap::free(void* addr)
{
    direct_int_lock lock;
    allocator->free(reinterpret_cast<uintptr_t>(addr));
}

void kheap::print()
{
    direct_int_lock lock;
    allocator->print();
}
