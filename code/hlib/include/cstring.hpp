#pragma once

#include <stddef.h>

namespace hlib
{
    inline constexpr size_t cstring_length(const char* str)
    {
        size_t length = 0;

        while (*str++)
        {
            length++;
        }

        return length;
    }
} // namespace hlib
