#pragma once

namespace hlib
{
    template<typename T>
    constexpr bool get_bit(T value, int bit)
    {
        return ((value) & (1 << bit));
    }
} // namespace hlib
