#include "keyboard.hpp"

#include "interrupts/interrupts.hpp"
#include "logger/logger.hpp"
#include "scheduler/scheduler.hpp"
#include "terminal/command_line.hpp"

#include "port_utils.hpp"

void clear_buffer()
{
    unsigned char key = 0;
    while (((key = port::in_byte(0x64)) & 1) == 1)
    {
        port::in_byte(0x60);
    }
}

void on_isr_callback(interrupts::registers* regs)
{
    // logger::info("ip: {x} cs: {x} eflags: {b}", regs->eip, regs->cs, regs->eflags);

    auto key_code = static_cast<uint8_t>(port::in_byte(0x60));

    keyboard::key_event key_event{};
    key_event.key = static_cast<keyboard::key>(key_code & 0b01111111);
    key_event.state = static_cast<keyboard::button_state>((key_code & 0b10000000) >> 7);

    if (key_event.key == keyboard::key::tab && key_event.state == keyboard::button_state::released)
    {
        scheduler::tick(regs);
    }
    else
    {
        command_line::send_input(key_event);
    }
}

void keyboard::init()
{
    clear_buffer();

    interrupts::register_isr_callback(1, on_isr_callback);

    logger::info("Keyboard driver initialized");
}

char keyboard::map_key_to_ascii(keyboard::key key)
{
    switch (key)
    {
        case key::a: return 'a';
        case key::b: return 'b';
        case key::c: return 'c';
        case key::d: return 'd';
        case key::e: return 'e';
        case key::f: return 'f';
        case key::g: return 'g';
        case key::h: return 'h';
        case key::i: return 'i';
        case key::j: return 'j';
        case key::k: return 'k';
        case key::l: return 'l';
        case key::m: return 'm';
        case key::n: return 'n';
        case key::o: return 'o';
        case key::p: return 'p';
        case key::q: return 'q';
        case key::r: return 'r';
        case key::s: return 's';
        case key::t: return 't';
        case key::u: return 'u';
        case key::v: return 'v';
        case key::w: return 'w';
        case key::x: return 'x';
        case key::y: return 'y';
        case key::z: return 'z';
        case key::num0: return '0';
        case key::num1: return '1';
        case key::num2: return '2';
        case key::num3: return '3';
        case key::num4: return '4';
        case key::num5: return '5';
        case key::num6: return '6';
        case key::num7: return '7';
        case key::num8: return '8';
        case key::num9: return '9';
        case key::space: return ' ';
        default: return '\0';
    }
}
