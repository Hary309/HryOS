#pragma once

#include <stdint.h>

#include <array.hpp>

namespace fonts
{
    struct glyph
    {
        hlib::array<uint8_t, 16> data;
    };

    struct font
    {
        glyph* glyphs;
    };

    inline font ZAP_LIGHT_16{};

    void init();
} // namespace fonts
