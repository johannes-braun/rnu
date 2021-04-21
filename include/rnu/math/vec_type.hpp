#pragma once
#include <utility>
#include <array>
#include <complex>
#include "traits.hpp"

namespace rnu {
  template<typename T, size_t S>
  class vec;

  namespace detail {

    template<typename T>
    struct vec_tuple;

    template<typename T, size_t I>
    struct vec_tuple<rnu::vec<T, I>>
    {
      using type = decltype(std::tuple_cat(std::tuple<T>{}, typename vec_tuple<vec<T, I - 1>>::type{}));
    };

    template<typename T>
    struct vec_tuple<rnu::vec<T, 0>>
    {
      using type = std::tuple<>;
    };

    template<typename T>
    using vec_tuple_t = typename vec_tuple<T>::type;

    template<typename T, size_t S>
    struct vec_components
    {
      std::array<T, S> components{ 0 };
    };

    template<typename T>
    struct vec_components<T, 1>
    {
      union
      {
        std::array<T, 1> components{ 0 };
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
        std::array<T, 2> components{ 0 };
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
        std::array<T, 3> components{ 0 };
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
        std::array<T, 4> components{ 0 };
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

    [[nodiscard]] iterator begin() { return std::begin(this->components); }
    [[nodiscard]] iterator end() { return std::end(this->components); }
    [[nodiscard]] const_iterator begin() const { return std::begin(this->components); }
    [[nodiscard]] const_iterator end() const { return std::end(this->components); }
    [[nodiscard]] const_iterator cbegin() const { return std::begin(this->components); }
    [[nodiscard]] const_iterator cend() const { return std::end(this->components); }
    [[nodiscard]] reverse_iterator rbegin() { return std::rbegin(this->components); }
    [[nodiscard]] reverse_iterator rend() { return std::rend(this->components); }
    [[nodiscard]] const_reverse_iterator rbegin() const { return std::rbegin(this->components); }
    [[nodiscard]] const_reverse_iterator rend() const { return std::rend(this->components); }
    [[nodiscard]] const_reverse_iterator crbegin() const { return std::crbegin(this->components); }
    [[nodiscard]] const_reverse_iterator crend() const { return std::crend(this->components); }

    constexpr vec() noexcept;

    template<typename X, size_type D> requires std::convertible_to<X, T>
    constexpr vec(const vec<X, D>& other) noexcept;
    template<typename X> requires std::convertible_to<X, T>
    explicit constexpr vec(const X* ptr);
    template<typename X> requires std::convertible_to<X, T>
    explicit constexpr vec(X* ptr);

    explicit constexpr vec(T&& value) noexcept;

    template<typename... Ts> requires (S > 1 && S == sizeof...(Ts)) &&
      (std::convertible_to<Ts, T> && ...)
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

    constexpr detail::vec_tuple_t<vec<T, S>> tuple() const noexcept { return std::bit_cast<detail::vec_tuple_t<vec<T, S>>>(this->components); }

    [[nodiscard]] constexpr operator bool() const noexcept
      requires std::same_as<T, bool>
    {
      for (size_t i = 0; i < component_count; ++i)
        if (!at(i)) return false;
      return true;
    }

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
