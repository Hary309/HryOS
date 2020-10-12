#include "terminal/color.hpp"
#include "terminal/terminal.hpp"

#include "multiboot.h"

extern "C" void kernel_main()
{
    terminal::init();

    terminal::print("Hry");
    terminal::set_foreground_color(terminal::color::cyan);
    terminal::print_line("OS!");
}
