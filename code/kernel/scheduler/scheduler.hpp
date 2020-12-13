#pragma once

#include "interrupts/interrupts.hpp"
#include "scheduler/process.hpp"

namespace scheduler
{
    using process_function_t = void();

    void tick(interrupts::registers* regs);

    void create_process(process_function_t* task);
    void terminate_process(pid_t pid);

    process* get_current_process();

    void init();

    void idle();
    void halt();
} // namespace scheduler
