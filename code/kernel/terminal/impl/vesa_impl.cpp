#include <algorithm.hpp>
#include <binary.hpp>

#include "drivers/vesa/color.hpp"
#include "drivers/vesa/vesa.hpp"
#include "fonts/fonts.hpp"
#include "fonts/psf.hpp"
#include "logger/logger.hpp"
#include "math/vec2.hpp"
#include "storage/fonts/zap-light16.h"
#include "terminal/color.hpp"

#include "assert.hpp"
#include "config.hpp"
#include "impl.hpp"

#if USE_VESA

static fonts::font* current_font;

vesa::color terminal_to_vesa_color(terminal::color color)
{
    switch (color)
    {
        case terminal::color::black: return vesa::color::create(0, 0, 0);
        case terminal::color::blue: return vesa::color::create(0, 0, 170);
        case terminal::color::green: return vesa::color::create(0, 170, 0);
        case terminal::color::cyan: return vesa::color::create(0, 170, 170);
        case terminal::color::red: return vesa::color::create(170, 0, 0);
        case terminal::color::magenta: return vesa::color::create(170, 0, 170);
        case terminal::color::brown: return vesa::color::create(170, 85, 0);
        case terminal::color::dark_grey: return vesa::color::create(85, 85, 85);
        case terminal::color::light_grey: return vesa::color::create(85, 85, 85);
        case terminal::color::light_blue: return vesa::color::create(85, 85, 255);
        case terminal::color::light_green: return vesa::color::create(85, 255, 85);
        case terminal::color::light_cyan: return vesa::color::create(85, 255, 255);
        case terminal::color::light_red: return vesa::color::create(255, 85, 85);
        case terminal::color::light_magenta: return vesa::color::create(255, 85, 255);
        case terminal::color::light_brown: return vesa::color::create(255, 255, 255);
        case terminal::color::white: return vesa::color::create(255, 255, 255);
    }

    return { 255, 255, 255 };
}

void reset_char(const vec2u& pos)
{
    const auto real_pos = pos * vec2u{ 8, 16 };

vec2u get_screen_pos(const vec2u& text_pos)
{
    return text_pos * current_font->size;
}

void reset_char(const vec2u& text_pos)
{
    const auto screen_pos = get_screen_pos(text_pos);

    for (size_t y = 0; y < 16; y++)
    {
        for (size_t x = 0; x < 8; x++)
        {
            vesa::set_pixeL(real_pos + vec2u{ x, y }, { 0, 0, 0 });
        }
    }
}

void terminal::impl::init()
{
    current_font = &fonts::ZAP_LIGHT_16;
}

void terminal::impl::clear_screen()
{
    const auto& screen_size = vesa::get_screen_size();

    auto* it = vesa::get_display();
    auto* end = it + screen_size.x * screen_size.y;

    while (it != end)
    {
        *it = {};
        ++it;
    }
}

void terminal::impl::put_char_at(
    char ch, const vec2u& text_pos, const terminal::combined_color& color)
{
    reset_char(text_pos);

    const auto& current_glyph = current_font->glyphs[(uint32_t)ch];
    const auto vesa_color = terminal_to_vesa_color(color.foreground);
    const auto screen_pos = get_screen_pos(text_pos);

    for (size_t y = 0; y < current_font->size.y; y++)
    {
        const auto& current_row = current_glyph.data[y];

        for (size_t x = 0; x < current_font->size.x; x++)
        {
            if (hlib::get_bit(current_row, current_font->size.x - 1 - x))
            {
                vesa::set_pixeL(real_pos + vec2u{ x, y }, vesa_color);
            }
        }
    }
}

uint32_t terminal::impl::get_width()
{
    return vesa::get_screen_size().x / current_font->size.x;
}

uint32_t terminal::impl::get_height()
{
    return vesa::get_screen_size().y / current_font->size.y;
}

void terminal::impl::set_cursor_at(const vec2u& pos)
{
    // TODO
}

void terminal::impl::scroll_down()
{
    // TODO
}

#endif
