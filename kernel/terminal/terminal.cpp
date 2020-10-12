#include "terminal.hpp"

#include <stdint.h>

#include "utils/string.hpp"

#include "display_cursor.hpp"

namespace terminal
{
    static constexpr auto DisplayAddress = 0xB8000;

    static vec2u cursor_pos;
    static combined_color current_color;
    static uint16_t* buffer;

    uint16_t vga_entry(unsigned char ch, uint8_t color) 
    {
        return static_cast<uint16_t>(ch) | static_cast<uint16_t>(color << 8);
    }

    uint32_t get_index(vec2u pos)
    {
        return pos.y * Width + pos.x;
    }

    void init()
    {
        cursor_pos = { 0, 0 };
        buffer = reinterpret_cast<uint16_t*>(Display);

        current_color.background = color::black;
        current_color.foreground = color::white;
    }

    void put_char_at(char ch, vec2u pos)
    {
        buffer[get_index(pos)] = vga_entry(ch, current_color.value());
    }

    void put_char(char ch)
    {
        put_char_at(ch, cursor_pos);
        cursor_pos.x++;

        if (cursor_pos.x >= Width)
        {
            cursor_pos.x = 0;
            cursor_pos.y++;
        }

        cursor_pos.y %= Height;
    }

    void clear_screen()
    {
        for (uint32_t y = 0; y < Height; y++)
        {
            for (uint32_t x = 0; x < Width; x++)
            {
                put_char_at(' ', {x, y});
            }
        }
    }

    void raw_print(const char* msg)
    {
        while (*msg)
        {
            put_char(*msg);
            msg++;
        }
    }

    void move_cursor(vec2u pos)
    {
        cursor_pos = pos;
    }

    void set_foreground_color(color color)
    {
        current_color.foreground = color;
    }

    void set_background_color(color color)
    {
        current_color.background = color;
    }

}
