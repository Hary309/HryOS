#include "vesa.hpp"

#include <binary.hpp>

#include "drivers/vesa/color.hpp"
#include "logger/logger.hpp"
#include "math/vec2.hpp"

#include "assert.hpp"

static bool initialized = false;

static vesa::color* display;

static uint8_t bpp;

static uint32_t pitch;
static vec2u screen_size;

bool vesa::init(multiboot_info* mbi)
{
    HRY_ASSERT(hlib::get_bit(mbi->flags, 12), "VESA is not available!");

    display = reinterpret_cast<vesa::color*>(mbi->framebuffer_addr);

    screen_size = { mbi->framebuffer_width, mbi->framebuffer_height };
    pitch = mbi->framebuffer_pitch;
    bpp = mbi->framebuffer_bpp;

    logger::info("VESA info:");
    logger::info("- Screen: {x}", mbi->framebuffer_addr);
    logger::info("- Size: {}x{}", screen_size.x, screen_size.y);
    logger::info("- Pitch: {}", pitch);
    logger::info("- Bits per pixel: {}", bpp);

    initialized = true;

    logger::info("VESA driver initialized");

    return true;
}

void vesa::set_pixeL(const vec2u& pos, color color)
{
    if (initialized && pos < screen_size)
    {
        auto* pixel = display + pos.y * screen_size.x + pos.x;
        *reinterpret_cast<vesa::color*>(pixel) = color;
    }
}

const vec2u& vesa::get_screen_size()
{
    return screen_size;
}

vesa::color* vesa::get_display()
{
    return display;
}
