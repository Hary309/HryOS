#pragma once

#include "math/vec2.hpp"

#include "color.hpp"
#include "multiboot.h"

namespace vesa
{
    bool init(multiboot_info* mbi);

    void set_pixeL(const vec2u& pos, color color);

    const vec2u& get_screen_size();

    color* get_display();
} // namespace vesa
