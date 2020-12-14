#pragma once

#include <stddef.h>
#include <stdint.h>

#include "array.hpp"
#include "optional.hpp"

namespace hlib
{
    template<typename T, size_t Size>
    class circular_buffer
    {
    public:
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using size_type = size_t;

    public:
        constexpr void push_back(const value_type& value)
        {
            if (!full())
            {
                buffer_[tail_] = value;
                tail_ = (tail_ + 1) % Size;
            }
        }

        constexpr hlib::optional<value_type> pop()
        {
            if (!empty())
            {
                value_type value = buffer_[head_];

                buffer_[head_] = value_type();

                head_ = (head_ + 1) % Size;

                return value;
            }

            return {};
        }

        constexpr iterator front()
        {
            return buffer_[head_];
        }

        constexpr const_iterator front() const
        {
            return buffer_[head_];
        }

        constexpr iterator back()
        {
            return buffer_[tail_];
        }

        constexpr const_iterator back() const
        {
            return buffer_[tail_];
        }

        constexpr T* data()
        {
            return buffer_.data();
        }

        constexpr bool empty()
        {
            return head_ == tail_;
        }

        constexpr bool full()
        {
            return tail_ == (head_ - 1) % Size;
        }

    private:
        size_t head_ = 0;
        size_t tail_ = 0;
        hlib::array<T, Size> buffer_;
    };
} // namespace hlib
