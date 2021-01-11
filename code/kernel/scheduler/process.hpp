#pragma once

#include <stdint.h>

#include <stack.hpp>
#include <string_view.hpp>
#include <variant.hpp>

namespace scheduler
{
    static const int STACK_SIZE = 1024 * 16;

    using pid_t = uint32_t;

    struct process
    {
        using function_t = int();

        struct sleep_data
        {
            uint32_t sleep_timeout;
        };

        struct waiting_data
        {
            pid_t pid;
        };

        struct blocked_data
        {
            enum type
            {
                io
            } value;
        };

        using Variant_t = hlib::variant<sleep_data, waiting_data, blocked_data>;

        enum class state
        {
            empty,    // not defined
            running,  // currently running
            ready,    // ready to run
            blocked,  // blocked by sth
            waiting,  // waiting for process to end
            sleeping, // sleep to @sleep_end
            count     // count of states
        };

        pid_t pid;

        char name[16]{};

        hlib::stack* stack;

        state state = state::empty;
        Variant_t state_data;

        function_t* task = nullptr;
        uint32_t start_time;
    };

    inline hlib::string_view state_to_text(enum process::state state)
    {
        switch (state)
        {
            case process::state::running: return "RUNNING";
            case process::state::ready: return "READY";
            case process::state::blocked: return "BLOCKED";
            case process::state::waiting: return "WAITING";
            case process::state::sleeping: return "SLEEPING";
            case process::state::empty: return "EMPTY";
            default:;
        }

        return "UNKNOWN";
    }

} // namespace scheduler
