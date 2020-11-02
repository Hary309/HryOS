#pragma once

#include <stdint.h>

#include "color.hpp"

namespace terminal
{
    class entry
    {
    public:
        void set_character(char ch)
        {
            character_ = ch;
        }

        char get_character()
        {
            return character_;
        }

        void set_color(terminal::color foreground, terminal::color background)
        {
            color_ = static_cast<uint8_t>(foreground) | static_cast<uint8_t>(background) << 4;
        }

        void set_color(terminal::combined_color color)
        {
            set_color(color.foreground, color.background);
        }

        void set_foreground_color(terminal::color fg)
        {
            color_ &= 0b11110000;
            color_ |= (0b00001111 & static_cast<uint8_t>(fg));
        }

        void set_background_color(terminal::color bg)
        {
            color_ &= 0b00001111;
            color_ |= (0b11110000 & static_cast<uint8_t>(bg) << 4);
        }

        void reset()
        {
            character_ = 0;
            color_ = 0;
        }

    private:
        char character_;
        uint8_t color_;
    };
} // namespace terminal
