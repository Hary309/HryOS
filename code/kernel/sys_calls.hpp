#pragma once

#include <stdint.h>

#include "scheduler/process.hpp"

namespace sys_calls
{
    void sleep_ms(uint32_t time);

    void wait_for(scheduler::pid_t pid);
} // namespace sys_calls
