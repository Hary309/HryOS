#include "sys_calls.hpp"

#include <stdint.h>

extern "C" void sys_call_1(uint32_t id, uint32_t arg);

void sys_calls::sleep_ms(uint32_t time)
{
    sys_call_1(0, time);
}
