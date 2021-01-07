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
#include "int_lock.hpp"
#include "process.hpp"

using namespace scheduler;

const auto PROCESS_COUNT = 8;

static bool is_enabled = false;

static hlib::array<process, PROCESS_COUNT> processes;

static hlib::circular_buffer<process*, PROCESS_COUNT> ready_queue;

process* current_process = nullptr;

extern "C" void switch_to_task(process* next);

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

void scheduler::reschedule()
{
    direct_int_lock lock;

    if (!is_enabled)
    {
        return;
    }

    // update sleeping processes
    for (auto& p : processes)
    {
        if (p.state == process::state::sleeping &&
            pit::get_timer() > p.state_data.get<process::sleep_data>().value().sleep_timeout)
        {
            p.state = process::state::ready;
            ready_queue.push_front(&p);
        }
    }

    if (ready_queue.empty())
    {
        return;
    }

    auto p = ready_queue.pop().value();

    if (p == current_process)
    {
        return;
    }

    if (current_process != nullptr)
    {
        if (current_process->state == process::state::running)
        {
            current_process->state = process::state::ready;
            ready_queue.push_back(current_process);
        }
    }

    if (p->state == process::state::ready)
    {
        p->state = process::state::running;

        switch_to_task(p);
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

    interrupts::enable();

    int result = current_process->task();

    logger::info("Process {} ended with result {}", current_process->pid, result);

    for (auto& p : processes)
    {
        if (p.state == process::state::waiting &&
            p.state_data.get<process::waiting_data>().value().pid == current_process->pid)
        {
            p.state = process::state::ready;
            ready_queue.push_back(&p);
        }
    }

    current_process->state = scheduler::process::state::empty;
    current_process = nullptr;

    scheduler::idle();
}

struct stack_prepare
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t return_addr;
};

hlib::optional<pid_t> scheduler::create_process(const char* name, process::function_t* task)
{
    logger::info("Creating process {}...", name);

    process* p = get_free_process();

    if (p == nullptr)
    {
        logger::warning("Cannot create process!");
        return {};
    }

    hlib::fill_n(p->stack, STACK_SIZE, 0);
    hlib::copy_n(name, 16, p->name);

    p->pid = static_cast<int32_t>(p - processes.begin());
    p->state = process::state::ready;
    p->task = task;
    p->start_time = pit::get_timer();

    p->stack_pointer = reinterpret_cast<uint32_t>(p->stack + STACK_SIZE - sizeof(stack_prepare));

    auto* prepare = reinterpret_cast<stack_prepare*>(p->stack_pointer);

    prepare->edi = 0;
    prepare->esi = 0;
    prepare->ebp = 0;
    prepare->ebx = 0;
    prepare->edx = 0;
    prepare->ecx = 0;
    prepare->eax = 0;

    prepare->return_addr = reinterpret_cast<uint32_t>(process_starter);

    logger::info(
        "Added task {} stack: {x} - {x}", p->pid, reinterpret_cast<uint32_t>(&p->stack),
        p->stack_pointer);

    ready_queue.push_back(p);

    return p->pid;
}

void scheduler::terminate_process(pid_t /*pid*/)
{
}

process* scheduler::get_current_process()
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

int idle_process()
{
    while (true)
    {
        asm("hlt");
    }
}

void scheduler::init()
{
    is_enabled = true;

    command_line::register_command("ps", list_process);

    create_process("idle", idle_process);

    logger::info("Scheduler initialized");
}

void scheduler::sleep_ms(uint32_t time)
{
    if (current_process != nullptr)
    {
        current_process->state = process::state::sleeping;
        current_process->state_data = process::sleep_data{ pit::get_timer() + time };

        logger::info("Process {} sleep {}ms", current_process->pid, time);
    }
}

void scheduler::wait_for(pid_t pid)
{
    if (current_process != nullptr)
    {
        current_process->state = process::state::waiting;
        current_process->state_data = process::waiting_data{ pid };

        logger::info("Process {} waiting for {}", current_process->pid, pid);
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
