#include "terminal.hpp"

#include <stdint.h>

#include "utils/string.hpp"

#include "cursor.hpp"
#include "entry.hpp"

namespace terminal
{
    static constexpr auto DisplayAddress = 0xB8000;

    static vec2u cursor_pos;
    static cursor cursor;

    static combined_color current_color;

    void init()
    {
        cursor = { DisplayAddress, { Width, Height } };
        cursor_pos = { 0, 0 };

        current_color.background = color::black;
        current_color.foreground = color::white;
    }

    void put_char_at(char ch, vec2u pos)
    {
        auto* entry = cursor.get_entry(pos);
        entry->set_character(ch);
        entry->set_color(current_color);
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
                put_char_at(' ', { x, y });
            }
        }
    }

    void print(const char* msg)
    {
        while (*msg)
        {
            put_char(*msg);
            msg++;
        }
    }

    void print_line(const char* msg)
    {
        print(msg);
        cursor_pos.x = 0;
        cursor_pos.y++;

        cursor_pos.y %= Height;
    }

    void move_cursor(vec2u pos) { cursor_pos = pos; }

    void set_foreground_color(color color) { current_color.foreground = color; }
    void set_background_color(color color) { current_color.background = color; }
} // namespace terminal
