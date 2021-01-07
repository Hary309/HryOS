#include <stddef.h>
#include <stdint.h>

#include <format.hpp>

#include "drivers/keyboard.hpp"
#include "drivers/pit.hpp"
#include "drivers/vesa/vesa.hpp"
#include "drivers/vga/color.hpp"
#include "drivers/vga/vga.hpp"
#include "fonts/fonts.hpp"
#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "math/vec2.hpp"
#include "memory/gdt.hpp"
#include "memory/kernel_heap.hpp"
#include "memory/memory_map.hpp"
#include "scheduler/lock_guard.hpp"
#include "scheduler/mutex.hpp"
#include "scheduler/scheduler.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "config.hpp"
#include "multiboot.h"
#include "sys_calls.hpp"

static int task_count = 0;
static mutex terminal_mutex;

void* operator new(size_t size)
{
    return kheap::allocate(size);
}

void operator delete(void* ptr)
{
    kheap::free(ptr);
}

void operator delete(void* ptr, long unsigned int)
{
    kheap::free(ptr);
}

void shutdown_callback()
{
    terminal::print_line("Shutting down...");
}

void task_base(const vec2u& pos)
{
    int timer_start = pit::get_timer();

    while (1)
    {
        terminal_mutex.spinlock();

        auto cursor_pos = terminal::get_cursor_pos();

        terminal::move_cursor(pos);
        terminal::print("{} sec", (pit::get_timer() - timer_start) / 1000);
        terminal::move_cursor(cursor_pos);

        terminal_mutex.unlock();

        sys_calls::sleep_ms(1000);
    }
}

int task()
{
    task_base({ static_cast<uint32_t>(10 + 10 * (task_count++ % 7)), 0 });

    return 0;
}

int spanko()
{
    terminal_mutex.spinlock();

    terminal::print_line("Mam spanko");
    sys_calls::sleep_ms(2000);
    terminal::print_line("No i juz");

    terminal_mutex.unlock();

    return 1;
}

extern "C" void kernel_main(uint32_t magic, multiboot_info* mbi)
{
    logger::init();

    gdt::init();

    mmap::init(mbi);
    kheap::init();

    int* value = new int(123456);
    logger::info("Value: {}", *value);
    delete value;

    interrupts::init();

    keyboard::init();

    pit::init();

#if USE_VESA
    fonts::init();
    vesa::init(mbi);
#else
    vga::init();
#endif

    terminal::init();
    terminal::clear_screen();

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        terminal::print_line("Invalid magic number [0x{x}]", magic);
        return;
    }

    terminal::print("Hry");
    terminal::set_foreground_color(terminal::color::cyan);
    terminal::print_line("OS!");
    terminal::set_foreground_color(terminal::color::white);

    command_line::init();

    command_line::register_command("shutdown", shutdown_callback);
    command_line::register_command("crash", [] { int a = 213 / 0; });

    command_line::register_command("rt", [] { scheduler::create_process("timer", task); });
    command_line::register_command("sleep", [] { scheduler::create_process("sleep", spanko); });

    command_line::register_command("wait", [] {
        auto pid = scheduler::create_process("wait", spanko);
        sys_calls::wait_for(pid.value());
    });

    scheduler::init();
    interrupts::enable();

    scheduler::idle();
}
