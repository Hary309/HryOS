#include "scheduler.hpp"

#include <stdint.h>

#include <algorithm.hpp>
#include <circular_buffer.hpp>

#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "array.hpp"
#include "assert.hpp"
#include "boot.hpp"
#include "process.hpp"

using namespace scheduler;

const auto PROCESS_COUNT = 8;

static bool is_enabled = false;

static hlib::array<process, PROCESS_COUNT> processes;

static hlib::circular_buffer<process*, PROCESS_COUNT> ready_queue;

static process* current_process = nullptr;

process* get_free_process()
{
    for (auto& p : processes)
    {
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
    p->registers.ebx = regs->ebx;
    p->registers.edx = regs->edx;
    p->registers.ecx = regs->ecx;
    p->registers.eax = regs->eax;

    // 16 because: irq id, eip, cs, eflags (4 * 4 bytes)
    p->registers.esp = regs->esp + 16;

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

    // push for iret
    asm("push %0" : : "g"(p->registers.eflags));
    asm("push %0" : : "g"(p->registers.cs));
    asm("push %0" : : "g"(p->registers.eip));

    // push for popa
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

    // update sleeping processes
    for (auto& p : processes)
    {
        if (p.state == process::state::sleeping && pit::get_timer() > p.sleep_timeout)
        {
            p.state = process::state::ready;
            ready_queue.push_front(&p);
        }
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

    if (current_process != nullptr)
    {
        save_registers(current_process, regs);

        if (current_process->state == process::state::running)
        {
            current_process->state = process::state::ready;
            ready_queue.push_back(current_process);
        }
    }

    if (p->state == process::state::ready)
    {
        switch_process(p);
    }
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

    current_process->state = scheduler::process::state::empty;
    current_process = nullptr;

    scheduler::idle();
}

void scheduler::create_process(const char* name, process::function_t* task)
{
    logger::info("Creating process...");

    process* p = get_free_process();

    if (p == nullptr)
    {
        logger::warning("Cannot create process!");
        return;
    }

    hlib::fill_n(p->stack, STACK_SIZE, 0);
    hlib::copy_n(name, 16, p->name);

    p->pid = static_cast<int32_t>(p - processes.begin());
    p->state = process::state::ready;
    p->task = task;
    p->start_time = pit::get_timer();

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

    ready_queue.push_back(p);
}

void scheduler::terminate_process(pid_t /*pid*/)
{
}

process* get_current_process()
{
    return current_process;
}

void list_process()
{
    terminal::print_line("Processes");
    terminal::print_line("---------------");

    uint32_t timer = pit::get_timer();

    for (const auto& p : processes)
    {
        const auto& pos = terminal::get_cursor_pos();

        if (p.state != scheduler::process::state::empty)
        {
            terminal::print("{} {}", p.pid, p.name);

            terminal::move_cursor({ 20, pos.y });
            terminal::print("{}s", (timer - p.start_time) / 1000);

            terminal::move_cursor({ 30, pos.y });
            terminal::print("{}", state_to_text(p.state));

            terminal::next_line();
        }
    }
}

void scheduler::init()
{
    is_enabled = true;

    command_line::register_command("ps", list_process);
}

void scheduler::sleep_ms(uint32_t time)
{
    if (current_process != nullptr)
    {
        current_process->state = process::state::sleeping;
        current_process->sleep_timeout = pit::get_timer() + time;

        logger::info("Process {} sleep {}ms", current_process->pid, time);
    }
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
