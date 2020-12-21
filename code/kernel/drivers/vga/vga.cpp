#include "vga.hpp"

#include "drivers/vga/entry.hpp"
#include "logger/logger.hpp"

#include "port_utils.hpp"

static constexpr auto VGA_DISPLAY_ADDRESS = 0xB8000;

static vga::entry* display;

void vga::init()
{
    display = reinterpret_cast<entry*>(VGA_DISPLAY_ADDRESS);

    logger::info("VGA driver initialized");
}

vga::entry* vga::get_entry(const vec2u& pos)
{
    if (pos.x < vga::DISPLAY_SIZE.x && pos.y < vga::DISPLAY_SIZE.y)
    {
        return display + pos.y * vga::DISPLAY_SIZE.x + pos.x;
    }

    return nullptr;
}

void vga::set_entry(const vec2u& pos, char ch, vga::combined_color color)
{
    entry* e = get_entry(pos);

    if (e != nullptr)
    {
        e->set_color(color);
        e->set_character(ch);
    }
}

void vga::set_entry(const vec2u& pos, char ch, color fg, color bg)
{
    entry* e = get_entry(pos);

    if (e != nullptr)
    {
        e->set_color({ fg, bg });
        e->set_character(ch);
    }
}

void vga::set_cursor_pos(const vec2u& pos)
{
    uint16_t flat_pos = pos.y * DISPLAY_SIZE.x + pos.x;

    port::out_byte(0x3D4, 0x0F);
    port::out_byte(0x3D5, (uint8_t)(flat_pos & 0xFF));
    port::out_byte(0x3D4, 0x0E);
    port::out_byte(0x3D5, (uint8_t)((flat_pos >> 8) & 0xFF));
}

vga::entry* vga::get_display()
{
    return display;
}
