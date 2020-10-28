#include "terminal.hpp"

#include <stdint.h>

#include "utils/string.hpp"

#include "display.hpp"
#include "entry.hpp"

namespace terminal
{
    static constexpr auto vga_display_address = 0xB8000;
    static constexpr vec2u vga_display_size = { 80, 25 };

    static vec2u cursor_pos;
    static display display;

    static combined_color current_color;

    void init()
    {
        display = { vga_display_address, vga_display_size };
        cursor_pos = { 0, 0 };

        current_color.background = color::black;
        current_color.foreground = color::white;
    }

    void put_char_at(char ch, const vec2u& pos)
    {
        auto* entry = display.get_entry(pos);
        entry->set_character(ch);
        entry->set_color(current_color);
    }

    void put_char(char ch)
    {
        put_char_at(ch, cursor_pos);
        cursor_pos.x++;

        if (cursor_pos.x >= vga_display_size.x)
        {
            cursor_pos.x = 0;
            cursor_pos.y++;
        }

        cursor_pos.y %= vga_display_size.y;
    }

    void clear_screen()
    {
        for (uint32_t y = 0; y < vga_display_size.y; y++)
        {
            for (uint32_t x = 0; x < vga_display_size.x; x++)
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

        cursor_pos.y %= vga_display_size.y;
    }

    void move_cursor(vec2u pos) { cursor_pos = pos; }

    void set_color(color foreground, color background)
    {
        set_foreground_color(foreground);
        set_background_color(background);
    }
    void set_foreground_color(color color) { current_color.foreground = color; }
    void set_background_color(color color) { current_color.background = color; }
} // namespace terminal
