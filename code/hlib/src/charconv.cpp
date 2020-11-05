#include "charconv.hpp"

#include "math.hpp"
#include "memory.hpp"
#include "numerics.hpp"
#include "type_traits.hpp"

namespace hlib
{
    static const auto DIGITS = "0123456789ABCDEF";

    template<typename T, typename = enable_if_t<is_integral_v<T>>>
    size_t to_chars_integral_impl(char* first, char* last, T value, num_base base)
    {
        if (first >= last)
        {
            return 0;
        }

        size_t digits_count = 0;

        if constexpr (!is_unsigned_v<T>)
        {
            if (value < 0)
            {
                *first = '-';
                value = -value;
                digits_count++;
            }
        }

        digits_count += hlib::digits_count(value, base);

        if (distance(first, last) < digits_count)
        {
            return 0;
        }

        auto num_base = get_base(base);

        auto* it = first + digits_count - 1;

        while (value)
        {
            auto digit = value % num_base;
            value /= num_base;

            *it = DIGITS[digit];
            it--;
        }

        return digits_count;
    }

    size_t to_chars(char* first, char* last, int8_t value, num_base base)
    {
        return to_chars_integral_impl(first, last, value, base);
    }

    size_t to_chars(char* first, char* last, uint8_t value, num_base base)
    {
        return to_chars_integral_impl(first, last, value, base);
    }

    size_t to_chars(char* first, char* last, int16_t value, num_base base)
    {
        return to_chars_integral_impl(first, last, value, base);
    }

    size_t to_chars(char* first, char* last, uint16_t value, num_base base)
    {
        return to_chars_integral_impl(first, last, value, base);
    }

    size_t to_chars(char* first, char* last, int32_t value, num_base base)
    {
        return to_chars_integral_impl(first, last, value, base);
    }

    size_t to_chars(char* first, char* last, uint32_t value, num_base base)
    {
        return to_chars_integral_impl(first, last, value, base);
    }

    void to_chars(char* first, char* last, float value, int precision)
    {
        if (first >= last)
        {
            return;
        }

        bool minus = value < 0;

        if (minus)
        {
            value = -value;
        }

        uint32_t int_part = static_cast<int>(value);

        float float_part = value - int_part;
        bool after_dot = float_part > 0 && precision > 0;

        uint32_t float_precision = float_part * pow10(precision);

        size_t int_part_len = hlib::digits_count(int_part);
        size_t float_part_len = 0;

        if (after_dot)
        {
            // dot
            float_part_len += 1;
            float_part_len += hlib::digits_count(float_precision);
        }

        if (distance(first, last) < int_part_len + float_part_len)
        {
            return;
        }

        if (minus)
        {
            *first = '-';
            first++;
        }

        to_chars(first, first + int_part_len, int_part);

        if (after_dot)
        {
            *(first + int_part_len) = '.';
            to_chars(
                first + int_part_len + 1, first + int_part_len + float_part_len, float_precision);
        }
    }
} // namespace hlib