#include "scheduler.hpp"

#include <stdint.h>

#include <algorithm.hpp>

#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "terminal/command_line.hpp"

#include "process.hpp"

using namespace scheduler;

static bool is_enabled = false;

static process processes[4];
static int processes_size = 0;

static process* current_process = nullptr;

process* get_next_process()
{
    for (int i = 0; i < processes_size; i++)
    {
        auto& p = processes[i];

        if (p.state == process::state::ready)
        {
            return &p;
        }
    }

    return nullptr;
}

void save_registers(process* p, interrupts::registers* regs)
{
    p->registers.edi = regs->edi;
    p->registers.esi = regs->esi;
    p->registers.ebp = regs->ebp;
    // 16 because of these pushed on stack before pushad: irq id, eip, cs, eflags (4 * 4 bytes)
    p->registers.esp = regs->esp + 16;
    p->registers.ebx = regs->ebx;
    p->registers.edx = regs->edx;
    p->registers.ecx = regs->ecx;
    p->registers.eax = regs->eax;

    p->registers.eip = regs->eip;
    p->registers.cs = regs->cs;
    p->registers.eflags = regs->eflags;
}

void switch_process(process* p)
{
    p->state = process::process::state::running;

    current_process = p;

    asm("push %0" : : "g"(p->registers.esp));
    asm("pop %esp");

    asm("push %0" : : "g"(p->registers.eflags));
    asm("push %0" : : "g"(p->registers.cs));
    asm("push %0" : : "g"(p->registers.eip));

    asm("push %0" : : "g"(p->registers.eax));
    asm("push %0" : : "g"(p->registers.ecx));
    asm("push %0" : : "g"(p->registers.edx));
    asm("push %0" : : "g"(p->registers.ebx));
    asm("push %0" : : "g"(p->registers.esp));
    asm("push %0" : : "g"(p->registers.ebp));
    asm("push %0" : : "g"(p->registers.esi));
    asm("push %0" : : "g"(p->registers.edi));

    asm("popa");

    asm("iret");
}

void scheduler::tick(interrupts::registers* regs)
{
    if (!is_enabled)
    {
        return;
    }

    auto p = get_next_process();

    if (p == current_process || p == nullptr)
    {
        return;
    }

    if (current_process)
    {
        save_registers(current_process, regs);
        current_process->state = process::state::ready;
    }

    switch_process(p);
}

void process_end()
{
    logger::info("process end!");
    // auto& p = scheduler::current_process();

    // if (p.state == process::state::running)
    // {
    //     p.state =
    // }
}

void scheduler::create_process(scheduler::process_function_t* task)
{
    process& p = processes[processes_size];

    hlib::fill_n(p.stack, STACK_SIZE, 0);

    p.stack[STACK_SIZE - 8] = reinterpret_cast<uint32_t>(process_end);

    p.state = process::state::ready;

    p.registers.edi = 0;
    p.registers.esi = 0;
    p.registers.ebp = 0;
    p.registers.esp = reinterpret_cast<uint32_t>(&p.stack[STACK_SIZE - 12]);
    p.registers.ebx = 0;
    p.registers.edx = 0;
    p.registers.ecx = 0;
    p.registers.eax = 0;

    p.registers.cs = gdt::KERNEL_CODE_SELECTOR;
    p.registers.eflags = 0x200;
    p.registers.eip = reinterpret_cast<uint32_t>(task);

    logger::info(
        "Added task {} stack: {x} - {x}", processes_size, reinterpret_cast<uint32_t>(&p.stack),
        p.registers.esp);

    ++processes_size;
}

void terminate_process(pid_t pid)
{
}

process* get_current_process()
{
    return current_process;
}

void scheduler::init()
{
    is_enabled = true;
}

void scheduler::idle()
{
    asm("push %0" : : "g"(&kernel_stack_top));
    asm("pop %esp");

    while (true)
    {
        asm("hlt");
    }
}
