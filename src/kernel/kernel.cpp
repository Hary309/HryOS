#include <stdint.h>

#include "terminal/color.hpp"
#include "terminal/terminal.hpp"

#include "multiboot.h"

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
}
