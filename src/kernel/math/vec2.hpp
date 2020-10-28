#pragma once

template<typename T>
struct vec2
{
    T x;
    T y;

    constexpr vec2() = default;
    constexpr vec2(T x, T y) : x(x), y(y) {}
    constexpr explicit vec2(T value) : x(value), y(value) {}
};

template<typename T>
constexpr vec2<T> operator-(const vec2<T>& right)
{
    return { -right.x, -right.y };
}

template<typename T>
constexpr vec2<T>& operator+=(vec2<T>& left, const vec2<T>& right)
{
    left.x += right.x;
    left.y += right.y;

    return left;
}

template<typename T>
constexpr vec2<T>& operator-=(vec2<T>& left, const vec2<T>& right)
{
    left.x -= right.x;
    left.y -= right.y;

    return left;
}

template<typename T>
constexpr vec2<T> operator+(const vec2<T>& left, const vec2<T>& right)
{
    return { left.x + right.x, left.y + right.y };
}

template<typename T>
constexpr vec2<T> operator+(const vec2<T>& left, T right)
{
    return { left.x + right, left.y + right };
}

template<typename T>
constexpr vec2<T> operator-(const vec2<T>& left, const vec2<T>& right)
{
    return { left.x - right.x, left.y - right.y };
}

template<typename T>
constexpr vec2<T> operator-(const vec2<T>& left, T right)
{
    return { left.x - right, left.y - right };
}

template<typename T>
constexpr vec2<T> operator*(const vec2<T>& left, T right)
{
    return { left.x * right, left.y * right };
}

template<typename T>
constexpr vec2<T> operator*(T left, const vec2<T>& right)
{
    return { left.x * right, left.y * right };
}

template<typename T>
constexpr vec2<T>& operator*=(vec2<T>& left, T right)
{
    left.x *= right;
    left.y *= right;

    return left;
}

template<typename T>
constexpr vec2<T> operator/(const vec2<T>& left, T right)
{
    return { left.x / right, left.y / right };
}

template<typename T>
constexpr vec2<T>& operator/=(vec2<T>& left, T right)
{
    left.x /= right;
    left.y /= right;

    return left;
}

template<typename T>
constexpr vec2<T>& operator/=(vec2<T>& left, const vec2<T>& right)
{
    left.x /= right.x;
    left.y /= right.y;

    return left;
}

template<typename T>
constexpr bool operator==(const vec2<T>& left, const vec2<T>& right)
{
    return left.x == right.x && left.y == right.y;
}

// TODO: add spaceship operator

using vec2f = vec2<float>;
using vec2i = vec2<int>;
using vec2u = vec2<unsigned int>;
