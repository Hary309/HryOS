#pragma once

#include <stdint.h>

namespace interrupts
{
    struct sys_regs
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
    };

    using callback_t = void(sys_regs*);

    void init();

    void register_isr_callback(int irq_id, callback_t callback);
} // namespace interrupts
