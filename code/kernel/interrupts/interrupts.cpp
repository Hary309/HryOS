#include "interrupts.hpp"

#include <stdint.h>

#include <algorithm.hpp>
#include <array.hpp>

#include "logger/logger.hpp"
#include "memory/gdt.hpp"

#include "exceptions.hpp"
#include "isr.hpp"
#include "port_utils.hpp"

using IRQ_t = int();

struct sys_regs
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t irq_id;
};

enum class gate_type
{
    task = 0b00101,
    interrupt_16 = 0b00110,
    interrupt_32 = 0b01110,
    trap_16 = 0b00111,
    trap_32 = 0b01111
};

// desc: https://wiki.osdev.org/Interrupt_Descriptor_Table
struct idt_entry
{
    uint16_t offset_lowerbits : 16;
    uint16_t selector : 16; // destination code segment
    uint8_t zero : 8;
    gate_type type : 5;
    uint8_t dpl : 2; // Descriptor Privilege Level (required privilege)
    uint8_t present : 1;
    uint16_t offset_higherbits : 16;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t size;
    idt_entry* idt;
} __attribute__((packed));

static_assert(sizeof(idt_entry) == 8, "Wrong idt_entry size");
static_assert(sizeof(idt_ptr) == 6, "Wrong idt_ptr size");

// error messages
static hlib::array<const char*, 21> ERROR_MESSAGES{ "Divide Error",
                                                    "Debug Exception",
                                                    "NMI Interrupt",
                                                    "Breakpoint",
                                                    "Overflow",
                                                    "BOUND Range Exceeded",
                                                    "Invalid Opcode (Undefined Opcode)",
                                                    "Device Not Available (No Math Coprocessor)",
                                                    "Double Fault",
                                                    "Coprocessor Segment Overrun",
                                                    "Invalid TSS",
                                                    "Segment Not Present",
                                                    "Stack-Segment Fault",
                                                    "General Protection",
                                                    "Page Fault",
                                                    "Intel Reserved",
                                                    "x87 FPU Floating-Point Error (Math Fault)",
                                                    "Alignment Check",
                                                    "Machine Check",
                                                    "SIMD Floating-Point Exception",
                                                    "Virtualization Exception" };

// programming PIC
static constexpr auto PIC1 = 0x20;
static constexpr auto PIC1_COMMAND = PIC1;
static constexpr auto PIC1_DATA = PIC1 + 1;
static constexpr auto PIC1_OFFSET = 32;
static constexpr auto PIC2 = 0xA0;
static constexpr auto PIC2_COMMAND = PIC2;
static constexpr auto PIC2_DATA = PIC2 + 1;
static constexpr auto PIC2_OFFSET = 40;

enum ICW1
{
    send_ICW4 = 1 << 0,
    init = 1 << 4
};

enum ICW4
{
    mode_8086 = 1 << 0,
    auto_eoi = 1 << 1,
};

// external functions
extern "C" int load_idt(idt_ptr* idtp);
extern "C" void enable_interrupts();

// variables
static hlib::array<idt_entry, 256> IDT{};
static idt_ptr IDTP;

void set_idt_entry(size_t offset, IRQ_t irq, uint16_t selector, gate_type type, uint8_t dpl)
{
    uint32_t address = reinterpret_cast<uint32_t>(irq);

    idt_entry& entry = IDT[offset];
    entry.offset_lowerbits = address & 0xFFFF;
    entry.selector = selector;
    entry.zero = 0;
    entry.type = type;
    entry.dpl = dpl;
    entry.present = 1; // always 1
    entry.offset_higherbits = address >> 16;
}

void setup_exceptions()
{
    set_idt_entry(0, exception0, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(1, exception1, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(2, exception2, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(3, exception3, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(4, exception4, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(5, exception5, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(6, exception6, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(7, exception7, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(8, exception8, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(9, exception9, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(10, exception10, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(11, exception11, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(12, exception12, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(13, exception13, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(14, exception14, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    // set_idt_entry(15, exception15, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0); // intel reserved
    set_idt_entry(16, exception16, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(17, exception17, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(18, exception18, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(19, exception19, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(20, exception20, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
}

void remap_pic()
{
    // ICW1
    // Restart the both PICs
    out_byte(PIC1_COMMAND, ICW1::init | ICW1::send_ICW4);
    out_byte(PIC2_COMMAND, ICW1::init | ICW1::send_ICW4);

    // ICW2
    out_byte(PIC1_DATA, PIC1_OFFSET); //Make PIC1 start at 32 (of IDT table)
    out_byte(PIC2_DATA, PIC2_OFFSET); //Make PIC2 start at 40 (of IDT table)

    // ICW3
    out_byte(PIC1_DATA, 0b00000100); // select IRQ2 as connection with PIC2
    out_byte(PIC2_DATA, 2);          // set selected IRQ port for master (2 from the right side)

    // ICW4
    // Set the default 8086 mode for PICs
    out_byte(PIC1_DATA, ICW4::mode_8086);
    out_byte(PIC2_DATA, ICW4::mode_8086);
}

void setup_idtp()
{
    IDTP.size = (IDT.size() * sizeof(idt_entry));
    IDTP.idt = IDT.data();

    load_idt(&IDTP);
}

void setup_pic_interrupts()
{
    set_idt_entry(PIC1_OFFSET + 0, isr0, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 1, isr1, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 2, isr2, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 3, isr3, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 4, isr4, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 5, isr5, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 6, isr6, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC1_OFFSET + 7, isr7, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);

    set_idt_entry(PIC2_OFFSET + 0, isr8, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 1, isr9, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 2, isr10, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 3, isr11, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 4, isr12, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 5, isr13, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 6, isr14, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(PIC2_OFFSET + 7, isr15, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
}

extern "C" __attribute__((fastcall)) void exception_handler(sys_regs* regs)
{
    logger::error("Exception");
    logger::info("Message: {}", ERROR_MESSAGES[regs->irq_id]);
}

// use fastcall to get regs in eax register to pass pointer not the value (avoid coping)
extern "C" __attribute__((fastcall)) void pic_handler(sys_regs* regs)
{
    if (regs->irq_id >= 8)
    {
        out_byte(0xA0, 0x20);
    }

    out_byte(0x20, 0x20);
}

void interrupts::init()
{
    setup_exceptions();

    remap_pic();
    setup_pic_interrupts();

    setup_idtp();

    enable_interrupts();

    logger::info("Interrupts initialized");
}
