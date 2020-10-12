#pragma once

#include <stdint.h>

#include "color.hpp"

namespace terminal
{
    class entry
    {
    public:
        void set_character(char ch) { _character = ch; }

        char get_character() { return _character; }

        void set_color(terminal::color foreground, terminal::color background)
        {
            _color = static_cast<uint8_t>(foreground) | static_cast<uint8_t>(background) << 4;
        }

        void set_color(terminal::combined_color color)
        {
            set_color(color.foreground, color.background);
        }

        void set_foreground_color(terminal::color fg)
        {
            _color &= 0b11110000;
            _color |= (0b00001111 & static_cast<uint8_t>(fg));
        }

        void set_background_color(terminal::color bg)
        {
            _color &= 0b00001111;
            _color |= (0b11110000 & static_cast<uint8_t>(bg) << 4);
        }

    private:
        char _character;
        uint8_t _color;
    };
} // namespace terminal
