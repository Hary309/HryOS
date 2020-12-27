#pragma once

#include "charconv.hpp"
#include "cstring.hpp"
#include "numerics.hpp"
#include "string.hpp"
#include "type_traits.hpp"
#include "utility.hpp"

namespace hlib
{
    using OutputFunction_t = void(char);

    template<typename>
    struct formatter;

    template<typename T>
    struct formatter_integral
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
            char buffer[33]{};
            to_chars(buffer, buffer + 32, static_cast<T>(num), base);

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
    struct formatter<char> : formatter_integral<int8_t>
    {
    };

    template<>
    struct formatter<unsigned char> : formatter_integral<uint8_t>
    {
    };

    template<>
    struct formatter<short> : formatter_integral<int16_t>
    {
    };

    template<>
    struct formatter<unsigned short> : formatter_integral<uint16_t>
    {
    };

    template<>
    struct formatter<int> : formatter_integral<int32_t>
    {
    };

    template<>
    struct formatter<unsigned int> : formatter_integral<uint32_t>
    {
    };

    template<>
    struct formatter<long unsigned int> : formatter_integral<uint32_t>
    {
    };

    template<>
    struct formatter<bool>
    {
        void parse(const char* fmt)
        {
            as_string = *fmt == 's';
        }

        void format(OutputFunction_t output, bool num)
        {
            if (as_string)
            {
                const char* txt = "false";

                if (num)
                {
                    txt = "true";
                }

                while (*txt)
                {
                    output(*txt);
                    txt++;
                }
            }
            else
            {
                output('0' + static_cast<int>(num));
            }
        }

        bool as_string = false;
    };

    template<>
    struct formatter<uint64_t>
    {
        void parse(const char* fmt)
        {
            switch (*fmt)
            {
                case 'x': base = num_base::hex; break;
                case 'b': base = num_base::bin; break;
                case 'o': base = num_base::oct; break;
            }

            fmt++;

            leading_zero = *fmt == '0';
        }

        void format(OutputFunction_t output, uint64_t num)
        {
            if (num >> 32 != 0 || (num >> 32 == 0 && leading_zero))
            {
                print_value(output, static_cast<uint32_t>(num >> 32));
            }

            print_value(output, static_cast<uint32_t>(num));
        }

        void print_value(OutputFunction_t output, uint32_t value)
        {
            char buffer[33]{};
            auto digit_count = to_chars(buffer, buffer + 32, static_cast<uint32_t>(value), base);

            auto* it = buffer;

            if (leading_zero && base == num_base::hex)
            {
                const int MAX_LENGTH = 8;

                // fill with zeros
                for (size_t i = 0; i < MAX_LENGTH - digit_count; i++)
                {
                    output('0');
                }
            }

            while (*it)
            {
                output(*it);
                it++;
            }
        }

        num_base base = num_base::dec;
        bool leading_zero = false;
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
    struct formatter<char*>
    {
        void parse(const char* /*fmt*/)
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
    struct formatter<char[N]>
    {
        void parse(const char* /*fmt*/)
        {
        }

        void format(OutputFunction_t output, const char (&txt)[N])
        {
            for (size_t i = 0; i < N && txt[i]; i++)
            {
                output(txt[i]);
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

        formatter<hlib::remove_const_t<hlib::remove_reference_t<Arg>>> f;
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

        formatter<hlib::remove_const_t<hlib::remove_reference_t<Arg>>> f;
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
