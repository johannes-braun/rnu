#pragma once

#include <rnu/math/math.hpp>
#include <concepts>

namespace rnu
{
  template<std::floating_point T>
  struct transform
  {
    static constexpr auto equal_epsilon = T{ 1e-4 };

    using matrix_t = mat4_t<T>;
    using vector_t = vec3_t<T>;
    using homogeneous_t = vec4_t<T>;
    using rotation_t = quat_t<T>;

    [[nodiscard]] constexpr transform() = default;
    [[nodiscard]] constexpr transform(transform&& other) = default;
    [[nodiscard]] constexpr transform(const transform& other) = default;
    [[nodiscard]] constexpr transform& operator=(transform&& other) = default;
    [[nodiscard]] constexpr transform& operator=(const transform& other) = default;
    [[nodiscard]] constexpr transform(vector_t const& position, vector_t const& scale = { 1, 1, 1 }, rotation_t const& rotation = { 1, 0, 0, 0 }) noexcept;
    [[nodiscard]] constexpr transform(matrix_t const& mat) noexcept;

    [[nodiscard]] constexpr bool equals(const transform& other, T epsilon) const noexcept;
    [[nodiscard]] constexpr bool operator==(const transform& other) const noexcept;
    [[nodiscard]] constexpr bool operator!=(const transform& other) const noexcept;

    [[nodiscard]] constexpr transform  operator*(const transform& other) const noexcept;
    constexpr transform& operator*=(const transform& other) noexcept;

    [[nodiscard]] constexpr matrix_t matrix() const noexcept;
    [[nodiscard]] constexpr operator matrix_t() const noexcept;

    [[nodiscard]] constexpr vector_t up() const noexcept;
    [[nodiscard]] constexpr vector_t down() const noexcept;
    [[nodiscard]] constexpr vector_t left() const noexcept;
    [[nodiscard]] constexpr vector_t right() const noexcept;
    [[nodiscard]] constexpr vector_t forward() const noexcept;
    [[nodiscard]] constexpr vector_t backward() const noexcept;

    vector_t position{ 0, 0, 0 };
    vector_t scale{ 1, 1, 1 };
    rotation_t rotation{ 1, 0, 0, 0 };
  };
}
 
#include "transform.hpp.inl"
