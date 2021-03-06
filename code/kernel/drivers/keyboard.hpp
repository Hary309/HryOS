#pragma once

#include "optional.hpp"

namespace keyboard
{
    enum class button_state
    {
        pressed = 0,
        released
    };

    enum class key
    {
        escape = 0x01,
        num1 = 0x02,
        num2 = 0x03,
        num3 = 0x04,
        num4 = 0x05,
        num5 = 0x06,
        num6 = 0x07,
        num7 = 0x08,
        num8 = 0x09,
        num9 = 0x0A,
        num0 = 0x0B,
        minus = 0x0C,
        equals = 0x0D,
        backspace = 0x0E,
        tab = 0x0F,
        q = 0x10,
        w = 0x11,
        e = 0x12,
        r = 0x13,
        t = 0x14,
        y = 0x15,
        u = 0x16,
        i = 0x17,
        o = 0x18,
        p = 0x19,
        left_square_bracket = 0x1A,
        right_squre_bracket = 0x1B,
        enter = 0x1C,
        left_ctrl = 0x1D,
        a = 0x1E,
        s = 0x1F,
        d = 0x20,
        f = 0x21,
        g = 0x22,
        h = 0x23,
        j = 0x24,
        k = 0x25,
        l = 0x26,
        semicolon = 0x27,
        single_quote = 0x28,
        back_tick = 0x29,
        backslash = 0x2A,
        left_shift = 0x28,
        z = 0x2C,
        x = 0x2D,
        c = 0x2E,
        v = 0x2F,
        b = 0x30,
        n = 0x31,
        m = 0x32,
        comma = 0x33,
        dot = 0x34,
        slash = 0x35,
        right_shift = 0x36,
        numpad_asterrisk = 0x37,
        left_alt = 0x38,
        space = 0x39,
        caps_lock = 0x3A,
        f1 = 0x3B,
        f2 = 0x3C,
        f3 = 0x3D,
        f4 = 0x3E,
        f5 = 0x3F,
        f6 = 0x40,
        f7 = 0x41,
        f8 = 0x42,
        f9 = 0x43,
        f10 = 0x44,
        num_lock = 0x45,
        scroll_lock = 0x46,
        numpad_7 = 0x47,
        numpad_8 = 0x48,
        numpad_9 = 0x49,
        numpad_minus = 0x4A,
        numpad_4 = 0x4B,
        numpad_5 = 0x4C,
        numpad_6 = 0x4D,
        numpad_plus = 0x4E,
        numpad_1 = 0x4F,
        numpad_2 = 0x50,
        numpad_3 = 0x51,
        numpad_0 = 0x52,
        numpad_dot = 0x53,
    };

    struct key_event
    {
        button_state state;
        keyboard::key key;
    };

    void init();

    bool is_buffer_empty();
    key_event pull_key();
    hlib::optional<key_event> try_pull_key();

    char map_key_to_ascii(keyboard::key key);
} // namespace keyboard
