#include "scheduler.hpp"

#include <stdint.h>

#include <algorithm.hpp>
#include <circular_buffer.hpp>

#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "memory/kernel_heap.hpp"
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

    interrupts::disable();

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

    scheduler::reschedule();
}

hlib::optional<pid_t> scheduler::create_process(const char* name, process::function_t* task)
{
    direct_int_lock lock;

    logger::info("Creating process {}...", name);

    process* p = get_free_process();

    if (p == nullptr)
    {
        logger::warning("Cannot create process!");
        return {};
    }

    hlib::copy_n(name, 16, p->name);

    p->pid = static_cast<int32_t>(p - processes.begin());
    p->state = process::state::ready;
    p->task = task;
    p->start_time = pit::get_timer();

    p->stack = new hlib::stack(STACK_SIZE);

    p->stack->push(reinterpret_cast<uint32_t>(process_starter));

    p->stack->push<uint32_t>(0); // eax
    p->stack->push<uint32_t>(0); // ecx
    p->stack->push<uint32_t>(0); // edx
    p->stack->push<uint32_t>(0); // ebx
    p->stack->push<uint32_t>(0); // ebp
    p->stack->push<uint32_t>(0); // esi
    p->stack->push<uint32_t>(0); // edi

    logger::info("Added task {}", p->pid);

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
    direct_int_lock lock;

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
            terminal::put_char(' ');
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
    interrupts::disable();

    if (current_process != nullptr)
    {
        current_process->state = process::state::sleeping;
        current_process->state_data = process::sleep_data{ pit::get_timer() + time };

        logger::info("Process {} sleep {}ms", current_process->pid, time);

        reschedule();
    }
}

void scheduler::wait_for(pid_t pid)
{
    interrupts::disable();

    if (current_process != nullptr)
    {
        current_process->state = process::state::waiting;
        current_process->state_data = process::waiting_data{ pid };

        logger::info("Process {} waiting for {}", current_process->pid, pid);

        reschedule();
    }
}

void scheduler::halt()
{
    interrupts::disable();

    while (true)
    {
        asm("hlt");
    }
}
