#pragma once

#include <stdint.h>

#include "math/vec2.hpp"

#include "entry.hpp"

namespace terminal
{
    class display
    {
    public:
        display() = default;

        display(uintptr_t display_addr, vec2u display_size)
            : display_(reinterpret_cast<entry*>(display_addr)), display_size_(display_size)
        {
        }

        entry* get_entry(const vec2u& pos)
        {
            if (pos.x < display_size_.x && pos.y < display_size_.y)
            {
                return display_ + get_index(pos);
            }

            return nullptr;
        }

    private:
        int get_index(const vec2u& pos) { return pos.y * display_size_.x + pos.x; }

    private:
        entry* display_;
        vec2u display_size_;
    };
} // namespace terminal