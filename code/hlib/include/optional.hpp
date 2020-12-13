#pragma once

#include "utility.hpp"
namespace hlib
{
    template<typename T>
    class optional
    {
    public:
        using value_type = T;

    public:
        optional() : value_(), has_value_(false)
        {
        }

        optional(const T& value) : value_(value), has_value_(true)
        {
        }

        optional(const optional<T>& rth) : value_(rth.value_), has_value_(rth.has_value_)
        {
        }

        optional(optional<T>&& rth)
            : value_(hlib::move(rth.value_)), has_value_(hlib::move(rth.value_))
        {
        }

        optional<value_type>& operator=(const value_type& value)
        {
            value_ = value;
            has_value_ = true;
        }

        constexpr void reset()
        {
            has_value_ = false;
        }

        constexpr value_type value()
        {
            return value_;
        }

        constexpr bool has_value()
        {
            return has_value_;
        }

    private:
        value_type value_{};
        bool has_value_ = false;
    };
} // namespace hlib
