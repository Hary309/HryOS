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

    void raw_print(const char* msg);

    inline void print(const char* msg)
    {
        raw_print(msg);        
    }

    void move_cursor(vec2u pos);

    void set_foreground_color(color color);
    void set_background_color(color color);
}
