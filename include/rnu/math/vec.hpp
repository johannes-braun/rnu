#pragma once
#include <array>
#include <concepts>
#include <algorithm>
#include <rnu/math/cx_fun.hpp>

#include "traits.hpp"

namespace rnu
{
  namespace detail {
    template<typename T>
    struct count {
      static constexpr size_t value = 1;
    };
    template<typename T, size_t S>
    struct count<vec<T, S>> {
      static constexpr size_t value = S;
    };
    template<typename T, size_t S>
    struct count<std::array<T, S>> {
      static constexpr size_t value = S;
    };

    template<typename T>
    static constexpr size_t count_v = count<std::decay_t<T>>::value;

    template<typename... Ts>
    concept contains_vectors = ((count_v<Ts> > 1) || ...);

    template<typename T, size_t S, element_type_convertible_to<T> Elem>
    constexpr void emplace(std::array<T, S>& array, size_t& index, Elem&& elem)
    {
      for (auto const& item : elem) {
        array[index] = static_cast<T>(item);
        ++index;
      }
    }

    template<typename T, size_t S, typename Elem>
    constexpr void emplace(std::array<T, S>& array, size_t& index, Elem&& elem)
      requires std::convertible_to<Elem, T>
    {
      array[index] = static_cast<T>(std::forward<Elem>(elem));
      ++index;
    }

    template<typename T> requires requires(T t, size_t i) { {t[i]}; }
    constexpr decltype(auto) get(T& t, size_t index) { return t[index]; }
    template<typename T>
    constexpr decltype(auto) get(T& t, size_t index) { return t; }
    template<typename T>
    constexpr decltype(auto) get(T&& t, size_t index) { return get(static_cast<T&>(t), index); }
  }

  template<typename... Ts>
  consteval bool equivalent() {
    size_t size = 0;
    const auto apply = [&](size_t val) -> bool {
      if (size == 0 && val > 1)
        size = val;
      return size == val || 1 == val;
    };

    return (apply(detail::count_v<Ts>) && ...);
  }

  template<typename... Ts>
  constexpr size_t max_size() {
    size_t max = 0;
    ((max = max < detail::count_v<Ts> ? detail::count_v<Ts> : max), ...);
    return max;
  }

  template<typename Fun, typename... T>
  constexpr decltype(auto) get_all(Fun&& fun, size_t index, T&&... ts)
  {
    return fun(detail::get(std::forward<T>(ts), index)...);
  }

  template<typename Fun, size_t... Indices, typename... T>
  constexpr auto apply_impl(Fun&& fun, std::index_sequence<Indices...>, T&&... ts)
  {
    using rv = decltype(get_all(std::forward<Fun>(fun), 0, std::forward<T>(ts)...));
    constexpr auto count = max_size<T...>();

    if constexpr (std::same_as<rv, void>)
      (get_all(std::forward<Fun>(fun), Indices, std::forward<T>(ts)...), ...);
    else if constexpr (std::is_reference_v<rv>)
      return vec<std::reference_wrapper<std::decay_t<rv>>, count>(get_all(std::forward<Fun>(fun), Indices, std::forward<T>(ts)...)...);
    else
      return vec<std::decay_t<rv>, count>(get_all(std::forward<Fun>(fun), Indices, std::forward<T>(ts)...)...);
  }

  template<typename Fun, typename... T>
  concept applicable = requires(Fun && fun, T&&... ts) {
    apply(std::forward<Fun>(fun), std::forward<T>(ts)...);
  };

  template<typename... T>
  concept equivalent_types = (vector<T> || ...) && (equivalent<T...>());

  template<typename Fun, typename... T>
  requires equivalent_types<T...> && requires(Fun&& fun, T&&... ts) { get_all(std::forward<Fun>(fun), 0, std::forward<T>(ts)...); }
  constexpr auto apply(Fun&& fun, T&&... ts) {
    return apply_impl(std::forward<Fun>(fun), std::make_index_sequence<max_size<T...>()>(), std::forward<T>(ts)...);
  }

#define vec_base_ctors_impl() \
  constexpr vec_base() = default; \
  template<typename... Ts> constexpr vec_base(Ts&&... ts) : _storage{ std::forward<Ts>(ts)... } {}\
  template<size_t... Indices, typename T> constexpr vec_base(std::index_sequence<Indices...>, T && t) : _storage{ (Indices, std::forward<T>(t))... } {}

  template<typename T, size_t S>
  struct vec_base {
    std::array<T, S> _storage{};

  protected:
    vec_base_ctors_impl();
  };
  template<typename T>
  struct vec_base<T, 1> {
    union {
      std::array<T, 1> _storage{};
      struct { T x; };
      struct { T r; };
    };

  protected:
    vec_base_ctors_impl();
  };
  template<typename T>
  struct vec_base<T, 2> {
    union {
      std::array<T, 2> _storage{};
      struct { T x, y; };
      struct { T r, g; };
    };

  protected:
    vec_base_ctors_impl();
  };
  template<typename T>
  struct vec_base<T, 3> {
    union {
      std::array<T, 3> _storage{};
      struct { T x, y, z; };
      struct { T r, g, b; };
    };

  protected:
    vec_base_ctors_impl();
  };

  template<typename T>
  struct vec_base<T, 4> {
    union {
      std::array<T, 4> _storage{};
      struct { T x, y, z, w; };
      struct { T r, g, b, a; };
    };

  protected:
    vec_base_ctors_impl();
  };

#undef vec_base_ctors_impl

  template<typename T, size_t S>
  struct vec : public vec_base<T, S> {
    using value_type = T;
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;

    using iterator = typename std::array<T, S>::iterator;
    using const_iterator = typename std::array<T, S>::const_iterator;
    using reverse_iterator = typename std::array<T, S>::reverse_iterator;
    using const_reverse_iterator = typename std::array<T, S>::const_reverse_iterator;

    static constexpr size_type count = S;

    template<typename... Ts>
    constexpr vec(Ts&&... components)
      requires (sizeof...(Ts) == count) && (std::convertible_to<Ts, T> && ...)
      : vec_base<T, S>(static_cast<T>(std::forward<Ts>(components))...)
    {
    }

    constexpr vec(T value)
      : vec_base<T, S>{ std::make_index_sequence<S>(), value }
    {
    }

    template<vector V>
    constexpr vec(V other) requires (V::count >= count)
    {
      for (size_t i = 0; i < count; ++i)
        at(i) = static_cast<T>(other[i]);
    }

    template<typename... Ts
//#if __INTELLISENSE__
      //, std::enable_if_t<
//#else
    > requires
//#endif
      ((std::less<size_t>{}(sizeof...(Ts), S)) && ((detail::count_v<Ts> +...) == S) && (recursively_convertible_to<Ts, T> && ...) && (std::less<size_t>{}(1, detail::count_v<Ts>) || ...))
//#if __INTELLISENSE__
      //, int > * = nullptr
    //>
//#endif
      constexpr vec(Ts&&... compounds)
    {
      size_t index = 0;
      (detail::emplace(this->_storage, index, std::forward<Ts>(compounds)), ...);
    }

    constexpr vec() : vec(T(0)) {}

    [[nodiscard]] constexpr size_type size() const { return vec::count; }
    [[nodiscard]] constexpr pointer data() { return this->_storage.data(); }
    [[nodiscard]] constexpr const_pointer data() const { return this->_storage.data(); }

    [[nodiscard]] constexpr iterator begin() { return std::begin(this->_storage); }
    [[nodiscard]] constexpr const_iterator begin() const { return std::begin(this->_storage); }
    [[nodiscard]] constexpr const_iterator cbegin() const { return std::cbegin(this->_storage); }
    [[nodiscard]] constexpr reverse_iterator rbegin() { return std::rbegin(this->_storage); }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const { return std::rbegin(this->_storage); }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const { return std::crbegin(this->_storage); }

    [[nodiscard]] constexpr iterator end() { return std::end(this->_storage); }
    [[nodiscard]] constexpr const_iterator end() const { return std::end(this->_storage); }
    [[nodiscard]] constexpr const_iterator cend() const { return std::cend(this->_storage); }
    [[nodiscard]] constexpr reverse_iterator rend() { return std::rend(this->_storage); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const { return std::rend(this->_storage); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const { return std::crend(this->_storage); }

    [[nodiscard]] constexpr reference operator[](size_t index) { return this->_storage[index]; }
    [[nodiscard]] constexpr const_reference operator[](size_t index) const { return this->_storage[index]; }
    [[nodiscard]] constexpr reference at(size_t index) { return this->_storage[index]; }
    [[nodiscard]] constexpr const_reference at(size_t index) const { return this->_storage[index]; }

    [[nodiscard]] constexpr operator bool() const noexcept
      requires std::same_as<T, bool>
    {
      return all();
    }

    [[nodiscard]] constexpr bool any() const noexcept requires std::same_as<T, bool> {
      auto const* d = data();
      auto const s = size();
      return std::any_of(d, d + s, [](auto const& v) { return v; });
    }

    [[nodiscard]] constexpr bool all() const noexcept requires std::same_as<T, bool> {
      auto const* d = data();
      auto const s = size();
      return std::all_of(d, d + s, [](auto const& v) { return v; });
    }
  };

  namespace detail {
    template<typename T, std::size_t... Is>
    constexpr auto dot_impl(std::index_sequence<Is...>, const T* s1, const T* s2) noexcept
    {
      return ((to_float(s1[Is]) * to_float(s2[Is])) + ...);
    }
  }    // namespace detail

#define assign_op(Ope) \
    template<typename Lhs, typename Rhs> \
    constexpr decltype(auto) operator Ope(Lhs& lhs, Rhs&& rhs) \
      requires uses_vector_arithmetic<Lhs, Rhs> && equivalent_types<Lhs, Rhs>&& requires(basic_scalar_t<Lhs>& lhs, basic_scalar_t<Rhs> rhs) { lhs Ope rhs; } \
    { apply([](auto&& l, auto&& r) { l Ope r; }, std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)); return lhs; }

#define arithmetic_op(Op) \
  template<typename Lhs, typename Rhs> \
    [[nodiscard]] constexpr auto operator Op(Lhs&& lhs, Rhs&& rhs) \
      requires uses_vector_arithmetic<Lhs, Rhs> && equivalent_types<Lhs, Rhs>&& requires(basic_scalar_t<Lhs> lhs, basic_scalar_t<Rhs> rhs) { lhs Op rhs; } \
    { return apply([](auto&& l, auto&& r) { return l Op r; }, std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)); }

#define unary_prefix_op(Op) \
  template<typename T> \
    [[nodiscard]] constexpr auto operator Op(T&& value) \
      requires vector<T> && requires(basic_scalar_t<T> value) { Op value; } \
    { return apply([](auto&& v) { return Op v; }, std::forward<T>(value)); }

#define arithmetic_op_with_assign(Op)\
    arithmetic_op(Op) \
    assign_op(Op=)

  arithmetic_op_with_assign(+);
  arithmetic_op_with_assign(*);
  arithmetic_op_with_assign(-);
  arithmetic_op_with_assign(/ );
  arithmetic_op_with_assign(%);
  arithmetic_op_with_assign(>> );
  arithmetic_op_with_assign(<< );
  arithmetic_op_with_assign(| );
  arithmetic_op_with_assign(&);
  arithmetic_op_with_assign(^);

  arithmetic_op(&&);
  arithmetic_op(|| );
  arithmetic_op(> );
  arithmetic_op(< );
  arithmetic_op(>= );
  arithmetic_op(<= );
  arithmetic_op(== );
  arithmetic_op(!= );

  unary_prefix_op(-);
  unary_prefix_op(+);
  unary_prefix_op(~);
  unary_prefix_op(!);

  template<vector V>
  [[nodiscard]] constexpr auto dot(V a, V b) noexcept
  {
    return detail::dot_impl(std::make_index_sequence < V{}.size() > (), a.data(), b.data());
  }
  template<vector V>
  [[nodiscard]] constexpr auto norm(V a) noexcept
  {
    return rnu::cx::sqrt(dot(a, a));
  }
  template<vector V>
  [[nodiscard]] constexpr V normalize(V a) noexcept
  {
    return a / norm(a);
  }
  template<typename T>
  [[nodiscard]] constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept
  {
    return vec<T, 3>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
  }

  template<typename T>
  constexpr decltype(auto) operator++(T&& value)
    requires vector<T>&& requires(basic_scalar_t<T> value) { ++ value; }
  { apply([](auto&& v) { ++v; }, std::forward<T>(value)); return value; }

  template<typename T>
  [[nodiscard]] constexpr auto operator++(T&& value, int)
    requires vector<T>&& requires(basic_scalar_t<T> value) { value++; }
  { auto const last = apply([](auto&& v) { return v++; }, std::forward<T>(value)); return last; }

  template<typename T>
  constexpr decltype(auto) operator--(T&& value)
    requires vector<T>&& requires(basic_scalar_t<T> value) { ++value; }
  { apply([](auto&& v) { --v; }, std::forward<T>(value)); return value; }

  template<typename T>
  [[nodiscard]] constexpr auto operator--(T&& value, int)
    requires vector<T>&& requires(basic_scalar_t<T> value) { value--; }
  { auto const last = apply([](auto&& v) { return v--; }, std::forward<T>(value)); return last; }

#undef assign_op
#undef arithmetic_op_with_assign
#undef arithmetic_op
}