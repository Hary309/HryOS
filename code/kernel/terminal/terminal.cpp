#include "terminal.hpp"

#include <stddef.h>
#include <stdint.h>

#include <algorithm.hpp>

#include "drivers/vga/color.hpp"
#include "drivers/vga/vga.hpp"
#include "logger/logger.hpp"

#include "port_utils.hpp"

namespace terminal
{
    static vec2u cursor_pos;

    static vga::combined_color current_color;

    void init()
    {
        cursor_pos = { 0, 0 };

        current_color.background = vga::color::black;
        current_color.foreground = vga::color::white;

        logger::info("Terminal initialized");
    }

    void scroll_down()
    {
        auto* display = vga::get_display();

        hlib::copy_n(
            display + vga::DISPLAY_SIZE.x, vga::DISPLAY_SIZE.x * (vga::DISPLAY_SIZE.y - 1),
            display);

        for (uint32_t i = 0; i < vga::DISPLAY_SIZE.x; i++)
        {
            vga::get_entry({ i, vga::DISPLAY_SIZE.y - 1 })->reset();
        }

        cursor_pos.y--;
    }

    void next_line()
    {
        cursor_pos.x = 0;

        cursor_pos.y++;

        if (cursor_pos.y == vga::DISPLAY_SIZE.y)
        {
            scroll_down();
        }

        vga::set_cursor_pos(cursor_pos);
    }

    void put_char_at(char ch, const vec2u& pos)
    {
        auto* entry = vga::get_entry(pos);
        entry->set_character(ch);
        entry->set_color(current_color);
    }

    void put_char(char ch)
    {
        put_char_at(ch, cursor_pos);
        cursor_pos.x++;

        if (cursor_pos.x >= vga::DISPLAY_SIZE.x)
        {
            next_line();
        }

        vga::set_cursor_pos(cursor_pos);
    }

    void clear_screen()
    {
        for (uint32_t y = 0; y < vga::DISPLAY_SIZE.y; y++)
        {
            for (uint32_t x = 0; x < vga::DISPLAY_SIZE.x; x++)
            {
                put_char_at(' ', { x, y });
            }
        }

        move_cursor({ 0, 0 });
    }

    void move_cursor(const vec2u& pos)
    {
        cursor_pos = pos;
        vga::set_cursor_pos(cursor_pos);
    }

    const vec2u& get_cursor_pos()
    {
        return cursor_pos;
    }

    void set_color(vga::color foreground, vga::color background)
    {
        set_foreground_color(foreground);
        set_background_color(background);
    }

    void set_foreground_color(vga::color color)
    {
        current_color.foreground = color;
    }

    void set_background_color(vga::color color)
    {
        current_color.background = color;
    }
} // namespace terminal
