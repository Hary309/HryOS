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

    inline constexpr int cstring_compare(const char* a, const char* b)
    {
        while (*a && *a == *b)
        {
            ++a;
            ++b;
        }

        return (*a > *b) - (*b > *a);
    }
} // namespace hlib
