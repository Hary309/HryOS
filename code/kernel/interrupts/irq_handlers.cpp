#include "logger/logger.hpp"
#include "terminal/terminal.hpp"

#include "port_utils.hpp"

extern "C" void irq0_handler(void)
{
    terminal::print_line("IRQ 0");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq1_handler(void)
{
    terminal::print_line("IRQ 1");
    auto key = static_cast<char>(in_byte(0x60));
    terminal::print_line("Key stroke {}", key);

    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq2_handler(void)
{
    terminal::print_line("IRQ 2");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq3_handler(void)
{
    terminal::print_line("IRQ 3");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq4_handler(void)
{
    terminal::print_line("IRQ 4");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq5_handler(void)
{
    terminal::print_line("IRQ 5");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq6_handler(void)
{
    terminal::print_line("IRQ 6");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq7_handler(void)
{
    terminal::print_line("IRQ 7");
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq8_handler(void)
{
    terminal::print_line("IRQ 8");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq9_handler(void)
{
    terminal::print_line("IRQ 9");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq10_handler(void)
{
    terminal::print_line("IRQ 10");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq11_handler(void)
{
    terminal::print_line("IRQ 11");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq12_handler(void)
{
    terminal::print_line("IRQ 12");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq13_handler(void)
{
    terminal::print_line("IRQ 13");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq14_handler(void)
{
    terminal::print_line("IRQ 14");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}

extern "C" void irq15_handler(void)
{
    terminal::print_line("IRQ 15");
    out_byte(0xA0, 0x20);
    out_byte(0x20, 0x20); //EOI
}