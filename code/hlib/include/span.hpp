#pragma once

#include <stddef.h>

namespace hlib
{
    template<typename T>
    class span
    {
    public:
        using element_type = T;
        using size_type = size_t;
        using reference = T&;
        using const_reference = const T&;
        using pointer = T*;
        using const_pointer = const T*;
        using iterator = T*;
        using const_iterator = const T*;

        constexpr span()
        {
        }

        constexpr span(iterator begin, iterator end) : begin_(begin), end_(end)
        {
        }

        constexpr span(iterator begin, size_type size) : begin_(begin), end_(begin + size)
        {
        }

        template<const size_type Size>
        constexpr span(element_type (&begin)[Size]) : begin_(begin), end_(begin + Size)
        {
        }

        constexpr span(const span& other) : begin_(other.begin_), end_(other.end_)
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

        constexpr span& operator=(const span& other)
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
} // namespace hlib
