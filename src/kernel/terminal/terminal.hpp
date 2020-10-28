#pragma once

#include "math/vec2.hpp"

#include "color.hpp"

namespace terminal
{
    void init();

    void clear_screen();

    void put_char(char ch);
    void put_char_at(char ch, const vec2u& pos);

    void print(const char* msg);
    void print_line(const char* msg);

    void move_cursor(const vec2u& pos);

    void set_color(color foreground, color background);
    void set_foreground_color(color color);
    void set_background_color(color color);
} // namespace terminal
