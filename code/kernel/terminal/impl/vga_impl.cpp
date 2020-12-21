#include <algorithm.hpp>

#include "drivers/vga/color.hpp"
#include "drivers/vga/vga.hpp"
#include "terminal/color.hpp"

#include "config.hpp"
#include "impl.hpp"

#if !USE_VESA

vga::combined_color terminal_to_vga_color(terminal::combined_color color)
{
    return *reinterpret_cast<vga::combined_color*>(&color);
}

void terminal::impl::init()
{
}

void terminal::impl::clear_screen()
{
    for (uint32_t y = 0; y < vga::DISPLAY_SIZE.y; y++)
    {
        for (uint32_t x = 0; x < vga::DISPLAY_SIZE.x; x++)
        {
            put_char_at(' ', { x, y }, { terminal::color::white, terminal::color::black });
        }
    }
}

void terminal::impl::put_char_at(char ch, const vec2u& pos, const terminal::combined_color& color)
{
    auto* entry = vga::get_entry(pos);
    entry->set_character(ch);
    entry->set_color(terminal_to_vga_color(color));
}

uint32_t terminal::impl::get_width()
{
    return vga::DISPLAY_SIZE.x;
}

uint32_t terminal::impl::get_height()
{
    return vga::DISPLAY_SIZE.y;
}

void terminal::impl::set_cursor_at(const vec2u& pos)
{
    vga::set_cursor_pos(pos);
}

void terminal::impl::scroll_down()
{
    auto* display = vga::get_display();

    hlib::copy_n(
        display + vga::DISPLAY_SIZE.x, vga::DISPLAY_SIZE.x * (vga::DISPLAY_SIZE.y - 1), display);

    for (uint32_t i = 0; i < vga::DISPLAY_SIZE.x; i++)
    {
        vga::get_entry({ i, vga::DISPLAY_SIZE.y - 1 })->reset();
    }
}

#endif
