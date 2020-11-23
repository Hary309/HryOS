#include "pit.hpp"

#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "terminal/command_line.hpp"
#include "terminal/terminal.hpp"

#include "port_utils.hpp"

static const uint16_t CHANNEL_0_PORT = 0x40;
static const uint16_t MC_REGISTER = 0x43; // mode/command register

static const uint32_t OSCILLATOR_SPEED = 1193182; // Hz
static const uint16_t CLOCK_SPEED = 1000;         // Hz (1ms)

static uint64_t timer = 0;

enum channel
{
    channel_0 = 0b00 << 6,
    channel_1 = 0b01 << 6,
    channel_2 = 0b10 << 6
};

enum access_mode
{
    lobyte_only = 0b01 << 4,
    hibyte_only = 0b10 << 4,
    both = 0b11 << 4,
};

// 0b 11 11 111 1

enum operating_mode
{
    mode_0 = 0b000 << 1, // interrupt on terminal count
    mode_1 = 0b001 << 1, // hardware re-triggerable one-shot
    mode_2 = 0b010 << 1, // rate generator
    mode_3 = 0b011 << 1, // square wave generator
    mode_4 = 0b100 << 1, // software triggered strobe
    mode_5 = 0b101 << 1  // hardware triggered strobe
};

void irq_callback(interrupts::registers* regs)
{
    // because PIT is to 1ms so I can just increase the variable
    timer++;
}

void command_line_timer()
{
    terminal::print_line("Timer: {}s ({})", static_cast<float>(timer) / 1000, timer);
}

void pit::init()
{
    const auto divisor = OSCILLATOR_SPEED / CLOCK_SPEED;

    port::out_byte(MC_REGISTER, channel::channel_0 | access_mode::both | operating_mode::mode_3);
    port::out_byte(CHANNEL_0_PORT, static_cast<uint8_t>(divisor));
    port::out_byte(CHANNEL_0_PORT, static_cast<uint8_t>(divisor >> 8));

    interrupts::register_isr_callback(0, irq_callback);

    command_line::register_command("uptime", command_line_timer);

    logger::info("PIT driver initialized");
}

uint32_t pit::get_timer()
{
    return timer;
}
