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
    uint32_t time = pit::get_timer();
    int counter = 0;

    while (1)
    {
        if (pit::get_timer() - time > 1000)
        {
            logger::info("jeden {}", counter++);
            time = pit::get_timer();
        }
    }
}

void task2()
{
    uint32_t time = pit::get_timer();
    int counter = 0;

    while (1)
    {
        if (pit::get_timer() - time > 1000)
        {
            logger::info("dwa {}", counter++);
            time = pit::get_timer();
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

    scheduler::create_task(task1);
    scheduler::create_task(task2);

    scheduler::init();
}
