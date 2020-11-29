#pragma once

#include "interrupts/interrupts.hpp"

namespace scheduler
{
    using task_t = void();

    void tick(interrupts::registers* regs);

    void create_task(task_t* task);

    void init();
} // namespace scheduler
