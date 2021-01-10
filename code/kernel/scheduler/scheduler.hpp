#pragma once

#include <optional.hpp>

#include "interrupts/interrupts.hpp"
#include "scheduler/process.hpp"

namespace scheduler
{
    void reschedule();

    hlib::optional<pid_t> create_process(const char* name, process::function_t* task);
    void terminate_process(pid_t pid);

    process* get_current_process();

    void init();

    void block_current_process(enum process::blocked_data::type type);
    void notify_blocked(enum process::blocked_data::type type);

    void sleep_ms(uint32_t time);
    void wait_for(pid_t pid);

    void halt();
} // namespace scheduler
