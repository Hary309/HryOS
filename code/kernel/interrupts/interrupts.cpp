#include "interrupts.hpp"

#include <stdint.h>

#include <array.hpp>

#include "logger/logger.hpp"
#include "memory/gdt.hpp"

#include "algorithm.hpp"
#include "port_utils.hpp"

using IRQ_t = int();

struct idt_entry
{
    uint16_t offset_lowerbits;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t offset_higherbits;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t size;
    idt_entry* idt;
} __attribute__((packed));

static_assert(sizeof(idt_entry) == 8, "Wrong idt_entry size");
static_assert(sizeof(idt_ptr) == 6, "Wrong idt_ptr size");

// programming PIC
static constexpr auto PIC1 = 0x20;
static constexpr auto PIC2 = 0XA0;
static constexpr auto PIC1_COMMAND = PIC1;
static constexpr auto PIC1_DATA = PIC1 + 1;
static constexpr auto PIC2_COMMAND = PIC2;
static constexpr auto PIC2_DATA = PIC2 + 1;

enum ICW1
{
    SEND_ICW4 = 1 << 0,
    INIT = 1 << 4
};

enum ICW4
{
    MODE_8086 = 1 << 0,
    AUTO_EOI = 1 << 1,
};

// external functions
extern "C" int load_idt(idt_ptr* idtp);
extern "C" void enable_interrupts();

extern "C" int irq0();
extern "C" int irq1();
extern "C" int irq2();
extern "C" int irq3();
extern "C" int irq4();
extern "C" int irq5();
extern "C" int irq6();
extern "C" int irq7();
extern "C" int irq8();
extern "C" int irq9();
extern "C" int irq10();
extern "C" int irq11();
extern "C" int irq12();
extern "C" int irq13();
extern "C" int irq14();
extern "C" int irq15();

static hlib::array<idt_entry, 256> IDT{};
static idt_ptr IDTP;

idt_entry set_idt_entry(IRQ_t irq, uint16_t selector, uint8_t type)
{
    uint32_t address = reinterpret_cast<uint32_t>(irq);

    idt_entry entry;
    entry.offset_lowerbits = address & 0xFFFF;
    entry.selector = selector;
    entry.zero = 0;
    entry.flags = type;
    entry.offset_higherbits = address >> 16;

    return entry;
}

void remap_pic()
{
    // ICW1
    // Restart the both PICs
    out_byte(PIC1_COMMAND, ICW1::INIT | ICW1::SEND_ICW4);
    out_byte(PIC2_COMMAND, ICW1::INIT | ICW1::SEND_ICW4);

    // ICW2
    out_byte(PIC1_DATA, 32); //Make PIC1 start at 32 (of IDT table)
    out_byte(PIC2_DATA, 40); //Make PIC2 start at 40 (of IDT table)

    // ICW3
    out_byte(PIC1_DATA, 0b00000100); // select IRQ2 as connection with PIC2
    out_byte(PIC2_DATA, 2);          // set selected IRQ port for master (2 from the right side)

    // ICW4
    // Set the default 8086 mode for PICs
    out_byte(PIC1_DATA, ICW4::MODE_8086);
    out_byte(PIC2_DATA, ICW4::MODE_8086);
}

void setup_idt()
{
    IDTP.size = (IDT.size() * sizeof(idt_entry));

    IDTP.idt = IDT.data();

    load_idt(&IDTP);
}

void setup_irq()
{
    IDT[32] = set_idt_entry(irq0, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[33] = set_idt_entry(irq1, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[34] = set_idt_entry(irq2, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[35] = set_idt_entry(irq3, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[36] = set_idt_entry(irq4, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[37] = set_idt_entry(irq5, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[38] = set_idt_entry(irq6, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[39] = set_idt_entry(irq7, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[40] = set_idt_entry(irq8, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[41] = set_idt_entry(irq9, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[42] = set_idt_entry(irq10, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[43] = set_idt_entry(irq11, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[44] = set_idt_entry(irq12, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[45] = set_idt_entry(irq13, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[46] = set_idt_entry(irq14, gdt::KERNEL_CODE_SELECTOR, 0x8e);
    IDT[47] = set_idt_entry(irq15, gdt::KERNEL_CODE_SELECTOR, 0x8e);
}

void interrupts::init()
{
    hlib::fill(IDT.begin(), IDT.end(), idt_entry{});

    remap_pic();
    setup_irq();

    setup_idt();

    enable_interrupts();

    logger::info("Interrupts initialized");
}
