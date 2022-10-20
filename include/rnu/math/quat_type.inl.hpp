#include "cx_fun.hpp"
namespace rnu
{
  template<typename T>
  template<typename U>
  [[nodiscard]] constexpr quat_t<T>::quat_t(quat_t<U> const& other) noexcept : components{ T(other[0]), T(other[1]), T(other[2]), T(other[3]) } {}

  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(T w, T x, T y, T z) noexcept : components{ w, x, y, z } {}

  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(angle_axis_type axis, T angle)
    : quat_t(angle_axis_init_t{}, cx::cos(T(angle / T(2))), cx::sin(T(angle / T(2))) * static_cast<vec<T, 3>>(axis)) {}

  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(angle_axis_init_t, T w, vec<T, 3> xyz) : components{ w, xyz[0], xyz[1], xyz[2] } {}

  template<typename T>
  [[nodiscard]] constexpr mat<T, 3, 3> quat_t<T>::matrix() const noexcept {
    const auto x2 = at(1) * at(1);
    const auto y2 = at(2) * at(2);
    const auto z2 = at(3) * at(3);

    const auto xy = at(1) * at(2);
    const auto xz = at(1) * at(3);
    const auto yz = at(2) * at(3);
    const auto wx = at(0) * at(1);
    const auto wy = at(0) * at(2);
    const auto wz = at(0) * at(3);
    return transpose(mat<T, 3, 3>{
      vec<T, 3>{1 - 2 * y2 - 2 * z2, 2 * xy - 2 * wz, 2 * xz + 2 * wy},
      vec<T, 3>{2 * xy + 2 * wz, 1 - 2 * x2 - 2 * z2, 2 * yz - 2 * wx},
      vec<T, 3>{2 * xz - 2 * wy, 2 * yz + 2 * wx, 1 - 2 * x2 - 2 * y2}
    });
  }

  template<typename T>
  template<typename V>
  [[nodiscard]] constexpr quat_t<T>::operator quat_t<V>() noexcept {
    return quat_t<V>(
      static_cast<V>(at(0)),
      static_cast<V>(at(1)),
      static_cast<V>(at(2)),
      static_cast<V>(at(3)));
  }

  template<quaternion Rhs>
  [[nodiscard]] constexpr auto operator-(Rhs const& rhs) noexcept {
    using rhs_t = typename Rhs::value_type;
    using result_t = decltype(-rhs_t{});

    return quat_t<result_t>(
      -rhs[0],
      -rhs[1],
      -rhs[2],
      -rhs[3]);
  }
  template<quaternion Lhs, quaternion Rhs>
  [[nodiscard]] constexpr auto operator-(Lhs const& lhs, Rhs const& rhs) noexcept {
    using lhs_t = typename Lhs::value_type;
    using rhs_t = typename Rhs::value_type;
    using result_t = decltype(lhs_t{} - rhs_t{});

    return quat_t<result_t>(
      lhs[0] - rhs[0],
      lhs[1] - rhs[1],
      lhs[2] - rhs[2],
      lhs[3] - rhs[3]);
  }
  template<quaternion Lhs, quaternion Rhs>
  [[nodiscard]] constexpr auto operator+(Lhs const& lhs, Rhs const& rhs) noexcept {
    using lhs_t = typename Lhs::value_type;
    using rhs_t = typename Rhs::value_type;
    using result_t = decltype(lhs_t{} + rhs_t{});

    return quat_t<result_t>(
      lhs[0] + rhs[0],
      lhs[1] + rhs[1],
      lhs[2] + rhs[2],
      lhs[3] + rhs[3]);
  }

  template<typename T>
  [[nodiscard]] constexpr quat_t<T> quat_t<T>::operator*(const quat_t& other) const noexcept
  {
    const auto w0 = at(0);
    const auto w1 = other[0];

    const vec<T, 3> v0(at(1), at(2), at(3));
    const vec<T, 3> v1(other[1], other[2], other[3]);

    const auto xyz = w0 * v1 + w1 * v0 + cross(v0, v1);
    const auto  w = w0 * w1 - dot(v0, v1);
    return quat_t(w, xyz[0], xyz[1], xyz[2]);
  }
  template<typename T>
  constexpr quat_t<T>& quat_t<T>::operator*=(const quat_t& other) noexcept
  {
    *this = *this * other;
    return *this;
  }
  template<typename T>
  [[nodiscard]] constexpr vec<T, 3> quat_t<T>::operator*(const vec<T, 3>& v) const noexcept
  {
    const auto t = T(2) * cross(vec<T, 3>(at(1), at(2), at(3)), v);
    return v + w * t + cross(vec<T, 3>(at(1), at(2), at(3)), t);
  }
}