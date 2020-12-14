#pragma once

#include "aligned_storage.hpp"
#include "optional.hpp"
#include "type_traits.hpp"

namespace hlib
{
    namespace detail
    {
        template<auto... Ts>
        struct max_value;

        template<auto Value>
        struct max_value<Value>
        {
            static inline constexpr auto value = Value;
        };

        template<auto First, auto Second>
        struct max_value<First, Second> : max_value<(First > Second ? First : Second)>
        {
        };

        template<auto First, auto Second, auto... Ts>
        struct max_value<First, Second, Ts...> : max_value<(First > Second ? First : Second), Ts...>
        {
        };

        template<auto... Ts>
        static constexpr auto max_value_v = max_value<Ts...>::value;

        template<size_t Index, typename T, typename... Ts>
        struct index_of;

        template<size_t Index, typename T, typename... Ts>
        struct index_of<Index, T, T, Ts...>
        {
            static constexpr size_t value = Index;
        };

        template<size_t Index, typename T, typename U, typename... Ts>
        struct index_of<Index, T, U, Ts...> : index_of<Index + 1, T, Ts...>
        {
        };

        template<typename T, typename... Ts>
        static constexpr auto index_of_v = index_of<0, T, Ts...>::value;
    }; // namespace detail

    template<typename... Ts>
    class variant
    {
        static_assert((is_trivial<Ts>::value && ...), "variant supports only trivial types");

    public:
        inline static constexpr size_t buffer_size = detail::max_value_v<sizeof(Ts)...>;
        inline static constexpr size_t buffer_align = detail::max_value_v<alignof(Ts)...>;

        using buffer_t = typename hlib::aligned_storage<buffer_size, buffer_align>::type;

    public:
        template<typename T>
        variant(const T& value)
        {
            *reinterpret_cast<T*>(buffer_.data) = value;

            type_id_ = detail::index_of_v<T, Ts...>;
        }

        template<typename T>
        auto operator=(const T& value)
        {
            *reinterpret_cast<T*>(buffer_.data) = value;

            type_id_ = detail::index_of_v<T, Ts...>;

            return *this;
        }

        template<typename T>
        bool has()
        {
            return detail::index_of_v<T, Ts...> == type_id_;
        }

        template<typename T>
        hlib::optional<T> get()
        {
            if (has<T>())
            {
                return *reinterpret_cast<T*>(buffer_.data);
            }

            return {};
        }

    private:
        buffer_t buffer_;
        size_t type_id_;
    };
} // namespace hlib
