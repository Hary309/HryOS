#include "scheduler.hpp"

#include <stdint.h>

#include <algorithm.hpp>
#include <circular_buffer.hpp>

#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "terminal/command_line.hpp"

#include "assert.hpp"
#include "boot.hpp"
#include "process.hpp"

using namespace scheduler;

const auto PROCESS_COUNT = 8;

static bool is_enabled = false;

static process processes[PROCESS_COUNT];

static hlib::circular_buffer<process*, PROCESS_COUNT> ready_queue;

static process* current_process = nullptr;

process* get_free_process()
{
    for (int i = 0; i < PROCESS_COUNT; i++)
    {
        auto& p = processes[i];

        if (p.state == process::state::empty)
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

    auto optional_p = ready_queue.pop();

    if (!optional_p.has_value())
    {
        return;
    }

    auto p = optional_p.value();

    if (p == current_process)
    {
        return;
    }

    if (current_process)
    {
        save_registers(current_process, regs);
        current_process->state = process::state::ready;
        ready_queue.push(current_process);
    }

    switch_process(p);
}

void process_starter()
{
    HRY_ASSERT(current_process != nullptr, "current_process is nullptr");
    HRY_ASSERT(
        current_process->state == scheduler::process::state::running,
        "current_process is not running");
    HRY_ASSERT(current_process != nullptr, "current_process::task is nullptr");

    logger::info("Starting process {}", current_process->pid);

    int result = current_process->task();

    logger::info("Process {} ended with result {}", current_process->pid, result);

    scheduler::idle();
}

void scheduler::create_process(process::function_t* task)
{
    logger::info("Creating process...");

    process* p = get_free_process();

    if (p == nullptr)
    {
        logger::warning("Cannot create process!");
        return;
    }

    hlib::fill_n(p->stack, STACK_SIZE, 0);

    p->pid = static_cast<int32_t>(p - processes);
    p->state = process::state::ready;
    p->task = task;

    p->registers.edi = 0;
    p->registers.esi = 0;
    p->registers.ebp = 0;
    p->registers.esp = reinterpret_cast<uint32_t>(p->stack + STACK_SIZE);
    p->registers.ebx = 0;
    p->registers.edx = 0;
    p->registers.ecx = 0;
    p->registers.eax = 0;

    p->registers.cs = gdt::KERNEL_CODE_SELECTOR;
    p->registers.eflags = 0x200;
    p->registers.eip = reinterpret_cast<uint32_t>(process_starter);

    logger::info(
        "Added task {} stack: {x} - {x}", p->pid, reinterpret_cast<uint32_t>(&p->stack),
        p->registers.esp);

    ready_queue.push(p);
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

void scheduler::halt()
{
    interrupts::disable();
    scheduler::idle();
}
