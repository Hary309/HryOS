#pragma once

#include "math/vec2.hpp"

#include "color.hpp"

namespace terminal
{
    inline static auto Width = 80u;
    inline static auto Height = 25u;

    void init();

    void clear_screen();

    void put_char(char ch);
    void put_char_at(char ch, vec2u pos);

    void print(const char* msg);
    void print_line(const char* msg);

    void move_cursor(vec2u pos);

    void set_foreground_color(color color);
    void set_background_color(color color);
} // namespace terminal
