#pragma once

#include <stddef.h>

namespace hlib
{
    template<size_t Len, size_t Align>
    struct aligned_storage
    {
        struct type
        {
            alignas(Align) unsigned char data[Len];
        };
    };
} // namespace hlib
