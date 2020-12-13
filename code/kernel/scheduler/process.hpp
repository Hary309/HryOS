#pragma once

#include <stdint.h>

namespace scheduler
{
    static const int STACK_SIZE = 8 * 1024;

    using pid_t = uint32_t;

    struct process
    {
        using function_t = int();

        enum class state
        {
            empty,   // not defined
            running, // currently running
            ready,   // ready to run
            blocked, // blocked by IO
            sleeping // sleep to @sleep_end
        };

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

            uint32_t eip;
            uint32_t cs;
            uint32_t eflags;
        };

        pid_t pid;

        char stack[STACK_SIZE]{};
        registers registers;
        state state = state::empty;
        function_t* task = nullptr;
        uint32_t start_time;
    };

    inline const char* state_to_text(enum process::state state)
    {
        switch (state)
        {
            case process::state::running: return "RUNNING";
            case process::state::ready: return "READY";
            case process::state::blocked: return "BLOCKED";
            case process::state::sleeping: return "SLEEPING";
            case process::state::empty: return "EMPTY";
        }

        return "UNKNOWN";
    }

} // namespace scheduler
