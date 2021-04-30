#pragma once

#ifndef RNU_MATH_QUAT_TYPE_HPP
#define RNU_MATH_QUAT_TYPE_HPP

#include <array>
#include <concepts>
#include "vec.hpp"
#include "traits.hpp"

namespace rnu {
  template<typename T = float>
  struct quat_t {
  public:
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using scalar_type = value_type;
    using angle_axis_type = vec<T, 3>;

  private:
    using array_type = std::array<value_type, 4>;
    enum class angle_axis_init_t;
  public:
    using iterator = typename array_type::iterator;
    using const_iterator = typename array_type::const_iterator;
    using reverse_iterator = typename array_type::reverse_iterator;
    using const_reverse_iterator = typename array_type::const_reverse_iterator;

    constexpr static size_t count = 4;

    [[nodiscard]] constexpr quat_t() noexcept = default;
    [[nodiscard]] constexpr quat_t(quat_t const& other) noexcept;
    [[nodiscard]] constexpr quat_t(T w, T x, T y, T z) noexcept;
    [[nodiscard]] constexpr quat_t(angle_axis_type axis, T angle);

  private:
    [[nodiscard]] constexpr quat_t(angle_axis_init_t, T w, vec<T, 3> xyz);

  public:
    [[nodiscard]] constexpr mat<T, 3, 3> matrix() const noexcept;

    template<typename V>
    [[nodiscard]] explicit constexpr operator quat_t<V>() noexcept;

    constexpr quat_t& operator*=(const quat_t& other) noexcept;
    [[nodiscard]] constexpr quat_t operator*(const quat_t& other) const noexcept;
    [[nodiscard]] constexpr vec<T, 3> operator*(const vec<T, 3>& v) const noexcept;

    constexpr reference       at(size_type index) { return components[index]; }
    constexpr const_reference at(size_type index) const { return components[index]; }
    constexpr reference       operator[](size_type index) { return components[index]; }
    constexpr const_reference operator[](size_type index) const { return components[index]; }

    [[nodiscard]] constexpr iterator begin() { return std::begin(this->components); }
    [[nodiscard]] constexpr iterator end() { return std::end(this->components); }
    [[nodiscard]] constexpr const_iterator begin() const { return std::begin(this->components); }
    [[nodiscard]] constexpr const_iterator end() const { return std::end(this->components); }
    [[nodiscard]] constexpr const_iterator cbegin() const { return std::cbegin(this->components); }
    [[nodiscard]] constexpr const_iterator cend() const { return std::cend(this->components); }
    [[nodiscard]] constexpr reverse_iterator rbegin() { return std::rbegin(this->components); }
    [[nodiscard]] constexpr reverse_iterator rend() { return std::rend(this->components); }
    [[nodiscard]] constexpr reverse_iterator rbegin() const { return std::rbegin(this->components); }
    [[nodiscard]] constexpr reverse_iterator rend() const { return std::rend(this->components); }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const { return std::crbegin(this->components); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const { return std::crend(this->components); }

    union {
      struct { T w, x, y, z; };
      std::array<T, 4> components{ 1, 0, 0, 0 };
    };
  };

  template<quaternion Q>
  [[nodiscard]] constexpr Q conj(const Q& q) noexcept
  {
    return Q(q.w, -q.x, -q.y, -q.z);
  }
  template<quaternion Q>
  [[nodiscard]] constexpr auto norm(const Q& q) noexcept
  {
    return norm(vec<typename Q::value_type, 4>(q.w, q.x, q.y, q.z));
  }
  template<quaternion Q>
  [[nodiscard]] constexpr auto normalize(Q q) noexcept
  {
    const auto n = norm(q);
    q.w /= n;
    q.x /= n;
    q.y /= n;
    q.z /= n;
    return q;
  }
  template<quaternion Q>
  [[nodiscard]] constexpr Q inverse(const Q& q) noexcept
  {
    auto c = conj(q);
    return normalize(conj(q));
  }
}

#include "quat_type.inl.hpp"

#endif // RNU_MATH_QUAT_TYPE_HPP
