#pragma once

#include <stddef.h>

namespace hlib
{
    enum class num_base
    {
        bin,
        oct,
        dec,
        hex
    };

    inline size_t get_base(num_base base)
    {
        switch (base)
        {
            case num_base::bin: return 2;
            case num_base::oct: return 8;
            case num_base::dec: return 10;
            case num_base::hex: return 16;
        }

        return 10;
    }

    template<typename N>
    size_t digits_count(N number, num_base base = num_base::dec)
    {
        size_t num_base = 10;

        if (base != num_base::dec)
        {
            num_base = get_base(base);
        }

        if (number < static_cast<N>(num_base))
        {
            return 1;
        }

        size_t count = 0;

        while (number)
        {
            number /= num_base;
            count++;
        }

        return count;
    }
} // namespace hlib
