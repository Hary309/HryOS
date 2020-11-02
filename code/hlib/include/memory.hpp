#pragma once

#include <stddef.h>

namespace hlib
{
    template<typename T>
    size_t distance(T first, T last)
    {
        return last - first;
    }
} // namespace hlib
