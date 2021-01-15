#include "scheduler.hpp"

#include <stdint.h>

#include <algorithm.hpp>
#include <circular_buffer.hpp>
#include <list.hpp>

#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "memory/kernel_heap.hpp"
#include "scheduler/lock_guard.hpp"
#include "scheduler/mutex.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "array.hpp"
#include "assert.hpp"
#include "boot.hpp"
#include "int_lock.hpp"
#include "process.hpp"

using namespace scheduler;

static bool is_enabled = false;

static hlib::list<process> processes{};

using process_list_t = hlib::list<process*, true>;

static hlib::array<process_list_t, static_cast<int>(process::state::count)> lists;

process* current_process = nullptr;

extern "C" void switch_to_task(process* next);

process_list_t& get_list(enum process::state state)
{
    return lists[static_cast<int>(state)];
}

template<typename PredicateFunc, typename ListMethod>
void move_to_if(
    enum process::state from, enum process::state to, PredicateFunc predicate, ListMethod func)
{
    auto& from_list = get_list(from);
    auto& to_list = get_list(to);

    from_list.erase_if([&predicate, from, to, &to_list, &func](process* p) {
        if (p->state == from && predicate(p))
        {
            p->state = to;
            (to_list.*func)(p);
            return true;
        }

        return false;
    });
}

template<typename PredicateFunc>
void move_to_back_if(enum process::state from, enum process::state to, PredicateFunc predicate)
{
    move_to_if(from, to, predicate, &process_list_t::push_back);
}

template<typename PredicateFunc>
void move_to_front_if(enum process::state from, enum process::state to, PredicateFunc predicate)
{
    move_to_if(from, to, predicate, &process_list_t::push_front);
}

void scheduler::reschedule()
{
    weak_int_lock lock;

    if (!is_enabled)
    {
        return;
    }

    // update sleeping list
    move_to_front_if(process::state::sleeping, process::state::ready, [](process* p) {
        return pit::get_timer() > p->state_data.get<process::sleep_data>().value().sleep_timeout;
    });

    process* p = nullptr;

    auto& ready_list = get_list(process::state::ready);

    if (ready_list.empty())
    {
        return;
    }

    p = ready_list.front();
    ready_list.pop_front(); // remove pointer from ready_list

    if (p == current_process)
    {
        return;
    }

    if (current_process != nullptr)
    {
        if (current_process->state == process::state::running)
        {
            current_process->state = process::state::ready;
            ready_list.push_back(current_process);
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

    logger::info("Starting process '{}'", current_process->name);

    interrupts::enable();

    int result = current_process->task();

    interrupts::disable();

    logger::info("Process '{}' ended with result {}", current_process->name, result);

    // wake up waiting processes
    move_to_back_if(process::state::waiting, process::state::ready, [](process* p) {
        return p->state_data.get<process::waiting_data>().value().pid == current_process->pid;
    });

    current_process->state = scheduler::process::state::empty;
    current_process = nullptr;

    scheduler::reschedule();
}

pid_t scheduler::create_process(const char* name, process::function_t* task)
{
    direct_int_lock int_lock;
    logger::info("Creating process '{}'...", name);

    auto* p = processes.push_back({});

    HRY_ASSERT(p != nullptr, "Cannot create process!");

    hlib::copy_n(name, 16, p->name);

    static int pid_generator = 0;

    p->pid = pid_generator++;
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

    logger::info("Added task '{}'", p->name);

    get_list(process::state::ready).push_back(p);

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

void scheduler::block_current_process(enum process::blocked_data::type type)
{
    if (current_process != nullptr)
    {
        {
            direct_int_lock lock;
            current_process->state = process::state::blocked;
            current_process->state_data = process::blocked_data{ type };

            get_list(process::state::blocked).push_back(current_process);
        }

        reschedule();
    }
}

void scheduler::notify_blocked(enum process::blocked_data::type type)
{
    move_to_front_if(process::state::blocked, process::state::ready, [&type](process* p) {
        return p->state_data.get<process::blocked_data>().value().value == type;
    });
}

void scheduler::sleep_ms(uint32_t time)
{
    if (current_process != nullptr)
    {
        {
            direct_int_lock lock;
            current_process->state = process::state::sleeping;
            current_process->state_data = process::sleep_data{ pit::get_timer() + time };

            get_list(process::state::sleeping).push_back(current_process);

            logger::info(
                "Process {} '{}' sleep {}ms", current_process->pid, current_process->name, time);
        }

        reschedule();
    }
}

void scheduler::wait_for(pid_t pid)
{
    if (current_process != nullptr)
    {
        {
            direct_int_lock lock;
            current_process->state = process::state::waiting;
            current_process->state_data = process::waiting_data{ pid };

            get_list(process::state::waiting).push_back(current_process);

            logger::info(
                "Process {} '{}' waiting for {}", current_process->pid, current_process->name, pid);
        }

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
