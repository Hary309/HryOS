#include "memory_map.hpp"

#include <binary.hpp>
#include <string_view.hpp>

#include "logger/logger.hpp"

using entries_t = hlib::span<mmap::entry>;

static entries_t entries;

hlib::string_view type_to_string(enum mmap::entry::type type)
{
    switch (type)
    {
        case mmap::entry::type::available: return "available";
        case mmap::entry::type::reserved: return "reserved";
        case mmap::entry::type::acpi_reclaimable: return "acpi_reclaimable";
        case mmap::entry::type::nvs: return "nvs";
        case mmap::entry::type::badram: return "badram";
        default:;
    }

    return "";
}

bool mmap::init(multiboot_info* mbi)
{
    if (!hlib::get_bit(mbi->flags, 6))
    {
        logger::error("No mmap info!");
        return false;
    }

    entries = { reinterpret_cast<entries_t::iterator>(mbi->mmap_addr),
                reinterpret_cast<entries_t::iterator>(mbi->mmap_addr + mbi->mmap_length) };

    for (const auto& entry : entries)
    {
        logger::info(
            "Size = 0x{x} :: base_addr = 0x{x} :: length = 0x{x} :: type = {}", entry.size,
            entry.addr, entry.length, type_to_string(entry.type));
    }

    logger::info("Memory map initialized");

    return true;
}

hlib::span<mmap::entry> mmap::get_entries()
{
    return entries;
}
