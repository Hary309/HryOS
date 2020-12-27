#pragma once

#include <stdint.h>

namespace port
{
    inline uint8_t in_byte(uint16_t port)
    {
        uint8_t ret;

        asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));

        return ret;
    }

    inline void out_byte(uint16_t port, uint8_t data)
    {
        asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
    }
} // namespace port
