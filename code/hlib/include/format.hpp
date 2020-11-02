#pragma once

#include "charconv.hpp"
#include "numerics.hpp"
#include "utility.hpp"

namespace hlib
{
    using OutputFunction_t = void(char);

    template<typename>
    struct formatter;

    template<>
    struct formatter<int>
    {
        num_base base = num_base::dec;

        void parse(const char* fmt)
        {
            switch (*fmt)
            {
                case 'x': base = num_base::hex; break;
                case 'b': base = num_base::bin; break;
                case 'o': base = num_base::oct; break;
            }
        }

        void format(OutputFunction_t output, int num)
        {
            char buffer[12]{};
            to_chars(buffer, buffer + 11, num, base);

            auto* it = buffer;

            while (*it)
            {
                output(*it);
                it++;
            }
        }
    };

    template<>
    struct formatter<const char*>
    {
        void parse(const char* fmt)
        {
        }

        void format(OutputFunction_t output, const char* txt)
        {
            while (*txt)
            {
                output(*txt);
                txt++;
            }
        }
    };

    template<auto N>
    struct formatter<const char (&)[N]>
    {
        void parse(const char* fmt)
        {
        }

        void format(OutputFunction_t output, const char (&txt)[N])
        {
            auto* it = txt;
            while (*it)
            {
                output(*it);
                it++;
            }
        }
    };

    bool is_start_formatting(char ch)
    {
        return ch == '{';
    }

    bool is_end_formatting(char ch)
    {
        return ch == '}';
    }

    template<typename T>
    void execute_formatter_imp(
        OutputFunction_t output, const char* fmt, int& current_index, int index, T&& t)
    {
        if (current_index == index)
        {
            formatter<T> f;
            f.parse(fmt);
            f.format(output, t);
            current_index++;
        }
        else
        {
            current_index++;
        }
    }

    template<typename... Ts>
    void execute_formatter(
        OutputFunction_t output, const char* fmt, int current_index, int index, Ts&&... ts)
    {
        (execute_formatter_imp(output, fmt, current_index, index, hlib::forward<Ts>(ts)), ...);
    }

    template<typename... Ts>
    void format_to(OutputFunction_t output, const char* fmt, Ts&&... ts)
    {
        auto* it = fmt;

        int format_index = 0;

        bool formating = false;

        while (*it)
        {
            char ch = *it;

            if (is_start_formatting(ch))
            {
                execute_formatter(output, it + 1, 0, format_index++, hlib::forward<Ts>(ts)...);
                formating = true;
            }

            if (!formating)
            {
                output(ch);
            }

            if (is_end_formatting(ch))
            {
                formating = false;
            }

            it++;
        }
    }
} // namespace hlib
