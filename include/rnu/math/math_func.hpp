#pragma once
#include "vec_type.hpp"
#include "operators.hpp"
#include <cmath>
#include <algorithm>
#include <numbers>
#include <concepts>
#include <complex>
#include <numeric>
#include <bit>
#include "cx_fun.hpp"

namespace rnu
{
  namespace detail2
  {
    template<typename T> requires requires(T t, size_t i) { {t.col(i)}; }
    constexpr decltype(auto) get(T& t, size_t index)
    {
      return t.col(index);
    }

    template<typename T> requires requires(T t, size_t i) { {t[i]}; }
    constexpr decltype(auto) get(T& t, size_t index)
    {
      return t[index];
    }
    template<typename T>
    constexpr decltype(auto) get(T& t, size_t index)
    {
      return t;
    }

    template<typename T>
    constexpr auto get(T&& t, size_t index)
    {
      return get(static_cast<T&>(t), index);
    }

    template<typename T> requires requires { { std::decay_t<T>::cols }; }
    constexpr size_t size()
    {
      return std::decay_t<T>::cols;
    }
    template<typename T> requires requires { { std::decay_t<T>::component_count }; }
    constexpr size_t size()
    {
      return std::decay_t<T>::component_count;
    }
    template<typename T>
    constexpr size_t size()
    {
      return 1;
    }

    template<typename T>
    concept has_value_type = requires { { typename std::decay_t<T>::value_type{} }; };

    template<typename T> struct value_type_impl { using type = std::decay_t<T>; };

    template<typename T> requires vector_type<std::decay_t<T>> struct value_type_impl<T> {
      using type = typename std::decay_t<T>::value_type;
    };
    template<typename T> requires matrix_type<std::decay_t<T>> struct value_type_impl<T> {
      using type = typename std::decay_t<T>::value_type;
    };

    template<typename T>
    using value_type = typename value_type_impl<T>::type;

    template<typename A, typename B>
    concept compatible = size<A>() == size<B>() || size<A>() == 1 || size<B>() == 1;

    template<typename T, typename ... Ts>
    concept all_compatible = true && (compatible<T, Ts> && ...);

    template<typename T, typename... Ts> requires all_compatible<T, Ts...> consteval size_t common_size() {
      size_t maximum = size<T>();
      ((maximum = size<Ts>() > maximum ? size<Ts>() : maximum), ...);
      return maximum;
    }

    template<typename Callable, typename... Ts>
    concept suitable_operands = requires(Callable callable, value_type<Ts>... ts) {
      { common_size<Ts...>() };
      { callable(ts...) };
    };

    template<typename Callable, typename... Ts>
    constexpr auto apply(Callable callable, Ts&&... ts) requires suitable_operands<Callable, Ts...> {
      constexpr auto s = common_size<Ts...>();

      if constexpr (!(vector_type<std::decay_t<decltype(detail2::get(std::declval<Ts>(), 0))>> || ...))
      {
        using ty = std::decay_t<decltype(callable(detail2::get(ts, 0)...))>;
        if constexpr (s == 1)
          return callable(detail2::get(ts, 0)...);
        else if constexpr (std::same_as<ty, void>)
        {
          for (size_t i = 0; i < s; ++i)
            callable(detail2::get(ts, i)...);
        }
        else
        {
          vec<ty, s> r{};
          for (size_t i = 0; i < s; ++i) {
            r[i] = callable(detail2::get(ts, i)...);
          }
          return r;
        }
      }
      else
      {
        using ty = std::decay_t<decltype(detail2::apply<Callable>(callable, detail2::get(ts, 0)...))>;

        if constexpr (std::same_as<ty, void>)
        {
          for (size_t i = 0; i < s; ++i)
            detail2::apply<Callable>(callable, detail2::get(ts, i)...);
        }
        else
        {
          vec<ty, s> r{};
          for (size_t i = 0; i < s; ++i) {
            r[i] = detail2::apply<Callable>(callable, detail2::get(ts, i)...);
          }
          return r;
        }
      }
    }
  }
  template<typename Callable, typename... Ts>
  constexpr auto apply(Callable callable, Ts&&... ts) requires detail2::suitable_operands<Callable, Ts...> {
    return detail2::apply(callable, std::forward<Ts>(ts)...);
  }

#define make_fun1(name, parent, a)\
  template<typename Q>\
  constexpr [[nodiscard]] decltype(auto) name(Q&& a) requires requires(detail2::value_type<Q> a) { { parent(a) }; }\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a); }

#define make_fun1_checked(name, parent, a)\
  template<typename Q>\
  constexpr [[nodiscard]] decltype(auto) name(Q&& a) requires requires(detail2::value_type<Q> a) { { parent(a) }; } && detail2::any_vector_type<Q>\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a); }

#define make_fun2(name, parent, a, b)\
  template<typename Q, typename R>\
  constexpr [[nodiscard]] decltype(auto) name(Q&& a, R&& b) requires requires(detail2::value_type<Q> a, detail2::value_type<R> b) { { parent(a, b) }; }\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a, b); }\
  template<typename Q, typename R>\
  constexpr [[nodiscard]] decltype(auto) name(Q const& a, R const& b) requires requires(detail2::value_type<Q> const a, detail2::value_type<R> const b) { { parent(a, b) }; }\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a, b); }

#define make_fun2_checked(name, parent, a, b)\
  template<typename Q, typename R>\
  constexpr [[nodiscard]] decltype(auto) name(Q&& a, R&& b) requires requires(detail2::value_type<Q> a, detail2::value_type<R> b) { { parent(a, b) }; } && detail2::any_vector_type<Q, R>\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a, b); } \
  template<typename Q, typename R>\
  constexpr [[nodiscard]] decltype(auto) name(Q const& a, R const& b) requires requires(detail2::value_type<Q> const a, detail2::value_type<R> const b) { { parent(a, b) }; } && detail2::any_vector_type<Q, R>\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a, b); }

#define make_fun3(name, parent, a, b, c)\
  template<typename Q, typename R, typename S>\
  constexpr [[nodiscard]] decltype(auto) name(Q&& a, R&& b, S&& c) requires requires(detail2::value_type<Q> a, detail2::value_type<R> b, detail2::value_type<S> c) { { parent(a, b, c) }; }\
  { return detail2::apply([](auto&&... v) { return parent(v...); }, a, b, c); }

#define expand_one_operator1(op, name)\
  make_fun1_checked(operator op, rnu::call_##name, value);

#define expand_one_operator2(op, name)\
  make_fun2_checked(operator op, rnu::call_##name, lhs, rhs);

#define make_assign(op)\
  template<typename Q, typename R>\
  constexpr [[nodiscard]] decltype(auto) operator op=(Q& a, R&& b) requires requires(detail2::value_type<Q> a, detail2::value_type<R> b) { { a op= b }; } && detail2::any_vector_type<Q>\
  { a = detail2::apply([](auto&& v, auto&& w) { return v op= w; }, a, b); return a; }

#define expand_two_operators2(op, name) \
  make_fun2_checked(operator op, rnu::call_##name, lhs, rhs); \
  make_assign(op);

  make_fun1(isinf, std::isinf, value);
  make_fun1(isnan, std::isnan, value);
  make_fun1(abs, rnu::cx::abs, value);
  make_fun1(sqrt, rnu::cx::sqrt, value);
  make_fun1(sign, rnu::cx::sign, value);
  make_fun1(radians, rnu::cx::radians, value);
  make_fun1(degrees, rnu::cx::degrees, value);

  make_fun1(sin, rnu::cx::sin, rad);
  make_fun1(cos, rnu::cx::cos, rad);
  make_fun1(tan, rnu::cx::tan, rad);
  make_fun1(asin, rnu::cx::asin, value);
  make_fun1(acos, rnu::cx::acos, value);
  make_fun1(atan, rnu::cx::atan, value);
  make_fun2(atan2, rnu::cx::atan2, y, x);

  make_fun2(pow, rnu::cx::pow, base, exponent);
  make_fun2(min, rnu::cx::min, a, b);
  make_fun2(max, rnu::cx::max, a, b);
  make_fun2(step, rnu::cx::step, value, threshold);
  make_fun3(clamp, rnu::cx::clamp, value, lower, upper);
  make_fun3(smoothstep, rnu::cx::smoothstep, edge0, edge1, value);

  expand_two_operators2(+, plus);
  expand_two_operators2(-, minus);
  expand_two_operators2(*, multiplies);
  expand_two_operators2(/ , divides);
  expand_two_operators2(%, modulus);
  expand_two_operators2(<< , bit_shl);
  expand_two_operators2(>> , bit_shr);
  expand_two_operators2(&, bit_and);
  expand_two_operators2(| , bit_or);
  expand_two_operators2(^, bit_xor);
  expand_one_operator2(&&, logical_and);
  expand_one_operator2(|| , logical_or);

  expand_one_operator2(== , equal_to);
  expand_one_operator2(!= , not_equal_to);
  expand_one_operator2(> , greater);
  expand_one_operator2(< , less);
  expand_one_operator2(>= , greater_equal);
  expand_one_operator2(<= , less_equal);

  expand_one_operator1(!, logical_not);
  expand_one_operator1(~, bit_not);
  expand_one_operator1(-, negate);

  template<vector_type V> requires requires(detail2::value_type<V>& v) { { v += 1}; }
  constexpr V& operator++(V& v) noexcept { return v += 1; }

  template<vector_type V> requires requires(detail2::value_type<V>& v) { {v += 1}; }
  [[nodiscard]] constexpr V operator++(V& v, int) noexcept { V result = v; v += 1; return result; }

  template<vector_type V> requires requires(detail2::value_type<V>& v) { {v -= 1}; }
  constexpr V& operator--(V& v) noexcept { return v -= 1; }

  template<vector_type V> requires requires(detail2::value_type<V>& v) { {v -= 1}; }
  [[nodiscard]] constexpr V operator--(V& v, int) noexcept { V result = v; v -= 1; return result; }



#undef expand_one_operator1
#undef expand_one_operator2
#undef expand_two_operators2
#undef make_fun1
#undef make_fun2
#undef make_fun3
}
#include "vec_apply.hpp"

#include "math_func.inl.hpp"
#include "vec_math.inl.hpp"