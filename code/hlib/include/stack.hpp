#pragma once

#include <stdint.h>

#include "algorithm.hpp"
#include "type_traits.hpp"

namespace hlib
{
    class stack
    {
    public:
        stack(uint32_t size) : size_(size)
        {
            bottom_ = new uint8_t[size];
            top_ = bottom_ + size;

            fill_n(top_, size, 0u);
        }

        template<typename T>
        void push(T&& value)
        {
            static_assert(hlib::is_trivial_v<T>, "T has to be trivial!");

            auto new_top = top_ - sizeof(T);

            if (new_top >= bottom_)
            {
                top_ = new_top;
                *reinterpret_cast<T*>(top_) = value;
            }
        }

        template<typename T>
        T pop()
        {
            static_assert(hlib::is_trivial_v<T>, "T has to be trivial!");

            auto new_top = top_ + sizeof(T);

            if (new_top > bottom_ > size_)
            {
                return {};
            }

            T value = reinterpret_cast<T*>(top_);
            top_ = new_top;

            return value;
        }

    private:
        uint8_t* bottom_ = nullptr;
        uint8_t* top_ = nullptr;
        uint8_t size_ = 0;
    };
} // namespace hlib
