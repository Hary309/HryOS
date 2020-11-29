#pragma once

#include <stdint.h>

namespace interrupts
{
    struct registers
    {
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
        uint32_t irq_id;
        uint32_t eip;
        uint32_t cs;
        uint32_t eflags;
    };

    using callback_t = void(registers*);

    void init();

    void register_isr_callback(int irq_id, callback_t callback);
} // namespace interrupts
