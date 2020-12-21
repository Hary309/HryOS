#pragma once

#include "logger/logger.hpp"
#include "scheduler/scheduler.hpp"

#define HRY_ASSERT(condition, message)                                                             \
    if (!(condition))                                                                              \
    {                                                                                              \
        logger::error(message);                                                                    \
        scheduler::halt();                                                                         \
    }
