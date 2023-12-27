#pragma once

#ifndef RNU_MATH_QUAT_TYPE_HPP
#define RNU_MATH_QUAT_TYPE_HPP

#include <array>
#include <concepts>
#include "vec.hpp"
#include "traits.hpp"
#include <cmath>

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
    enum class angle_axis_init_t {};
  public:
    using iterator = typename array_type::iterator;
    using const_iterator = typename array_type::const_iterator;
    using reverse_iterator = typename array_type::reverse_iterator;
    using const_reverse_iterator = typename array_type::const_reverse_iterator;

    constexpr static size_t count = 4;

    [[nodiscard]] constexpr quat_t() noexcept = default;
    template<typename U>
    [[nodiscard]] constexpr quat_t(quat_t<U> const& other) noexcept;
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

    [[nodiscard]] constexpr T* data() noexcept {
      return components.data();
    }
    [[nodiscard]] constexpr T const* data() const noexcept {
      return components.data();
    }
    [[nodiscard]] constexpr size_t size() noexcept {
      return components.size();
    }

    [[nodiscard]] constexpr bool operator==(quat_t<T> other) const noexcept
    {
      return w == other.w && x == other.w && y == other.y && z == other.z;
    }

    [[nodiscard]] constexpr bool operator!=(quat_t<T> other) const noexcept
    {
      return !(*this == other);
    }

    union {
      struct { T w, x, y, z; };
      std::array<T, 4> components{ 1, 0, 0, 0 };
    };
  };

  template<quaternion Q>
  [[nodiscard]] constexpr Q conj(const Q& q) noexcept
  {
    return Q(q[0], -q[1], -q[2], -q[3]);
  }
  template<quaternion Q>
  [[nodiscard]] constexpr auto dot(const Q& q1, const Q& q2) noexcept
  {
    auto const v1 = vec<typename Q::value_type, 4>(q1[0], q1[1], q1[2], q1[3]);
    auto const v2 = vec<typename Q::value_type, 4>(q2[0], q2[1], q2[2], q2[3]);
    return dot(v1, v2);
  }
  template<quaternion Q>
  [[nodiscard]] constexpr auto norm(const Q& q) noexcept
  {
    return norm(vec<typename Q::value_type, 4>(q[0], q[1], q[2], q[3]));
  }
  template<quaternion Q>
  [[nodiscard]] constexpr auto snorm(const Q& q) noexcept
  {
    return dot(q, q);
  }
  template<quaternion Q>
  [[nodiscard]] constexpr auto normalize(Q q) noexcept
  {
    const auto n = norm(q);
    q[0] /= n;
    q[1] /= n;
    q[2] /= n;
    q[3] /= n;
    return q;
  }
  template<quaternion Q>
  [[nodiscard]] constexpr Q inverse(const Q& q) noexcept
  {
    auto c = conj(q);
    return normalize(conj(q));
  }

  template<quaternion Q>
  Q slerp(Q qa, Q qb, float t) {
    /*
     * slerp_cross.c
     *
     * Copyright (C) 2019 bzt (bztsrc@gitlab)
     *
     * Permission is hereby granted, free of charge, to any person
     * obtaining a copy of this software and associated documentation
     * files (the "Software"), to deal in the Software without
     * restriction, including without limitation the rights to use, copy,
     * modify, merge, publish, distribute, sublicense, and/or sell copies
     * of the Software, and to permit persons to whom the Software is
     * furnished to do so, subject to the following conditions:
     *
     * The above copyright notice and this permission notice shall be
     * included in all copies or substantial portions of the Software.
     *
     * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
     * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
     * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
     * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
     * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
     * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
     * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
     * DEALINGS IN THE SOFTWARE.
     *
     * @brief cross-platform optimized SLERP implementation
     * https://gitlab.com/bztsrc/slerp-opt
     *
     * Compile with:
     *   cc slerp_cross.c -o slerp -lm
     *
     */

    // Fix discontinuities due to large paths between quaternions.
    auto const scalar = dot(qa, qb);
    if (scalar < 0)
      qa = -qa;

    typename Q::value_type a = 1.0 - t, b = t, d = scalar, c = fabsf(d);
    if (c < 0.999) {
      c = acosf(c);
      b = 1 / sinf(c);
      a = sinf(a * c) * b;
      b *= sinf(t * c);
    }
    Q ret;
    ret[0] = qa[0] * a + qb[0] * b;
    ret[1] = qa[1] * a + qb[1] * b;
    ret[2] = qa[2] * a + qb[2] * b;
    ret[3] = qa[3] * a + qb[3] * b;
    return ret;
  }

  template<typename F>
  quat_t<F> look_at(vec<F, 3> eye, vec<F, 3> center, vec<F, 3> up)
  {
    mat<F, 3, 3> result;
    result[2] = -normalize(center - eye);
    vec<F, 3> const right = cross(up, result[2]);
    result[0] = right / std::sqrt(std::max(static_cast<F>(1e-5), dot(right, right)));
    result[1] = cross(result[2], result[0]);
    auto const t = transform<F>(result);
    return t.rotation;
  }
}

#include "quat_type.inl.hpp"

#endif // RNU_MATH_QUAT_TYPE_HPP
