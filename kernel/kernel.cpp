#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"

#include "terminal/color.hpp"
#include "terminal/terminal.hpp"
 

extern "C" void kernel_main() 
{
	terminal::init();

	terminal::print("Hry");

	terminal::set_foreground_color(terminal::color::cyan);
	terminal::print("OS!");
}
