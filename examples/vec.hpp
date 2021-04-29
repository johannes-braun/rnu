#pragma once
#include <array>
#include <concepts>

#include <algorithm>

namespace rnu2
{

  template<typename T, size_t S>
  struct vec;

  template<typename T> struct is_vector : std::false_type {};
  template<typename T, size_t S> struct is_vector<vec<T, S>> : std::true_type {};
  template<typename T> concept vector = is_vector<std::decay_t<T>>::value;

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

    template<typename T>
    concept const_iterable = requires(const std::decay_t<T> t) {
      t.begin();
      t.end();
    } || requires(const std::decay_t<T> t) {
      t.cbegin();
      t.cend();
    };

    template<typename Src, typename Dst>
    concept element_type_convertible_to = const_iterable<Src> && std::convertible_to<std::decay_t<decltype(*std::declval<Src>().begin())>, Dst>;

    template<typename T, typename Dst>
    concept recursively_convertible_to = detail::element_type_convertible_to<T, Dst> || std::convertible_to<T, Dst>;

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

    template<typename T>
    struct direct_scalar {
      using type = T;
    };

    template<typename T, size_t S>
    struct direct_scalar<vec<T, S>> {
      using type = typename vec<T, S>::value_type;
    };

    template<typename T>
    using direct_scalar_t = typename direct_scalar<std::remove_reference_t<T>>::type;

    template<typename T>
    struct basic_scalar {
      using type = T;
    };

    template<typename T, size_t S>
    struct basic_scalar<vec<T, S>> : basic_scalar<typename vec<T, S>::value_type> {};

    template<typename T, size_t S>
    struct basic_scalar<const vec<T, S>> : basic_scalar<typename vec<T, S>::value_type const> {};

    template<typename T>
    using basic_scalar_t = typename basic_scalar<std::remove_reference_t<T>>::type;

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
  decltype(auto) get_all(Fun&& fun, size_t index, T&&... ts)
  {
    return fun(rnu2::detail::get(std::forward<T>(ts), index)...);
  }

  template<typename Fun, size_t... Indices, typename... T>
  auto apply_impl(Fun&& fun, std::index_sequence<Indices...>, T&&... ts)
  {
    using rv = decltype(get_all(std::forward<Fun>(fun), 0, std::forward<T>(ts)...));
    constexpr auto count = rnu2::max_size<T...>();

    if constexpr (std::same_as<rv, void>)
      (get_all(std::forward<Fun>(fun), Indices, std::forward<T>(ts)...), ...);
    else if constexpr (std::is_reference_v<rv>)
      return rnu2::vec<std::reference_wrapper<std::decay_t<rv>>, count>(get_all(std::forward<Fun>(fun), Indices, std::forward<T>(ts)...)...);
    else
      return rnu2::vec<std::decay_t<rv>, count>(get_all(std::forward<Fun>(fun), Indices, std::forward<T>(ts)...)...);
  }

  template<typename Fun, typename... T>
  concept applicable = requires(Fun && fun, T&&... ts) {
    apply(std::forward<Fun>(fun), std::forward<T>(ts)...);
  };

  template<typename... T>
  concept equivalent_types = (vector<T> || ...) && (equivalent<T...>());

  template<typename Fun, typename... T>
  requires equivalent_types<T...> && requires(Fun&& fun, T&&... ts) { get_all(std::forward<Fun>(fun), 0, std::forward<T>(ts)...); }
  auto apply(Fun&& fun, T&&... ts) {
    return apply_impl(std::forward<Fun>(fun), std::make_index_sequence<sizeof...(T)>(), std::forward<T>(ts)...);
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
      requires (std::convertible_to<Ts, T> && ...)
      : vec_base<T, S>(static_cast<T>(std::forward<Ts>(components))...)
    {
    }

    constexpr vec(T value)
      : vec_base<T, S>{ std::make_index_sequence<S>(), value }
    {
    }

    template<vector V>
    constexpr vec(V other) requires (V::count > count)
    {
      for (size_t i = 0; i < count; ++i)
        at(i) = static_cast<T>(other[i]);
    }

    template<typename... Ts
#if __INTELLISENSE__
      , std::enable_if_t<
#else
    > requires
#endif
      ((std::less<size_t>{}(sizeof...(Ts), S)) && ((detail::count_v<Ts> +...) == S) && (detail::recursively_convertible_to<Ts, T> && ...) && (std::less<size_t>{}(1, detail::count_v<Ts>) || ...))
#if __INTELLISENSE__
      , int > * = nullptr
    >
#endif
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
      for (size_t i = 0; i < size(); ++i)
        if (!at(i)) return false;
      return true;
    }
  };

#define assign_op(Ope) \
    template<typename Lhs, typename Rhs> \
    constexpr decltype(auto) operator Ope(Lhs& lhs, Rhs&& rhs) \
      requires equivalent_types<Lhs, Rhs>&& requires(detail::basic_scalar_t<Lhs>& lhs, detail::basic_scalar_t<Rhs> rhs) { lhs Ope rhs; } \
    { apply([](auto&& l, auto&& r) { l Ope r; }, std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)); return lhs; }

#define arithmetic_op(Op) \
  template<typename Lhs, typename Rhs> \
    [[nodiscard]] constexpr auto operator Op(Lhs&& lhs, Rhs&& rhs) \
      requires equivalent_types<Lhs, Rhs>&& requires(detail::basic_scalar_t<Lhs> lhs, detail::basic_scalar_t<Rhs> rhs) { lhs Op rhs; } \
    { return apply([](auto&& l, auto&& r) { return l Op r; }, std::forward<Lhs>(lhs), std::forward<Rhs>(rhs)); }

#define unary_prefix_op(Op) \
  template<typename T> \
    [[nodiscard]] constexpr auto operator Op(T&& value) \
      requires vector<T> && requires(detail::basic_scalar_t<T> value) { Op value; } \
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


  template<typename T>
  [[nodiscard]] constexpr decltype(auto) operator++(T&& value)
    requires vector<T>&& requires(detail::basic_scalar_t<T> value) { ++ value; }
  { apply([](auto&& v) { ++v; }, std::forward<T>(value)); return value; }

  template<typename T>
  [[nodiscard]] constexpr auto operator++(T&& value, int)
    requires vector<T>&& requires(detail::basic_scalar_t<T> value) { value++; }
  { auto const last = apply([](auto&& v) { return v++; }, std::forward<T>(value)); return last; }

  template<typename T>
  [[nodiscard]] constexpr decltype(auto) operator--(T&& value)
    requires vector<T>&& requires(detail::basic_scalar_t<T> value) { ++value; }
  { apply([](auto&& v) { --v; }, std::forward<T>(value)); return value; }

  template<typename T>
  [[nodiscard]] constexpr auto operator--(T&& value, int)
    requires vector<T>&& requires(detail::basic_scalar_t<T> value) { value--; }
  { auto const last = apply([](auto&& v) { return v--; }, std::forward<T>(value)); return last; }

#undef assign_op
#undef arithmetic_op_with_assign
#undef arithmetic_op
}