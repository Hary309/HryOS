#pragma once

#include <stdint.h>

namespace gdt
{
    inline constexpr uint16_t NULL_SELECTOR = 0x00;
    inline constexpr uint16_t KERNEL_CODE_SELECTOR = 0x08;
    inline constexpr uint16_t KERNEL_DATA_SELECTOR = 0x10;
    inline constexpr uint16_t USER_CODE_SELECTOR = 0x18;
    inline constexpr uint16_t USER_DATA_SELECTOR = 0x20;

    void init();
} // namespace gdt
