#pragma once

#include <stddef.h>

#include "format.hpp"

namespace hlib
{
    class string_view
    {
    public:
        using element_type = char;
        using size_type = size_t;
        using reference = char&;
        using const_reference = const char&;
        using pointer = char*;
        using const_pointer = const char*;
        using iterator = char*;
        using const_iterator = const char*;

        constexpr string_view()
        {
        }

        constexpr string_view(const_iterator begin, const_iterator end)
            : begin_(const_cast<iterator>(begin)), end_(const_cast<iterator>(end))
        {
        }

        constexpr string_view(const_iterator begin, size_type size)
            : begin_(const_cast<iterator>(begin)), end_(const_cast<iterator>(begin + size))
        {
        }

        template<const size_type Size>
        constexpr string_view(const element_type (&begin)[Size])
            : begin_(const_cast<iterator>(begin)), end_(const_cast<iterator>(begin + Size - 1))
        {
        }

        constexpr string_view(const string_view& other) : begin_(other.begin_), end_(other.end_)
        {
        }

        constexpr reference front() const
        {
            return *begin_;
        }

        constexpr reference back() const
        {
            return *(end_ - 1);
        }

        constexpr pointer data() const
        {
            return begin_;
        }

        constexpr iterator begin() const
        {
            return begin_;
        }

        constexpr iterator end() const
        {
            return end_;
        }

        constexpr bool empty() const
        {
            return begin_ == end_;
        }

        constexpr size_type size()
        {
            return end_ - begin_;
        }

        constexpr string_view& operator=(const string_view& other)
        {
            begin_ = other.begin_;
            end_ = other.end_;

            return *this;
        }

        constexpr reference operator[](const size_type i) const
        {
            return begin_[i];
        }

    private:
        pointer begin_ = nullptr;
        pointer end_ = nullptr;
    };

    template<>
    struct formatter<string_view>
    {
        void parse(const char* /*fmt*/)
        {
        }

        template<typename Output>
        void format(Output output, string_view str)
        {
            for (auto it : str)
            {
                output(it);
            }
        }
    };
} // namespace hlib
