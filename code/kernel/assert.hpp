#pragma once

#include "scheduler/scheduler.hpp"

#define HRY_ASSERT(condition, message)                                                             \
    if (!(condition))                                                                              \
    {                                                                                              \
        logger::error(message);                                                                    \
        scheduler::halt();                                                                         \
    }
