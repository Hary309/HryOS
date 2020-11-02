#include "logger.hpp"

#include "port_utils.hpp"

constexpr auto COM1_PORT = 0x3F8;

void logger::init()
{
    out_byte(COM1_PORT + 1, 0x00); // Disable all interrupts
    out_byte(COM1_PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    out_byte(COM1_PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    out_byte(COM1_PORT + 1, 0x00); //                  (hi byte)
    out_byte(COM1_PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    out_byte(COM1_PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    out_byte(COM1_PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

int is_transmit_empty()
{
    return in_byte(COM1_PORT + 5) & 0x20;
}

void logger::write_char(char ch)
{
    while (is_transmit_empty() == 0)
    {
    }

    out_byte(COM1_PORT, ch);
}
