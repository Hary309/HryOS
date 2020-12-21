#pragma once

#include <stdint.h>

#include <array.hpp>

#include "math/vec2.hpp"

namespace fonts
{
    struct glyph
    {
        hlib::array<uint8_t, 16> data;
    };

    struct font
    {
        glyph* glyphs;
        vec2u size;
    };

    inline font ZAP_LIGHT_16{};

    void init();
} // namespace fonts
