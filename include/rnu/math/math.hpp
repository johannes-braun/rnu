#pragma once

#include "mat.hpp"
#include "vec.hpp"
#include "quat.hpp"
#include "rect.hpp"
#include "traits.hpp"
#include "cx_fun.hpp"

namespace rnu {
#define implement_cx_fun1(name, param1)\
  template<typename T> constexpr auto name(T&& param1) requires requires(direct_scalar_t<T> t) { cx::name(t); } { return apply_2d(&cx::name<direct_scalar_t<T>>, param1); }
#define implement_cx_fun2(name, param1, param2)\
  template<typename T1, typename T2> constexpr auto name(T1&& param1, T2&& param2) requires requires(direct_scalar_t<T1> t1, direct_scalar_t<T2> t2) { cx::name(t1, t2); } { return apply_2d([](auto&&... args){ return cx::name(args...); }, param1, param2); }
#define implement_cx_fun3(name, param1, param2, param3)\
  template<typename T1, typename T2, typename T3> constexpr auto name(T1&& param1, T2&& param2, T3&& param3) requires requires(direct_scalar_t<T1> t1, direct_scalar_t<T2> t2, direct_scalar_t<T3> t3) { cx::name(t1, t2, t3); } { return apply_2d([](auto&&... args){ return cx::name(args...); }, param1, param2, param3); }

  implement_cx_fun1(abs, value);
  implement_cx_fun1(round, value);
  implement_cx_fun1(sign, value);
  implement_cx_fun1(sqrt, value);
  implement_cx_fun1(sin,  rad);
  implement_cx_fun1(cos,  rad);
  implement_cx_fun1(tan,  rad);
  implement_cx_fun1(asin, rad);
  implement_cx_fun1(acos, rad);
  implement_cx_fun1(atan, rad);
  implement_cx_fun1(radians, deg);
  implement_cx_fun1(degrees, rad);

  implement_cx_fun2(pow, base, exponent);
  implement_cx_fun2(modf, x, iptr);
  implement_cx_fun2(fmod, value, divisor);
  implement_cx_fun2(step, value, threshold);
  implement_cx_fun2(min, a, b);
  implement_cx_fun2(max, a, b);
  implement_cx_fun2(atan2, y, x);

  implement_cx_fun3(clamp, value, low, high);
  implement_cx_fun3(smoothstep, low, high, value);


  template<typename T> using mat2_t = mat<T, 2, 2>;
  template<typename T> using mat3_t = mat<T, 3, 3>;
  template<typename T> using mat4_t = mat<T, 4, 4>;

  template<typename T>
  using vec2_t = vec<T, 2>;
  template<typename T>
  using vec3_t = vec<T, 3>;
  template<typename T>
  using vec4_t = vec<T, 4>;

  using vec2 = vec2_t<float>;
  using vec3 = vec3_t<float>;
  using vec4 = vec4_t<float>;
  using vec2d = vec2_t<double>;
  using vec3d = vec3_t<double>;
  using vec4d = vec4_t<double>;
  using vec2i = vec2_t<int>;
  using vec3i = vec3_t<int>;
  using vec4i = vec4_t<int>;
  using vec2ui = vec2_t<unsigned>;
  using vec3ui = vec3_t<unsigned>;
  using vec4ui = vec4_t<unsigned>;
  using vec2b = vec2_t<bool>;
  using vec3b = vec3_t<bool>;
  using vec4b = vec4_t<bool>;
  using vec2i8 = vec2_t<int8_t>;
  using vec3i8 = vec3_t<int8_t>;
  using vec4i8 = vec4_t<int8_t>;
  using vec2ui8 = vec2_t<uint8_t>;
  using vec3ui8 = vec3_t<uint8_t>;
  using vec4ui8 = vec4_t<uint8_t>;
  using vec2i16 = vec2_t<int16_t>;
  using vec3i16 = vec3_t<int16_t>;
  using vec4i16 = vec4_t<int16_t>;
  using vec2ui16 = vec2_t<uint16_t>;
  using vec3ui16 = vec3_t<uint16_t>;
  using vec4ui16 = vec4_t<uint16_t>;
  using vec2i32 = vec2_t<int32_t>;
  using vec3i32 = vec3_t<int32_t>;
  using vec4i32 = vec4_t<int32_t>;
  using vec2ui32 = vec2_t<uint32_t>;
  using vec3ui32 = vec3_t<uint32_t>;
  using vec4ui32 = vec4_t<uint32_t>;
  using vec2i64 = vec2_t<int64_t>;
  using vec3i64 = vec3_t<int64_t>;
  using vec4i64 = vec4_t<int64_t>;
  using vec2ui64 = vec2_t<uint64_t>;
  using vec3ui64 = vec3_t<uint64_t>;
  using vec4ui64 = vec4_t<uint64_t>;

  using mat2 = mat2_t<float>;
  using mat3 = mat3_t<float>;
  using mat4 = mat4_t<float>;
  using mat2d = mat2_t<double>;
  using mat3d = mat3_t<double>;
  using mat4d = mat4_t<double>;
  using mat2i = mat2_t<int>;
  using mat3i = mat3_t<int>;
  using mat4i = mat4_t<int>;
  using mat2ui = mat2_t<unsigned>;
  using mat3ui = mat3_t<unsigned>;
  using mat4ui = mat4_t<unsigned>;
  using mat2i8 = mat2_t<std::int8_t>;
  using mat3i8 = mat3_t<std::int8_t>;
  using mat4i8 = mat4_t<std::int8_t>;
  using mat2ui8 = mat2_t<std::uint8_t>;
  using mat3ui8 = mat3_t<std::uint8_t>;
  using mat4ui8 = mat4_t<std::uint8_t>;
  using mat2i16 = mat2_t<std::int16_t>;
  using mat3i16 = mat3_t<std::int16_t>;
  using mat4i16 = mat4_t<std::int16_t>;
  using mat2ui16 = mat2_t<std::uint16_t>;
  using mat3ui16 = mat3_t<std::uint16_t>;
  using mat4ui16 = mat4_t<std::uint16_t>;
  using mat2i32 = mat2_t<std::int32_t>;
  using mat3i32 = mat3_t<std::int32_t>;
  using mat4i32 = mat4_t<std::int32_t>;
  using mat2ui32 = mat2_t<std::uint32_t>;
  using mat3ui32 = mat3_t<std::uint32_t>;
  using mat4ui32 = mat4_t<std::uint32_t>;
  using mat2i64 = mat2_t<std::int64_t>;
  using mat3i64 = mat3_t<std::int64_t>;
  using mat4i64 = mat4_t<std::int64_t>;
  using mat2ui64 = mat2_t<std::uint64_t>;
  using mat3ui64 = mat3_t<std::uint64_t>;
  using mat4ui64 = mat4_t<std::uint64_t>;

  using rect2f = box<2, float>;
  using rect2d = box<2, double>;
  using box3f = box<3, float>;
  using box3d = box<3, double>;

  template<typename T>
  constexpr mat4_t<T> translation(vec3_t<T> vector) noexcept {
    return mat4_t<T>{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, vector[0], vector[1], vector[2], 1 };
  }
  template<quaternion T>
  constexpr mat4_t<typename T::value_type> rotation(T quat) noexcept {
    return mat4_t<typename T::value_type>(quat.matrix());
  }
  template<typename T>
  constexpr mat4_t<T> scale(vec3_t<T> scales) noexcept {
    mat4_t<T> result;
    result.at(0, 0) = scales[0];
    result.at(1, 1) = scales[1];
    result.at(2, 2) = scales[2];
    return result;
  }

  template <matrix Lhs, matrix Rhs>
  [[nodiscard]] constexpr auto operator*(Lhs const& lhs, Rhs&& rhs) requires requires(Lhs m, Rhs v) {
    v[0][0] * m[0][0] + v[0][0] * m[0][0];
  } &&(std::decay_t<Lhs>::rows == std::decay_t<Rhs>::columns && std::decay_t<Lhs>::columns == std::decay_t<Rhs>::rows) {
    constexpr auto rows = std::decay_t<Lhs>::rows;
    constexpr auto columns = std::decay_t<Rhs>::columns;
    return detail::traverse<rows, columns>([&](size_t c, size_t r) { return dot(lhs.row(r), rhs.col(c)); });
  }

  template<matrix Lhs, matrix Rhs>
  [[nodiscard]] constexpr auto operator*(Lhs&& lhs, Rhs&& rhs)
    requires requires(Lhs m, Rhs v) { v[0][0] * m[0][0] + v[0][0] * m[0][0]; } &&
      (std::decay_t<Lhs>::rows == std::decay_t<Rhs>::columns && std::decay_t<Lhs>::columns == std::decay_t<Rhs>::rows)
  {
    constexpr auto rows = std::decay_t<Lhs>::rows;
    constexpr auto columns = std::decay_t<Rhs>::columns;
    return detail::traverse<rows, columns>([&](size_t c, size_t r) {
      return dot(lhs.row(r).get(), rhs.col(c));
      });
  }
  template<matrix Lhs, matrix Rhs>
  constexpr decltype(auto) operator*=(Lhs& lhs, Rhs&& rhs)
    requires requires(Lhs m, Rhs v) { v[0][0] * m[0][0] + v[0][0] * m[0][0]; } &&
    (std::decay_t<Lhs>::rows == std::decay_t<Rhs>::columns && std::decay_t<Lhs>::is_square)
  {
    constexpr auto rows = std::decay_t<Lhs>::rows;
    constexpr auto columns = std::decay_t<Rhs>::columns;
    auto const tmp = detail::traverse<rows, columns>([&](size_t c, size_t r) {
      return dot(lhs.row(r).get(), rhs.col(c));
      });
    lhs = std::move(tmp);
    return lhs;
  }
}