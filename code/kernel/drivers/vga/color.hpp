#pragma once

#include <stdint.h>

namespace vga
{
    enum class color
    {
        black = 0,
        blue = 1,
        green = 2,
        cyan = 3,
        red = 4,
        magenta = 5,
        brown = 6,
        light_grey = 7,
        dark_grey = 8,
        light_blue = 9,
        light_green = 10,
        light_cyan = 11,
        light_red = 12,
        light_magenta = 13,
        light_brown = 14,
        white = 15,
    };

    struct combined_color
    {
        color foreground;
        color background;

        uint8_t value()
        {
            return static_cast<uint8_t>(foreground) | static_cast<uint8_t>(background) << 4;
        }
    };
} // namespace vga
