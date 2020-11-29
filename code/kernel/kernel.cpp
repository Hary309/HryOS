#include <stddef.h>
#include <stdint.h>

#include <format.hpp>

#include "drivers/keyboard.hpp"
#include "drivers/pit.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "terminal/color.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "multiboot.h"
#include "scheduler.hpp"

void shutdown_callback()
{
    terminal::print_line("Shutting down...");
}

void task1()
{
    int counter = 100;
    int timer = 0;

    while (1)
    {
        if (pit::get_timer() - timer > 2)
        {
            auto cursor_pos = terminal::get_cursor_pos();

            terminal::move_cursor({ 0, 10 });
            terminal::print("{}", counter++);

            terminal::move_cursor(cursor_pos);

            timer = pit::get_timer();
        }
    }
}

void task2()
{
    int counter = 0;
    int timer = 0;

    while (1)
    {
        if (pit::get_timer() - timer > 2)
        {
            auto cursor_pos = terminal::get_cursor_pos();

            terminal::move_cursor({ terminal::VGA_DISPLAY_SIZE.x / 2, 10 });
            terminal::print("{}", counter++);

            terminal::move_cursor(cursor_pos);

            timer = pit::get_timer();
        }
    }
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

    command_line::register_command("run task1", []() { scheduler::create_task(task1); });
    command_line::register_command("run task2", []() { scheduler::create_task(task2); });

    scheduler::init();
}
