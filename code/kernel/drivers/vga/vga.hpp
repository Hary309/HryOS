#pragma once

#include "drivers/vga/color.hpp"
#include "math/vec2.hpp"

#include "entry.hpp"

namespace vga
{
    static constexpr vec2u DISPLAY_SIZE = { 80, 25 };

    void init();

    entry* get_entry(const vec2u& pos);

    void set_entry(const vec2u& pos, char ch, combined_color color);
    void set_entry(const vec2u& pos, char ch, color fg, color bg);

    void set_cursor_pos(const vec2u& pos);

    entry* get_display();
} // namespace vga
