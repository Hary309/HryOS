#pragma once

#include <stdint.h>

#include <span.hpp>

#include "multiboot.h"

namespace mmap
{
    struct entry
    {
        enum class type
        {
            available = 1,
            reserved = 2,
            acpi_reclaimable = 3,
            nvs = 4,
            badram = 5
        };

        uint32_t size;
        uint64_t addr;
        uint64_t length;

        type type;
    } __attribute__((packed));

    bool init(multiboot_info* mbi);

    hlib::span<entry> get_entries();
} // namespace mmap
