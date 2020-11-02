#pragma once

#include <stdint.h>

#include "numerics.hpp"

namespace hlib
{
    void to_chars(char* first, char* last, int value, num_base base = num_base::dec);
    void to_chars(char* first, char* last, uint32_t value, num_base base = num_base::dec);
    void to_chars(char* first, char* last, float value, int precision);
} // namespace hlib
