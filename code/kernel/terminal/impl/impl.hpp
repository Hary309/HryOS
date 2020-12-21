#pragma once

#include <stdint.h>

#include "math/vec2.hpp"
#include "terminal/color.hpp"

namespace terminal::impl
{
    void init();

    void clear_screen();

    void put_char_at(char ch, const vec2u& pos, const combined_color& color);

    uint32_t get_width();
    uint32_t get_height();

    void set_cursor_at(const vec2u& pos);

    void scroll_down();
} // namespace terminal::impl
