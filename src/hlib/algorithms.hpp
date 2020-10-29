#pragma once

#include <stddef.h>

#include "utility.hpp"

namespace hlib
{
    template<typename InputIterator, typename OutputIterator>
    constexpr void copy(InputIterator in_first, InputIterator in_last, OutputIterator out_first)
    {
        if (in_first != in_last)
        {
            while (in_first != in_last)
            {
                *out_first = *in_first;

                ++in_first;
                ++out_first;
            }
        }
    }

    template<typename InputIterator, typename OutputIterator>
    constexpr void copy_n(InputIterator in_first, size_t n, OutputIterator out_first)
    {
        while (n > 0)
        {
            *out_first = *in_first;

            --n;
            ++in_first;
            ++out_first;
        }
    }

    template<typename InputIterator, typename OutputIterator>
    constexpr void move_n(InputIterator in_first, size_t n, OutputIterator out_first)
    {
        while (n > 0)
        {
            *out_first = move(*in_first);

            --n;
            ++in_first;
            ++out_first;
        }
    }

    template<typename Interator, typename T>
    constexpr void fill(Interator first, Interator last, const T& value)
    {
        while (first != last)
        {
            *first = value;
            ++first;
        }
    }

    template<typename Interator, typename T>
    constexpr void fill_n(Interator first, size_t n, const T& value)
    {
        while (n > 0)
        {
            *first = value;
            ++first;
            --n;
        }
    }

    template<typename T>
    constexpr const T& min(const T& a, const T& b)
    {
        return a <= b ? a : b;
    }

    template<typename T>
    constexpr const T& max(const T& a, const T& b)
    {
        return a >= b ? a : b;
    }

    template<typename T>
    constexpr const T& clamp(const T& v, const T& lo, const T& hi)
    {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }
} // namespace hlib
