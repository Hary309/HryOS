#include "scheduler.hpp"

#include <stdint.h>

#include "interrupts/interrupts.hpp"
#include "memory/gdt.hpp"
#include "terminal/command_line.hpp"

#include "algorithm.hpp"

static const int STACK_SIZE = 8 * 1024;

enum class status
{
    waiting,
    running,
    blocked
};

struct registers
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

struct process
{
    char stack[STACK_SIZE]{};
    ::registers registers;
    ::status status = ::status::blocked;
};

static bool is_enabled = false;

static process processes[4]{};
static int processes_size = 0;

static process* current_process = nullptr;

process* get_next_process()
{
    for (int i = 0; i < processes_size; i++)
    {
        auto& p = processes[i];

        if (p.status == status::waiting)
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
    p->registers.esp = regs->esp;
    p->registers.ebx = regs->ebx;
    p->registers.edx = regs->edx;
    p->registers.ecx = regs->ecx;
    p->registers.eax = regs->eax;

    p->registers.eip = regs->eip;
    p->registers.cs = regs->cs;
    p->registers.eflags = regs->eflags;
}

void switch_process_to(process* p)
{
    p->status = status::running;

    current_process = p;

    asm("push %0" : : "g"(p->registers.esp));
    asm("pop %esp");

    asm("push %0" ::"g"(p->registers.eflags));
    asm("push %0" ::"g"(p->registers.cs));
    asm("push %0" ::"g"(p->registers.eip));

    asm("push %0" ::"g"(p->registers.eax));
    asm("push %0" ::"g"(p->registers.ecx));
    asm("push %0" ::"g"(p->registers.edx));
    asm("push %0" ::"g"(p->registers.ebx));
    asm("push %0" ::"g"(p->registers.esp));
    asm("push %0" ::"g"(p->registers.ebp));
    asm("push %0" ::"g"(p->registers.esi));
    asm("push %0" ::"g"(p->registers.edi));

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

    if (current_process)
    {
        save_registers(current_process, regs);
        current_process->status = status::waiting;
    }

    if (p == current_process || p == nullptr)
    {
        return;
    }

    switch_process_to(p);
}

void scheduler::create_task(scheduler::task_t* task)
{
    process& p = processes[processes_size];

    hlib::fill_n(p.stack, STACK_SIZE, 0);

    p.status = status::waiting;

    p.registers.edi = 0;
    p.registers.esi = 0;
    p.registers.ebp = 0;
    p.registers.esp = reinterpret_cast<uint32_t>(&p.stack[STACK_SIZE - 8]);
    p.registers.ebx = 0;
    p.registers.edx = 0;
    p.registers.ecx = 0;
    p.registers.eax = 0;

    p.registers.cs = gdt::KERNEL_CODE_SELECTOR;
    p.registers.eflags = 0x200;
    p.registers.eip = reinterpret_cast<uint32_t>(task);

    ++processes_size;
}

void scheduler::init()
{
    is_enabled = true;
}
