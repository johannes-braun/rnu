namespace rnu {
    template<typename T, size_t S>
    template<std::size_t... Is>
    constexpr vec<T, S>::vec(std::index_sequence<Is...>, T&& value) noexcept : detail::vec_components<T, S>{ (static_cast<void>(Is), value)... }
    {}

    template<typename T, size_t S>
    template<typename X, size_t D, std::size_t... Is>
    constexpr vec<T, S>::vec(std::index_sequence<Is...>, const vec<X, D>& other) noexcept
        : detail::vec_components<T, S>{ static_cast<T>(other[Is])... }
    {}

    template<typename T, size_t S>
    template<typename X, std::size_t... Is>
    constexpr vec<T, S>::vec(std::index_sequence<Is...>, const X* other) noexcept : detail::vec_components<T, S>{ static_cast<T>(other[Is])... }
    {}

    template<typename T, size_t S>
    constexpr vec<T, S>::vec() noexcept : vec(T{})
    {}

    template<typename T, size_t S>
    template<typename X, size_t D, typename>
    constexpr vec<T, S>::vec(const vec<X, D>& other) noexcept : vec(std::make_index_sequence<std::min(S, D)>(), other)
    {}

    template<typename T, size_t S>
    template<typename X, typename>
    constexpr vec<T, S>::vec(const X* ptr) : vec(std::make_index_sequence<S>(), ptr)
    {}

    template<typename T, size_t S>
    template<typename X, typename>
    constexpr vec<T, S>::vec(X* ptr) : vec(std::make_index_sequence<S>(), ptr)
    {}

    template<typename T, size_t S>
    constexpr vec<T, S>::vec(T&& value) noexcept : vec(std::make_index_sequence<S>(), std::forward<T&&>(value))
    {}

    template<typename T, size_t S>
    template<typename... Ts, typename>
    constexpr vec<T, S>::vec(Ts&&... ts) noexcept : detail::vec_components<T, S>{ static_cast<value_type>(ts)... }
    {}

    template<typename T, size_t S>
    template<std::size_t ...Is, typename UnaryConvertFun>
    inline constexpr auto vec<T, S>::apply(std::index_sequence<Is...>, UnaryConvertFun&& fun) const noexcept
    {
        using type = decltype(fun(this->components[0]));
        if constexpr (std::is_same_v< type, void>)
            (fun(this->components[Is]), ...);
        else
            return vec<type, S>(fun(this->components[Is])...);
    }

    template<typename T, size_t S>
    template<std::size_t ...Is, typename UnaryConvertFun>
    inline constexpr auto vec<T, S>::apply(std::index_sequence<Is...>, const vec& other, UnaryConvertFun&& fun) const noexcept
    {
        using type = decltype(fun(this->components[0], other.components[0]));
        if constexpr (std::is_same_v<type, void>)
            (fun(this->components[Is], other.components[Is]), ...);
        else
            return vec<type, S>(fun(this->components[Is], other.components[Is])...);
    }

    template<typename T, size_t S>
    inline constexpr auto vec<T, S>::real() const noexcept
    {
        return apply(std::make_index_sequence<S>{}, [](const auto& x) { return std::real(x); });
    }

    template<typename T, size_t S>
    inline constexpr auto vec<T, S>::imag() const noexcept
    {
        return apply(std::make_index_sequence<S>{}, [](const auto& x) { return std::real(x); });
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::reference vec<T, S>::at(size_type index)
    {
        return detail::vec_components<T, S>::components[index];
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::const_reference vec<T, S>::at(size_type index) const
    {
        return detail::vec_components<T, S>::components[index];
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::reference vec<T, S>::operator[](size_type index)
    {
        return detail::vec_components<T, S>::components[index];
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::const_reference vec<T, S>::operator[](size_type index) const
    {
        return detail::vec_components<T, S>::components[index];
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::pointer vec<T, S>::data() noexcept
    {
        return this->components;
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::const_pointer vec<T, S>::data() const noexcept
    {
        return this->components;
    }

    template<typename T, size_t S>
    constexpr typename vec<T, S>::size_type vec<T, S>::size() const noexcept
    {
        return S;
    }

    template<typename T, size_t S>
    constexpr void vec<T, S>::fill(const T& value)
    {
        std::fill_n(this->components, S, value);
    }

}