#pragma once
#include <array>
#include <concepts>

namespace rnu {
  template<typename T> struct quat_t;
  template<typename T, size_t S> struct vec;
  template<typename T, size_t C, size_t R> class mat;

  template<typename T> struct is_matrix : std::false_type {};
  template<typename T, size_t C, size_t R> struct is_matrix<mat<T, C, R>> : std::true_type {};
  template<typename T> concept matrix = is_matrix<std::decay_t<T>>::value;

  template<typename T> struct is_vector : std::false_type {};
  template<typename T, size_t S> struct is_vector<vec<T, S>> : std::true_type {};
  template<typename T> concept vector = is_vector<std::decay_t<T>>::value;

  template<typename T> struct is_quaternion : std::false_type {};
  template<typename T> struct is_quaternion<quat_t<T>> : std::true_type {};
  template<typename T> concept quaternion = is_quaternion<std::decay_t<T>>::value;

  template<typename T> concept scalar = !vector<T> && !matrix<T> && !quaternion<T>;

  template<typename T, typename V>
  concept uses_vector_arithmetic = (vector<T> && scalar<V>) || (vector<T> && vector<V>) || (scalar<T> && vector<V>);
  template<typename T, typename V>
  concept uses_matrix_arithmetic = (matrix<T> && scalar<V>) || (scalar<T> && matrix<V>);

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
  concept recursively_convertible_to = element_type_convertible_to<T, Dst> || std::convertible_to<T, Dst>;

  template<typename T>
  struct direct_scalar {
    using type = T;
  };

  template<typename T, size_t S>
  struct direct_scalar<vec<T, S>> {
    using type = typename vec<T, S>::value_type;
  };

  template<typename T, size_t C, size_t R>
  struct direct_scalar<mat<T, C, R>> {
    using type = typename mat<T, C, R>::value_type;
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

  template<typename T, size_t C, size_t R>
  struct basic_scalar<mat<T, C, R>> : basic_scalar<typename mat<T, C, R>::value_type> {};

  template<typename T, size_t C, size_t R>
  struct basic_scalar<const mat<T, C, R>> : basic_scalar<typename mat<T, C, R>::value_type const> {};

  template<typename T>
  using basic_scalar_t = typename basic_scalar<std::remove_reference_t<T>>::type;

  template<typename Int>
  [[nodiscard]] constexpr auto to_float(Int integer)
    requires (std::convertible_to<Int, float> || std::convertible_to<Int, double>)
  {
    if constexpr (sizeof(Int) <= sizeof(float))
      return static_cast<float>(integer);
    else
      return static_cast<double>(integer);
  }

  template<typename Int>
  using to_float_t = decltype(to_float(std::declval<Int>()));
}