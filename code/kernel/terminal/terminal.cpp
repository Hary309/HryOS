#include "terminal.hpp"

#include <stddef.h>
#include <stdint.h>

#include <algorithm.hpp>

#include "impl/impl.hpp"
#include "logger/logger.hpp"

#include "serial_port.hpp"

namespace terminal
{
    static bool is_initialized = false;

    static vec2u cursor_pos;

    static combined_color current_color;

    void init()
    {
        impl::init();

        cursor_pos = { 0, 0 };

        current_color.background = terminal::color::black;
        current_color.foreground = terminal::color::white;

        logger::info("Terminal initialized");

        is_initialized = true;
    }

    void scroll_down()
    {
        if (!is_initialized)
            return;

        impl::scroll_down();

        cursor_pos.y--;
    }

    void next_line()
    {
        if (!is_initialized)
            return;

        cursor_pos.x = 0;

        cursor_pos.y++;

        if (cursor_pos.y == impl::get_height())
        {
            scroll_down();
        }

        impl::set_cursor_at(cursor_pos);
    }

    void put_char_at(char ch, const vec2u& pos)
    {
        if (!is_initialized)
            return;

        impl::put_char_at(ch, pos, current_color);
    }

    void put_char(char ch)
    {
        if (!is_initialized)
            return;

        put_char_at(ch, cursor_pos);

        cursor_pos.x++;

        if (cursor_pos.x >= impl::get_width())
        {
            next_line();
        }

        impl::set_cursor_at(cursor_pos);
    }

    void clear_screen()
    {
        if (!is_initialized)
            return;

        impl::clear_screen();
        move_cursor({ 0, 0 });
    }

    void move_cursor(const vec2u& pos)
    {
        if (!is_initialized)
            return;

        cursor_pos = pos;
        impl::set_cursor_at(cursor_pos);
    }

    const vec2u& get_cursor_pos()
    {
        return cursor_pos;
    }

    void set_color(terminal::color foreground, terminal::color background)
    {
        set_foreground_color(foreground);
        set_background_color(background);
    }

    void set_foreground_color(terminal::color color)
    {
        current_color.foreground = color;
    }

    void set_background_color(terminal::color color)
    {
        current_color.background = color;
    }
} // namespace terminal
