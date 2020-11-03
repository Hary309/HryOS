#pragma once

#include <stdint.h>

namespace hlib
{
    inline uint32_t pow10(int power)
    {
        int result = 1;

        while (power)
        {
            result *= 10;
            power--;
        }

        return result;
    }
} // namespace hlib
