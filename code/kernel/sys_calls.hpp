#pragma once

#include <stdint.h>

#include "scheduler/process.hpp"

namespace sys_calls
{
    void sleep_ms(uint32_t time);
} // namespace sys_calls
