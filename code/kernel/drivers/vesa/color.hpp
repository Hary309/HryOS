#pragma once

#include <stdint.h>

namespace vesa
{
    struct color
    {
        uint8_t b = 0;
        uint8_t g = 0;
        uint8_t r = 0;
        uint8_t a = 0;

        static color create(uint8_t red, uint8_t green, uint8_t blue)
        {
            return { blue, green, red };
        }
    };
} // namespace vesa
