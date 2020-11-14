#include <stddef.h>
#include <stdint.h>

#include <format.hpp>

#include "drivers/keyboard.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "memory/gdt.hpp"
#include "terminal/color.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "multiboot.h"

void shutdown_callback()
{
    terminal::print_line("Shutting down...");
}

extern "C" void kernel_main(uint32_t magic, multiboot_info* /*info*/)
{
    logger::init();

    gdt::init();

    interrupts::init();

    keyboard::init();

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
}
