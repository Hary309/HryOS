#include "terminal.hpp"

#include <stddef.h>
#include <stdint.h>

#include <algorithm.hpp>

#include "logger/logger.hpp"

#include "entry.hpp"
#include "port_utils.hpp"

namespace terminal
{
    static constexpr auto VGA_DISPLAY_ADDRESS = 0xB8000;

    static vec2u cursor_pos;
    static entry* display;

    static combined_color current_color;

    void init()
    {
        display = reinterpret_cast<entry*>(VGA_DISPLAY_ADDRESS);
        cursor_pos = { 0, 0 };

        current_color.background = color::black;
        current_color.foreground = color::white;

        logger::info("Terminal initialized");
    }

    void update_vga_cursor_pos(const vec2u& pos)
    {
        uint16_t flat_pos = pos.y * VGA_DISPLAY_SIZE.x + pos.x;

        port::out_byte(0x3D4, 0x0F);
        port::out_byte(0x3D5, (uint8_t)(flat_pos & 0xFF));
        port::out_byte(0x3D4, 0x0E);
        port::out_byte(0x3D5, (uint8_t)((flat_pos >> 8) & 0xFF));
    }

    constexpr entry* get_entry(const vec2u& pos)
    {
        if (pos.x < VGA_DISPLAY_SIZE.x && pos.y < VGA_DISPLAY_SIZE.y)
        {
            return display + pos.y * VGA_DISPLAY_SIZE.x + pos.x;
        }

        return nullptr;
    }

    void scroll_down()
    {
        hlib::copy_n(
            display + VGA_DISPLAY_SIZE.x, VGA_DISPLAY_SIZE.x * (VGA_DISPLAY_SIZE.y - 1), display);

        for (uint32_t i = 0; i < VGA_DISPLAY_SIZE.x; i++)
        {
            get_entry({ i, VGA_DISPLAY_SIZE.y - 1 })->reset();
        }

        cursor_pos.y--;
    }

    void next_line()
    {
        cursor_pos.x = 0;

        cursor_pos.y++;

        if (cursor_pos.y == VGA_DISPLAY_SIZE.y)
        {
            scroll_down();
        }

        update_vga_cursor_pos(cursor_pos);
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

        if (cursor_pos.x >= VGA_DISPLAY_SIZE.x)
        {
            next_line();
        }

        update_vga_cursor_pos(cursor_pos);
    }

    void clear_screen()
    {
        for (uint32_t y = 0; y < VGA_DISPLAY_SIZE.y; y++)
        {
            for (uint32_t x = 0; x < VGA_DISPLAY_SIZE.x; x++)
            {
                put_char_at(' ', { x, y });
            }
        }

        move_cursor({ 0, 0 });
    }

    void move_cursor(const vec2u& pos)
    {
        cursor_pos = pos;
        update_vga_cursor_pos(cursor_pos);
    }

    const vec2u& get_cursor_pos()
    {
        return cursor_pos;
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
