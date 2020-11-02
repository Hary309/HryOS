#pragma once

#include "charconv.hpp"
#include "numerics.hpp"
#include "string.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace hlib
{
    using OutputFunction_t = void(char);

    template<typename>
    struct formatter;

    template<>
    struct formatter<int>
    {
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

        num_base base = num_base::dec;
    };

    template<>
    struct formatter<uint32_t>
    {
        void parse(const char* fmt)
        {
            switch (*fmt)
            {
                case 'x': base = num_base::hex; break;
                case 'b': base = num_base::bin; break;
                case 'o': base = num_base::oct; break;
            }
        }

        void format(OutputFunction_t output, uint32_t num)
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

        num_base base = num_base::dec;
    };

    template<>
    struct formatter<float>
    {
        void parse(const char* fmt)
        {
            if (is_digit(*fmt))
            {
                precision = *fmt - '0';
            }
        }

        void format(OutputFunction_t output, float num)
        {
            char buffer[12]{};
            to_chars(buffer, buffer + 11, num, precision);

            auto* it = buffer;

            while (*it)
            {
                output(*it);
                it++;
            }
        }

        int precision = 3;
    };

    template<>
    struct formatter<const char*>
    {
        void parse(const char*& /*fmt*/)
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
    struct formatter<const char[N]>
    {
        void parse(const char*& /*fmt*/)
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

    inline bool is_start_formatting(char ch)
    {
        return ch == '{';
    }

    inline bool is_end_formatting(char ch)
    {
        return ch == '}';
    }

    inline void print_to_format_sign(OutputFunction_t output, const char*& fmt)
    {
        while (!is_start_formatting(*fmt) && *fmt)
        {
            output(*fmt);
            fmt++;
        }
    }

    inline void format_to(OutputFunction_t output, const char* fmt)
    {
        while (*fmt)
        {
            output(*fmt);
            ++fmt;
        }
    }

    template<typename Arg>
    void format_to(OutputFunction_t output, const char* fmt, Arg&& arg)
    {
        print_to_format_sign(output, fmt);

        formatter<hlib::remove_reference_t<Arg>> f;
        f.parse(++fmt);
        f.format(output, hlib::forward<Arg>(arg));

        // skip inside of {}
        while (!is_end_formatting(*fmt))
        {
            ++fmt;
        }

        // skip }
        ++fmt;

        format_to(output, fmt);
    }

    template<typename Arg, typename... Args>
    void format_to(OutputFunction_t output, const char* fmt, Arg&& arg, Args&&... args)
    {
        print_to_format_sign(output, fmt);

        formatter<hlib::remove_reference_t<Arg>> f;
        f.parse(++fmt);
        f.format(output, hlib::forward<Arg>(arg));

        // skip inside of {}
        while (!is_end_formatting(*fmt))
        {
            ++fmt;
        }

        // skip }
        ++fmt;

        format_to(output, fmt, hlib::forward<Args>(args)...);
    }
} // namespace hlib
