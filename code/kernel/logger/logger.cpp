#include "logger.hpp"

#include "serial_port.hpp"

constexpr auto COM1_PORT = 0x3F8;

enum line_status : uint8_t
{
    data_ready = 1 << 0,
    overrun_error = 1 << 1,
    parity_error = 1 << 2,
    framing_error = 1 << 3,
    break_indicator = 1 << 4,
    transmission_buffer_empty = 1 << 5,
    transmitter_empty = 1 << 6,
    impending_error = 1 << 7
};

void logger::init()
{
    port::out_byte(COM1_PORT + 1, 0x00); // Disable all interrupts
    port::out_byte(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    port::out_byte(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    port::out_byte(COM1_PORT + 1, 0x00); //                  (hi byte)
    port::out_byte(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    port::out_byte(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    port::out_byte(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set

    logger::info("Logger initialized");
}

line_status get_line_status()
{
    return static_cast<line_status>(port::in_byte(COM1_PORT + 5));
}

bool is_buffer_empty()
{
    return static_cast<bool>(get_line_status() & line_status::transmission_buffer_empty);
}

void logger::write_char(char ch)
{
    while (!is_buffer_empty())
    {
        // wait for empty buffer
    }

    port::out_byte(COM1_PORT, ch);
}
