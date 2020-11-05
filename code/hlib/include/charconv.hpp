#pragma once

#include <stdint.h>

#include "numerics.hpp"

namespace hlib
{
    size_t to_chars(char* first, char* last, int8_t value, num_base base = num_base::dec);
    size_t to_chars(char* first, char* last, uint8_t value, num_base base = num_base::dec);
    size_t to_chars(char* first, char* last, int16_t value, num_base base = num_base::dec);
    size_t to_chars(char* first, char* last, uint16_t value, num_base base = num_base::dec);
    size_t to_chars(char* first, char* last, int32_t value, num_base base = num_base::dec);
    size_t to_chars(char* first, char* last, uint32_t value, num_base base = num_base::dec);
    void to_chars(char* first, char* last, float value, int precision);
} // namespace hlib
