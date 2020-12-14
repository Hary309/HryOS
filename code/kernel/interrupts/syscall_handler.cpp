#include "syscall_handler.hpp"

#include <array.hpp>

#include "interrupts/interrupts.hpp"
#include "scheduler/process.hpp"
#include "scheduler/scheduler.hpp"

using system_call_t = void(interrupts::registers* regs);

hlib::array<system_call_t*, 16> system_calls;
uint32_t system_calls_index = 0;

void sleep_ms_handler(interrupts::registers* regs)
{
    uint32_t time = regs->ebx;
}

extern "C" __attribute__((fastcall)) void syscall_handler(interrupts::registers* regs)
{
    uint32_t id = regs->eax;

    if (system_calls[id] != nullptr)
    {
        system_calls[id](regs);
    }
}

void syscall_init()
{
    system_calls[system_calls_index++] = sleep_ms_handler;
}
