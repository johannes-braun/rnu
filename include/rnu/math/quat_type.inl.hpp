namespace rnu
{
  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(quat_t const& other) noexcept : components{ other.w, other.x, other.y, other.z } {}

  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(T w, T x, T y, T z) noexcept : components{ w, x, y, z } {}

  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(angle_axis_type axis, T angle)
    : quat_t(angle_axis_init_t{}, rnu::cos(T(angle / T(2))), rnu::sin(T(angle / T(2))) * static_cast<vec<T, 3>>(axis)) {}

  template<typename T>
  [[nodiscard]] constexpr quat_t<T>::quat_t(angle_axis_init_t, T w, vec<T, 3> xyz) : components{ w, xyz.x, xyz.y, xyz.z } {}

  template<typename T>
  [[nodiscard]] constexpr mat<T, 3, 3> quat_t<T>::matrix() const noexcept {
    const auto x2 = this->x * this->x;
    const auto y2 = this->y * this->y;
    const auto z2 = this->z * this->z;

    const auto xy = this->x * this->y;
    const auto xz = this->x * this->z;
    const auto yz = this->y * this->z;
    const auto wx = this->w * this->x;
    const auto wy = this->w * this->y;
    const auto wz = this->w * this->z;
    return mat<T, 3, 3>{vec<T, 3>{1 - 2 * y2 - 2 * z2, 2 * xy - 2 * wz, 2 * xz + 2 * wy},
      vec<T, 3>{2 * xy + 2 * wz, 1 - 2 * x2 - 2 * z2, 2 * xz - 2 * wx},
      vec<T, 3>{2 * xz - 2 * wy, 2 * yz + 2 * wx, 1 - 2 * x2 - 2 * y2}};
  }

  template<typename T>
  template<typename V>
  [[nodiscard]] constexpr quat_t<T>::operator quat_t<V>() noexcept {
    return quat_t<V>(
      static_cast<V>(w),
      static_cast<V>(x),
      static_cast<V>(y),
      static_cast<V>(z));
  }

  template<typename T>
  [[nodiscard]] constexpr quat_t<T> quat_t<T>::operator*(const quat_t& other) const noexcept
  {
    const auto w0 = this->w;
    const auto w1 = other.w;

    const vec<T, 3> v0(x, y, z);
    const vec<T, 3> v1(other.x, other.y, other.z);

    const auto xyz = w0 * v1 + w1 * v0 + cross(v0, v1);
    const auto  w = w0 * w1 - dot(v0, v1);
    return quat_t(w, xyz.x, xyz.y, xyz.z);
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
    const auto t = T(2) * cross(vec<T, 3>(x, y, z), v);
    return v + w * t + cross(vec<T, 3>(x, y, z), t);
  }
}