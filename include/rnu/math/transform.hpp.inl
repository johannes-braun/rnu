namespace rnu
{
  template<std::floating_point T>
  constexpr transform<T>::transform(const vector_t& position, const vector_t& scale, const rotation_t& rotation) noexcept
    : position(position), scale(scale), rotation(rotation) {}

  template<std::floating_point T>
  constexpr bool transform<T>::operator==(const transform<T>& other) const noexcept
  {
    return equals(other, equal_epsilon);
  }

  template<std::floating_point T>
  constexpr bool transform<T>::equals(const transform<T>& other, T epsilon) const noexcept
  {
    const auto eps_eq = [&](auto x, auto y)
    {
      return rnu::cx::abs(x - y) < epsilon;
    };
    return
      eps_eq(position.x, other.position.x) &&
      eps_eq(position.y, other.position.y) &&
      eps_eq(position.z, other.position.z) &&
      eps_eq(scale.x, other.scale.x) &&
      eps_eq(scale.y, other.scale.y) &&
      eps_eq(scale.z, other.scale.z) &&
      eps_eq(rotation.w, other.rotation.w) &&
      eps_eq(rotation.x, other.rotation.x) &&
      eps_eq(rotation.y, other.rotation.y) &&
      eps_eq(rotation.z, other.rotation.z);
  }

  template<std::floating_point T>
  constexpr bool transform<T>::operator!=(const transform<T>& other) const noexcept { return !operator==(other); }

  template<std::floating_point T>
  constexpr transform<T> transform<T>::operator*(const transform<T>& other) const noexcept
  {
    transform result = *this;
    result *= other;
    return result;
  }

  template<std::floating_point T>
  constexpr transform<T>& transform<T>::operator*=(const transform<T>& other) noexcept
  {
    position.x += other.position.x;
    position.y += other.position.y;
    position.z += other.position.z;

    scale.x *= other.scale.x;
    scale.y *= other.scale.y;
    scale.z *= other.scale.z;

    rotation *= other.rotation;
    return *this;
  }

  template<std::floating_point T>
  constexpr transform<T>::transform(matrix_t const& mat) noexcept
  {
    const auto mat3w = mat[3][3] == 0 ? T(1.0) : mat[3][3];
    vector_t ax(mat[0]);
    vector_t ay(mat[1]);
    vector_t az(mat[2]);
    position = mat[3];
    position[0] /= mat3w;
    position[1] /= mat3w;
    position[2] /= mat3w;

    ax = ax / vector_t(scale[0] = norm(ax));
    ay = ay / vector_t(scale[1] = norm(ay));
    az = az / vector_t(scale[2] = norm(az));

    const auto m00 = ax[0];
    const auto m11 = ay[1];
    const auto m22 = az[2];
    const auto m01 = ax[1];
    const auto m10 = ay[0];
    const auto m02 = ax[2];
    const auto m20 = az[0];
    const auto m12 = ay[2];
    const auto m21 = az[1];
    float t = 0;
    if (m22 < 0)
    {
      if (m00 > m11)
      {
        t = 1 + m00 - m11 - m22;
        rotation = rotation_t(m12 - m21, t, m01 + m10, m20 + m02);
      }
      else
      {
        t = 1 - m00 + m11 - m22;
        rotation = rotation_t(m20 - m02, m01 + m10, t, m12 + m21);
      }
    }
    else
    {
      if (m00 < -m11)
      {
        t = 1 - m00 - m11 + m22;
        rotation = rotation_t(m01 - m10, m20 + m02, m12 + m21, t);
      }
      else
      {
        t = 1 + m00 + m11 + m22;
        rotation = rotation_t(t, m12 - m21, m20 - m02, m01 - m10);
      }
    }
    const auto half_t_sqrt = T(0.5) / rnu::cx::sqrt(t);
    rotation[3] *= half_t_sqrt;
    rotation[0] *= half_t_sqrt;
    rotation[1] *= half_t_sqrt;
    rotation[2] *= half_t_sqrt;
  }

  template<std::floating_point T>
  constexpr transform<T>::operator matrix_t() const noexcept
  {
    auto const x = left() * scale[0];
    auto const y = up() * scale[1];
    auto const z = backward() * scale[2];
    return matrix_t(homogeneous_t(x, 0), homogeneous_t(y, 0), homogeneous_t(z, 0), homogeneous_t(position, 1));
  }

  template<std::floating_point T>
  constexpr transform<T>::matrix_t transform<T>::matrix() const noexcept
  {
    return static_cast<matrix_t>(*this);
  }

  template<std::floating_point T>
  constexpr transform<T>::vector_t transform<T>::up() const noexcept { return rotation * vector_t(0, 1, 0); }
  template<std::floating_point T>
  constexpr transform<T>::vector_t transform<T>::down() const noexcept { return rotation * vector_t(0, -1, 0); }
  template<std::floating_point T>
  constexpr transform<T>::vector_t transform<T>::left() const noexcept { return rotation * vector_t(1, 0, 0); }
  template<std::floating_point T>
  constexpr transform<T>::vector_t transform<T>::right() const noexcept { return rotation * vector_t(-1, 0, 0); }
  template<std::floating_point T>
  constexpr transform<T>::vector_t transform<T>::forward() const noexcept { return rotation * vector_t(0, 0, -1); }
  template<std::floating_point T>
  constexpr transform<T>::vector_t transform<T>::backward() const noexcept { return rotation * vector_t(0, 0, 1); }
}