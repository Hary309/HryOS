#include "vesa.hpp"

#include "drivers/vesa/color.hpp"
#include "logger/logger.hpp"
#include "math/vec2.hpp"

#include "utility.hpp"

static bool initialized = false;

static vesa::color* display;

static uint8_t bpp;

static uint32_t pitch;
static vec2u screen_size;

bool vesa::init(multiboot_info* mbi)
{
    if (!hlib::get_bit(mbi->flags, 12))
    {
        logger::error("VESA is not available!");
        return false;
    }

    display = reinterpret_cast<vesa::color*>(mbi->framebuffer_addr);

    screen_size = { mbi->framebuffer_width, mbi->framebuffer_height };
    pitch = mbi->framebuffer_pitch;
    bpp = mbi->framebuffer_bpp;

    logger::info("[vesa] Screen: {x}", mbi->framebuffer_addr);
    logger::info("[vesa] Size: {}x{}", screen_size.x, screen_size.y);
    logger::info("[vesa] Pitch: {}", pitch);
    logger::info("[vesa] Bits per pixel: {}", bpp);

    for (size_t y = 100; y < 355; y++)
    {
        for (size_t x = 200; x < 500; x++)
        {
            set_pixeL({ x, y }, { y - 100, y - 100, y - 100 });
        }
    }

    initialized = true;

    logger::info("[vesa] Initialized");

    return true;
}

void vesa::set_pixeL(const vec2u& pos, color color)
{
    if (pos < screen_size)
    {
        auto* pixel = display + pos.y * screen_size.x + pos.x;
        *reinterpret_cast<vesa::color*>(pixel) = color;
    }
}

const vec2u& vesa::get_screen_size()
{
    return screen_size;
}
