#pragma once

#include "interrupts/interrupts.hpp"
#include "scheduler/process.hpp"

namespace scheduler
{
    void tick(interrupts::registers* regs);

    void create_process(const char* name, process::function_t* task);
    void terminate_process(pid_t pid);

    process* get_current_process();

    void init();

    void sleep_ms(uint32_t time);

    void idle();
    void halt();
} // namespace scheduler
