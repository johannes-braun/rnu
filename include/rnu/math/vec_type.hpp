#pragma once
#include <utility>
#include <array>
#include <complex>
#include "traits.hpp"

namespace rnu {
    namespace detail {
        constexpr size_t max_static_vector_size = 24;

        template<typename T, size_t S>
        struct vec_components
        {
            T components[S]{ 0 };
        };

        template<typename T>
        struct vec_components<T, 1>
        {
            union
            {
                T components[1]{ 0 };
                struct
                {
                    T x;
                };
                struct
                {
                    T r;
                };
                struct
                {
                    T u;
                };
            };
        };

        template<typename T>
        struct vec_components<T, 2>
        {
            union
            {
                T components[2]{ 0 };
                struct
                {
                    T x, y;
                };
                struct
                {
                    T r, g;
                };
                struct
                {
                    T u, v;
                };
            };
        };

        template<typename T>
        struct vec_components<T, 3>
        {
            union
            {
                T components[3]{ 0 };
                struct
                {
                    T x, y, z;
                };
                struct
                {
                    T r, g, b;
                };
                struct
                {
                    T u, v, w;
                };
            };
        };

        template<typename T>
        struct vec_components<T, 4>
        {
            union
            {
                T components[4]{ 0 };
                struct
                {
                    T x, y, z, w;
                };
                struct
                {
                    T r, g, b, a;
                };
                struct
                {
                    T u, v, s, t;
                };
            };
        };
    }    // namespace detail

    template<typename T>
    struct vec_tuple;

    template<size_t I, typename T>
    struct vec_tuple<vec<T, I>>
    {
        template<typename... Args>
        using type = typename vec_tuple<vec<T, I - 1>>::template type<T, Args...>;
    };

    template<typename T>
    struct vec_tuple<vec<T, 0>>
    {
        template<typename... Args>
        using type = std::tuple<Args...>;
    };
    template<typename T>
    using vec_tuple_t = typename vec_tuple<T>::template type<>;

    template<typename T, size_t S>
    class vec : public detail::vec_components<T, S>
    {
    public:
        using reference = T&;
        using const_reference = const T&;
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using scalar_type = scalar_type_of_t<value_type>;

        using iterator = typename std::array<value_type, S>::iterator;
        using const_iterator = typename std::array<value_type, S>::const_iterator;
        using reverse_iterator = typename std::array<value_type, S>::reverse_iterator;
        using const_reverse_iterator = typename std::array<value_type, S>::const_reverse_iterator;

        constexpr static size_t component_count = S;

        [[nodiscard]] auto begin() { return reinterpret_cast<std::array<T, S>&>(this->components).begin(); }
        [[nodiscard]] auto end() { return reinterpret_cast<std::array<T, S>&>(this->components).end(); }
        [[nodiscard]] auto begin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).begin(); }
        [[nodiscard]] auto end() const { return reinterpret_cast<const std::array<T, S>&>(this->components).end(); }
        [[nodiscard]] auto cbegin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).begin(); }
        [[nodiscard]] auto cend() const { return reinterpret_cast<const std::array<T, S>&>(this->components).end(); }
        [[nodiscard]] auto rbegin() { return reinterpret_cast<std::array<T, S>&>(this->components).rbegin(); }
        [[nodiscard]] auto rend() { return reinterpret_cast<std::array<T, S>&>(this->components).rend(); }
        [[nodiscard]] auto rbegin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rbegin(); }
        [[nodiscard]] auto rend() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rend(); }
        [[nodiscard]] auto crbegin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rbegin(); }
        [[nodiscard]] auto crend() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rend(); }

        constexpr vec() noexcept;
        template<typename X, size_type D, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
        explicit constexpr vec(const vec<X, D>& other) noexcept;
        template<typename X, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
        explicit constexpr vec(const X* ptr);
        template<typename X, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
        explicit constexpr vec(X* ptr);
        explicit constexpr vec(T&& value) noexcept;
        template<typename... Ts, typename = std::enable_if_t<S == sizeof...(Ts) && (S > 1)>>
        explicit constexpr vec(Ts&&... ts) noexcept;

    private:
        template<std::size_t... Is, typename UnaryConvertFun>
        constexpr auto apply(std::index_sequence<Is...>, UnaryConvertFun&& fun) const noexcept;
        template<std::size_t... Is, typename UnaryConvertFun>
        constexpr auto apply(std::index_sequence<Is...>, const vec& other, UnaryConvertFun&& fun) const noexcept;

    public:
        constexpr auto real() const noexcept;
        constexpr auto imag() const noexcept;

        constexpr reference       at(size_type index);
        constexpr const_reference at(size_type index) const;
        constexpr reference       operator[](size_type index);
        constexpr const_reference operator[](size_type index) const;
        constexpr pointer         data() noexcept;
        constexpr const_pointer   data() const noexcept;
        constexpr size_type       size() const noexcept;
        constexpr void            fill(const T& value);

        constexpr operator vec_tuple_t<vec>& () noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
        constexpr operator const vec_tuple_t<vec>& () noexcept { return reinterpret_cast<const vec_tuple_t<vec>&>(*this); }
        constexpr vec_tuple_t<vec>& tuple() noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
        constexpr const vec_tuple_t<vec>& tuple() const noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }

    private:
        template<std::size_t... Is>
        explicit constexpr vec(std::index_sequence<Is...>, T&& value) noexcept;
        template<typename X, size_type D, std::size_t... Is>
        explicit constexpr vec(std::index_sequence<Is...>, const vec<X, D>& other) noexcept;
        template<typename X, std::size_t... Is>
        explicit constexpr vec(std::index_sequence<Is...>, const X* other) noexcept;
    };

    template<typename T>
    class vec<T, 0>
    {};
}

#include "vec_type.inl.hpp"
