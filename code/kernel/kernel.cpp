#include <stddef.h>
#include <stdint.h>

#include <format.hpp>

#include "concurrency/lock_guard.hpp"
#include "concurrency/mutex.hpp"
#include "drivers/keyboard.hpp"
#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "math/vec2.hpp"
#include "memory/gdt.hpp"
#include "scheduler/scheduler.hpp"
#include "terminal/color.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "multiboot.h"
#include "sys_calls.hpp"

static int task_count = 0;
static mutex terminal_mutex;

void shutdown_callback()
{
    terminal::print_line("Shutting down...");
}

void task_base(const vec2u& pos)
{
    int counter = 0;

    while (1)
    {
        terminal_mutex.spinlock();

        auto cursor_pos = terminal::get_cursor_pos();

        terminal::move_cursor(pos);
        terminal::print("{} sec", counter++);

        terminal::move_cursor(cursor_pos);

        terminal_mutex.unlock();

        sys_calls::sleep_ms(1000);
    }
}

int task()
{
    task_base({ static_cast<uint32_t>(10 + 10 * (task_count++ % 7)), 0 });

    return 0;
}

int spanko()
{
    terminal_mutex.spinlock();

    terminal::print_line("Mam spanko");
    sys_calls::sleep_ms(2000);
    terminal::print_line("No i juz");

    terminal_mutex.unlock();

    return 1;
}

extern "C" void kernel_main(uint32_t magic, multiboot_info* /*info*/)
{
    logger::init();

    gdt::init();

    interrupts::init();

    keyboard::init();

    pit::init();

    terminal::init();
    terminal::clear_screen();

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        terminal::print_line("Invalid magic number [0x{x}]", magic);
        return;
    }

    terminal::print("Hry");
    terminal::set_foreground_color(terminal::color::cyan);
    terminal::print_line("OS!");
    terminal::set_foreground_color(terminal::color::white);

    command_line::init();

    command_line::register_command("shutdown", shutdown_callback);
    command_line::register_command("crash", [] { int a = 213 / 0; });

    command_line::register_command("rt", [] { scheduler::create_process("timer", task); });
    command_line::register_command("sleep", [] { scheduler::create_process("sleep", spanko); });

    command_line::register_command("deep_sleep", [] {
        auto pid = scheduler::create_process("deep_sleep", spanko);
        sys_calls::wait_for(pid.value());
    });

    scheduler::init();
    interrupts::enable();

    scheduler::idle();
}
