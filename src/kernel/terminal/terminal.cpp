#include "terminal.hpp"

#include <stdint.h>

#include "utils/memory.hpp"

#include "entry.hpp"

namespace terminal
{
    static constexpr auto vga_display_address = 0xB8000;
    static constexpr vec2u vga_display_size = { 80, 25 };

    static vec2u cursor_pos;
    entry* display;

    static combined_color current_color;

    void init()
    {
        display = reinterpret_cast<entry*>(vga_display_address);
        cursor_pos = { 0, 0 };

        current_color.background = color::black;
        current_color.foreground = color::white;
    }

    entry* get_entry(const vec2u& pos)
    {
        if (pos.x < vga_display_size.x && pos.y < vga_display_size.y)
        {
            return display + pos.y * vga_display_size.x + pos.x;
        }

        return nullptr;
    }

    void scroll_down()
    {
        for (size_t y = 0; y < vga_display_size.y - 1; y++)
        {
            for (size_t x = 0; x < vga_display_size.x; x++)
            {
                *get_entry({ x, y }) = *get_entry({ x, y + 1 });
            }
        }

        for (size_t i = 0; i < vga_display_size.x; i++)
        {
            get_entry({ i, vga_display_size.y - 1 })->reset();
        }

        cursor_pos.y--;
    }

    void next_line()
    {
        cursor_pos.y++;

        if (cursor_pos.y == vga_display_size.y)
        {
            scroll_down();
        }
    }

    void put_char_at(char ch, const vec2u& pos)
    {
        auto* entry = get_entry(pos);
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
            next_line();
        }
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

        next_line();
    }

    void move_cursor(vec2u pos)
    {
        cursor_pos = pos;
    }

    void set_color(color foreground, color background)
    {
        set_foreground_color(foreground);
        set_background_color(background);
    }

    void set_foreground_color(color color)
    {
        current_color.foreground = color;
    }

    void set_background_color(color color)
    {
        current_color.background = color;
    }
} // namespace terminal
