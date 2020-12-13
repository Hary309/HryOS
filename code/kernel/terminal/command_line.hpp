#pragma once

#include "drivers/keyboard.hpp"

namespace command_line
{
    using command_callback_t = void();

    void init();

    void register_command(const char* command, command_callback_t* callback);
} // namespace command_line
