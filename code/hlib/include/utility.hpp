#pragma once

#include "type_traits.hpp"

namespace hlib
{
    template<typename T>
    constexpr remove_reference_t<T>&& move(T&& t)
    {
        return static_cast<remove_reference_t<T>&&>(t);
    }

    template<typename T>
    constexpr T&& forward(remove_reference_t<T>& t)
    {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr T&& forward(remove_reference_t<T>&& t)
    {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr void swap(T& a, T& b)
    {
        T c(move(a));
        a = move(b);
        b = move(c);
    }

    template<class T, class U = T>
    constexpr T exchange(T& obj, U&& new_value)
    {
        T old_value = move(obj);
        obj = forward<U>(new_value);
        return old_value;
    }
} // namespace hlib
