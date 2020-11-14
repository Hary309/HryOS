#include "command_line.hpp"

#include <algorithm.hpp>

#include "drivers/keyboard.hpp"
#include "terminal/terminal.hpp"

#include "cstring.hpp"

struct command
{
    char command[32];
    command_line::command_callback_t* callback;
};

static const auto MAX_BUFFER_SIZE = 32;
static const auto COMMANDS_COUNT = 32;

static char input_buffer[MAX_BUFFER_SIZE]{};
static int input_buffer_index = 0;

command commands[COMMANDS_COUNT]{};
int commands_size = 0;

void print_prompt()
{
    terminal::print("> ");
}

bool call_callback()
{
    for (int i = 0; i < commands_size; i++)
    {
        const auto& command = commands[i];

        if (hlib::cstring_compare(input_buffer, command.command) == 0)
        {
            if (command.callback)
            {
                command.callback();
            }

            return true;
        }
    }

    return false;
}

void command_line::init()
{
    print_prompt();

    register_command(
        "cls", +[] { terminal::clear_screen(); });
}

void command_line::send_input(const keyboard::key_event& key_event)
{
    const auto& cursor_pos = terminal::get_cursor_pos();

    if (key_event.state == keyboard::button_state::released)
    {
        auto ch = keyboard::map_key_to_ascii(key_event.key);

        if (input_buffer_index < MAX_BUFFER_SIZE && ch != '\0')
        {
            terminal::put_char(ch);
            input_buffer[input_buffer_index++] = ch;
        }
        else if (key_event.key == keyboard::key::enter)
        {
            terminal::next_line();

            if (!call_callback())
            {
                terminal::print_line("'{}' unknown command", input_buffer);
            }

            terminal::next_line();
            print_prompt();

            for (int i = 0; i < input_buffer_index; i++)
            {
                input_buffer[i] = 0;
            }

            input_buffer_index = 0;
        }
    }

    if (key_event.key == keyboard::key::backspace &&
        key_event.state == keyboard::button_state::pressed)
    {
        if (input_buffer_index > 0)
        {
            terminal::put_char_at(' ', { cursor_pos.x - 1, cursor_pos.y });
            terminal::move_cursor({ cursor_pos.x - 1, cursor_pos.y });
            input_buffer[--input_buffer_index] = '\0';
        }
    }
}

void command_line::register_command(const char* command, command_callback_t* callback)
{
    if (commands_size < COMMANDS_COUNT - 1)
    {
        hlib::copy_n(command, 32, commands[commands_size].command);
        commands[commands_size].callback = callback;

        commands_size++;
    }
}
