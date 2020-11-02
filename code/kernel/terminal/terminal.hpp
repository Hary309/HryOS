#pragma once

#include <format.hpp>

#include "math/vec2.hpp"

#include "color.hpp"
#include "utility.hpp"

namespace terminal
{
    void init();

    void clear_screen();

    void put_char(char ch);
    void put_char_at(char ch, const vec2u& pos);

    void next_line();

    template<typename... Args>
    void print(const char* fmt, Args&&... args)
    {
        hlib::format_to(put_char, fmt, hlib::forward<Args>(args)...);
    }

    template<typename... Args>
    void print_line(const char* fmt, Args&&... args)
    {
        hlib::format_to(put_char, fmt, hlib::forward<Args>(args)...);
        next_line();
    }

    void move_cursor(const vec2u& pos);

    void set_color(color foreground, color background);
    void set_foreground_color(color color);
    void set_background_color(color color);
} // namespace terminal
