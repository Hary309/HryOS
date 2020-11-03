#pragma once

#include <stddef.h>

namespace hlib
{
    template<typename T, size_t N>
    struct array
    {
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using size_type = size_t;

        constexpr T& operator[](size_type pos)
        {
            return data_[pos];
        }

        constexpr const T& operator[](size_type pos) const
        {
            return data_[pos];
        }

        constexpr size_type size() const
        {
            return N;
        }

        constexpr iterator begin()
        {
            return &data_[0];
        }

        constexpr const_iterator begin() const
        {
            return &data_[0];
        }

        constexpr iterator end()
        {
            return &data_[N];
        }

        constexpr const_iterator end() const
        {
            return &data_[N];
        }

        constexpr value_type* data()
        {
            return data_;
        }

    public:
        T data_[N];
    };
} // namespace hlib
