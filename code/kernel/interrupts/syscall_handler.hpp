#pragma once

#include "interrupts/interrupts.hpp"

extern "C" int syscall();

extern "C" __attribute__((fastcall)) void syscall_handler(interrupts::registers* regs);

void syscall_init();
