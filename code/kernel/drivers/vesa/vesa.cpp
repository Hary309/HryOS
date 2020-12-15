#include "vesa.hpp"

#include "logger/logger.hpp"
#include "math/vec2.hpp"

#include "utility.hpp"

static bool initialized = false;

static vesa::color* display;

static vec2u screen_size;

bool vesa::init(multiboot_info* mbi)
{
    if (!hlib::get_bit(mbi->flags, 12))
    {
        logger::error("VESA is not available!");
        return false;
    }

    display = reinterpret_cast<vesa::color*>(&mbi->framebuffer_addr);

    screen_size = { mbi->framebuffer_width, mbi->framebuffer_height };

    initialized = true;

    return true;
}

void vesa::set_pixeL(const vec2u& pos, color color)
{
    if (pos < screen_size)
    {
        *(display + (pos.y * screen_size.x) + pos.x) = color;
    }
}

const vec2u& vesa::get_screen_size()
{
    return screen_size;
}
