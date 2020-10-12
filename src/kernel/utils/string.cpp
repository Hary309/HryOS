#include "string.hpp"

uint32_t string_length(const char* str)
{
    if (str == nullptr)
    {
        return 0;
    }

    uint32_t length = 0;

    while (*str)
    {
        str++;
        length++;
    }

    return length;
}