#include <stddef.h>
#include <stdint.h>

#include <format.hpp>

#include "terminal/color.hpp"
#include "terminal/terminal.hpp"

#include "multiboot.h"

void print(char ch)
{
    terminal::put_char(ch);
}

extern "C" void kernel_main(uint32_t magic, multiboot_info* info)
{
    terminal::init();
    terminal::clear_screen();

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        terminal::print_line("Invalid magic number");
        return;
    }

    terminal::print("Hry");
    terminal::set_foreground_color(terminal::color::cyan);
    terminal::print_line("OS!");

    terminal::set_foreground_color(terminal::color::white);

    hlib::format_to(print, "{x} is {x}", 48879, 57005);
}
