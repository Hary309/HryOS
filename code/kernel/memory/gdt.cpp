#include "gdt.hpp"

#include <stdint.h>

#include "logger/logger.hpp"

// description: https://wiki.osdev.org/Global_Descriptor_Table
struct gdt_descriptor
{
    uint32_t limit_low : 16;      // +0
    uint32_t base_low : 24;       // +16 (16 + 8)
    uint32_t accessed : 1;        // +40 (access byte start)
    uint32_t read_write : 1;      // +41
    uint32_t dc : 1;              // +42
    uint32_t exectuable : 1;      // +43
    uint32_t descriptor_type : 1; // +44
    uint32_t privilege : 2;       // +46
    uint32_t present : 1;         // +47 (access byte end)
    uint32_t limit_high : 4;      // +48
    uint32_t zero : 2;            // +52 (flags start)
    uint32_t big : 1;             // +54
    uint32_t granularity : 1;     // +55 (flags end)
    uint32_t base_high : 8;       // +56

    void set_base(uint32_t base)
    {
        base_low = base;
        base_high = (base & 0xFF000000) >> (4 * 6);
    }

    void set_limit(uint32_t limit)
    {
        limit_low = limit;
        limit_high = (limit & 0x000F0000) >> (4 * 4);
    }

} __attribute__((packed));

struct gdt_ptr
{
    uint16_t size;
    gdt_descriptor* gdt;
} __attribute__((packed));

static_assert(sizeof(gdt_descriptor) == 8, "Wrong descriptor size");
static_assert(sizeof(gdt_ptr) == 6, "Wrong gdt_ptr size");

// src: gdt.asm
extern "C" void load_gdt(gdt_ptr* gdtp);

static gdt_descriptor GDT[5];
static gdt_ptr GDTP;

gdt_descriptor create_null_descriptor()
{
    return {};
}

gdt_descriptor create_kernel_code_descriptor()
{
    gdt_descriptor desc{};

    desc.set_base(0x0);
    desc.set_limit(0xFFFFFFFF);

    desc.read_write = 1;
    desc.exectuable = 1;
    desc.descriptor_type = 1;
    desc.present = 1;

    desc.granularity = 1;
    desc.big = 1;

    return desc;
}

gdt_descriptor create_kernel_data_descriptor()
{
    gdt_descriptor desc{};

    desc.set_base(0x0);
    desc.set_limit(0xFFFFFFFF);

    desc.read_write = 1;
    desc.descriptor_type = 1;
    desc.present = 1;

    desc.granularity = 1;
    desc.big = 1;

    return desc;
}

gdt_descriptor create_user_code_descriptor()
{
    gdt_descriptor desc{};

    desc.set_base(0x0);
    desc.set_limit(0xFFFFFFFF);

    desc.read_write = 1;
    desc.exectuable = 1;
    desc.descriptor_type = 1;
    desc.privilege = 3; // user level
    desc.present = 1;

    desc.granularity = 1;
    desc.big = 1;

    return desc;
}

gdt_descriptor create_user_data_descriptor()
{
    gdt_descriptor desc{};

    desc.set_base(0x0);
    desc.set_limit(0xFFFFFFFF);

    desc.read_write = 1;
    desc.descriptor_type = 1;
    desc.privilege = 3; // user level
    desc.present = 1;

    desc.granularity = 1;
    desc.big = 1;

    return desc;
}

void gdt::init()
{
    GDT[0] = create_null_descriptor();
    GDT[1] = create_kernel_code_descriptor();
    GDT[2] = create_kernel_data_descriptor();
    GDT[3] = create_user_code_descriptor();
    GDT[4] = create_user_data_descriptor();

    GDTP.size = sizeof(GDT);
    GDTP.gdt = GDT;

    load_gdt(&GDTP);

    logger::info("GDT initialized");
}
