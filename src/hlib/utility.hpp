#pragma once

#include "type_traints.hpp"

namespace hlib
{
    template<typename T>
    constexpr typename remove_reference<T>::type&& move(T&& t)
    {
        return static_cast<typename remove_reference<T>::type&&>(t);
    }

    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type& t)
    {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type&& t)
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
