#pragma once

#include <stdint.h>

#include "math/vec2.hpp"

#include "entry.hpp"

namespace terminal
{
    class cursor
    {
    public:
        cursor() = default;

        cursor(uintptr_t display_addr, vec2u display_size)
            : _display(reinterpret_cast<entry*>(display_addr)), _display_size(display_size)
        {
        }

        entry* get_entry(const vec2u& pos)
        {
            if (pos.x < _display_size.x && pos.y < _display_size.y)
            {
                return _display + get_index(pos);
            }

            return nullptr;
        }

    private:
        int get_index(const vec2u& pos) { return pos.y * Width + pos.x; }

    private:
        entry* _display;
        vec2u _display_size;
    };
} // namespace terminal