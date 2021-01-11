#include "interrupts.hpp"

#include <stdint.h>

#include <algorithm.hpp>
#include <array.hpp>

#include "drivers/vga/color.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "scheduler/scheduler.hpp"
#include "terminal/terminal.hpp"

#include "fault.hpp"
#include "isr.hpp"
#include "serial_port.hpp"

using IRQ_t = int();

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

static bool interrupts_lock = true;

static bool interrupts_enabled = false;

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

// variables
static hlib::array<idt_entry, 256> IDT{};
static idt_ptr IDTP;

static interrupts::callback_t* isr_callbacks[16]{};

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

void remap_pic()
{
    // ICW1
    // Restart the both PICs
    port::out_byte(PIC1_COMMAND, ICW1::init | ICW1::send_ICW4);
    port::out_byte(PIC2_COMMAND, ICW1::init | ICW1::send_ICW4);

    // ICW2
    port::out_byte(PIC1_DATA, PIC1_OFFSET); //Make PIC1 start at 32 (of IDT table)
    port::out_byte(PIC2_DATA, PIC2_OFFSET); //Make PIC2 start at 40 (of IDT table)

    // ICW3
    port::out_byte(PIC1_DATA, 0b00000100); // select IRQ2 as connection with PIC2
    port::out_byte(PIC2_DATA, 2); // set selected IRQ port for master (2 from the right side)

    // ICW4
    // Set the default 8086 mode for PICs
    port::out_byte(PIC1_DATA, ICW4::mode_8086);
    port::out_byte(PIC2_DATA, ICW4::mode_8086);
}

void load_idtp()
{
    IDTP.size = (IDT.size() * sizeof(idt_entry));
    IDTP.idt = IDT.data();

    load_idt(&IDTP);
}

void setup_fault()
{
    set_idt_entry(0, fault0, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(1, fault1, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(2, fault2, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(3, fault3, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(4, fault4, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(5, fault5, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(6, fault6, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(7, fault7, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(8, fault8, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(9, fault9, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(10, fault10, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(11, fault11, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(12, fault12, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(13, fault13, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(14, fault14, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    // set_idt_entry(15, fault15, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0); // intel reserved
    set_idt_entry(16, fault16, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(17, fault17, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(18, fault18, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(19, fault19, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
    set_idt_entry(20, fault20, gdt::KERNEL_CODE_SELECTOR, gate_type::interrupt_32, 0);
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

extern "C" __attribute__((fastcall)) void fault_handler(interrupts::registers* regs)
{
    interrupts::disable();

    logger::info("Message: {}", ERROR_MESSAGES[regs->irq_id]);
    logger::info("eax: {x}", regs->eax);
    logger::info("ecx: {x}", regs->ecx);
    logger::info("edx: {x}", regs->edx);
    logger::info("ebx: {x}", regs->ebx);
    logger::info("esp: {x}", regs->esp);
    logger::info("ebp: {x}", regs->ebp);
    logger::info("esi: {x}", regs->esi);
    logger::info("edi: {x}", regs->edi);
    logger::info("eip: {x}", regs->eip);

    terminal::clear_screen();
    terminal::set_foreground_color(terminal::color::light_red);
    terminal::print_line("Avada Kedavra has been casted!");
    terminal::print_line("------------------------------");

    terminal::set_foreground_color(terminal::color::white);
    terminal::print_line("Message: {}", ERROR_MESSAGES[regs->irq_id]);

    terminal::print_line("");

    terminal::print_line("eax: {x}", regs->eax);
    terminal::print_line("ecx: {x}", regs->ecx);
    terminal::print_line("edx: {x}", regs->edx);
    terminal::print_line("ebx: {x}", regs->ebx);
    terminal::print_line("esp: {x}", regs->esp);
    terminal::print_line("ebp: {x}", regs->ebp);
    terminal::print_line("esi: {x}", regs->esi);
    terminal::print_line("edi: {x}", regs->edi);
    terminal::print_line("eip: {x}", regs->eip);

    scheduler::halt();
}

// use fastcall to get regs in eax register to pass pointer not the value (avoid coping)
extern "C" __attribute__((fastcall)) void pic_handler(interrupts::registers* regs)
{
    if (regs->irq_id >= 8)
    {
        port::out_byte(PIC2, 0x20);
    }

    port::out_byte(PIC1, 0x20);

    interrupts_enabled = false;

    auto callback = isr_callbacks[regs->irq_id];

    if (callback != nullptr)
    {
        callback(regs);
    }

    interrupts_enabled = true;
}

void interrupts::init()
{
    remap_pic();

    setup_fault();
    setup_pic_interrupts();

    load_idtp();

    logger::info("Interrupts initialized");
}

void interrupts::late_init()
{
    interrupts_lock = false;
}

void interrupts::register_isr_callback(int irq_id, interrupts::callback_t callback)
{
    if (irq_id < 16)
    {
        isr_callbacks[irq_id] = callback;
    }
}

void interrupts::enable()
{
    if (!interrupts_lock)
    {
        interrupts_enabled = true;
        asm("sti");
    }
}

void interrupts::disable()
{
    if (!interrupts_lock)
    {
        interrupts_enabled = false;
        asm("cli");
    }
}

bool interrupts::enabled()
{
    return interrupts_enabled;
}