#include <stddef.h>
#include <stdint.h>

#include "terminal/color.hpp"
#include "terminal/terminal.hpp"

#include "charconv.hpp"
#include "multiboot.h"
#include "numerics.hpp"

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

    char buffer[20]{};

    hlib::to_chars(buffer, buffer + 19, -1234.5678f, 2);

    terminal::print_line(buffer);
}
