#pragma once

#include <stddef.h>

namespace hlib
{
    inline bool is_digit(char ch)
    {
        return ch >= '0' && ch <= '9';
    }
} // namespace hlib
