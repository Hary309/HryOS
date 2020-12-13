#include <stddef.h>
#include <stdint.h>

#include <format.hpp>

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

void shutdown_callback()
{
    terminal::print_line("Shutting down...");
}

int task_count = 0;

void task_base(const vec2u& pos)
{
    int counter = 0;
    int timer = 0;

    while (1)
    {
        if (pit::get_timer() - timer > 1000)
        {
            auto cursor_pos = terminal::get_cursor_pos();

            terminal::move_cursor(pos);
            terminal::print("{} sec", counter++);

            terminal::move_cursor(cursor_pos);

            timer = pit::get_timer();
        }
    }
}

int task()
{
    task_base({ static_cast<uint32_t>(10 + 10 * (task_count++ % 7)), 0 });

    return 0;
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

    command_line::register_command("run timer", []() { scheduler::create_process(task); });

    scheduler::init();

    scheduler::idle();
}
