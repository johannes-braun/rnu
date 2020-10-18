#pragma once

#include "vec.hpp"

template<typename T>
constexpr T pi = T(3.14159265359);
template<typename T>
constexpr T inv_pi = 1 / T(3.14159265359);

namespace gfx {
template<typename T>
class tquat : public vec<T, 4>
{
    struct init_t_vec_t
    {};

    constexpr tquat(init_t_vec_t, T w, vec<T, 3>&& v) : vec<T, 4>(v.x, v.y, v.z, w) {}

public:
    using vec_type = vec<T, 4>;

    constexpr tquat(T w, T x, T y, T z) : vec<T, 4>(x, y, z, w) {}
    constexpr tquat(T angle, const vec<T, 3>& axis) : tquat(init_t_vec_t{}, std::cos(angle / T(2)), std::sin(angle / T(2)) * axis) {}

    constexpr tquat(identity_t) : vec<T, 4>(0, 0, 0, 0) {}

    constexpr operator mat<T, 3>() const
    {
        const auto x2 = this->x * this->x;
        const auto y2 = this->y * this->y;
        const auto z2 = this->z * this->z;

        const auto xy = this->x * this->y;
        const auto xz = this->x * this->z;
        const auto yz = this->y * this->z;
        const auto wx = this->w * this->x;
        const auto wy = this->w * this->y;
        const auto wz = this->w * this->z;
		return mat<T, 3>(vec<T, 3>{1 - 2 * y2 - 2 * z2, 2 * xy - 2 * wz, 2 * xz + 2 * wy},
			vec<T, 3>{2 * xy + 2 * wz, 1 - 2 * x2 - 2 * z2, 2 * xz - 2 * wx},
			vec<T, 3>{2 * xz - 2 * wy, 2 * yz + 2*wx, 1 - 2 * x2 - 2 * y2});
    }

    constexpr tquat operator*(const tquat& other) const noexcept
    {
        const auto& w0 = this->w;
        const auto& w1 = other.w;

        const auto& v0 = reinterpret_cast<const vec<T, 3>&>(this->at(0));
        const auto& v1 = reinterpret_cast<const vec<T, 3>&>(other.at(0));

        const auto& xyz = w0 * v1 + w1 * v0 + cross(v0, v1);
        const auto  w   = w0 * w1 - dot(v0, v1);
        return tquat(w, xyz.x, xyz.y, xyz.z);
    }
    constexpr tquat& operator*=(const tquat& other) noexcept
    {
        const auto& w0 = this->w;
        const auto& w1 = other.w;

        const auto& v0 = reinterpret_cast<const vec<T, 3>&>(this->at(0));
        const auto& v1 = reinterpret_cast<const vec<T, 3>&>(other.at(0));

        reinterpret_cast<const vec<T, 3>&>(this->at(0)) = w0 * v1 + w1 * v0 + cross(v0, v1);
        this->w                                         = w0 * w1 - dot(v0, v1);
        return *this;
    }

    constexpr tquat  operator/(const tquat& other) const noexcept;
    constexpr tquat& operator/=(const tquat& other) const noexcept;

    const vec<T, 3>& xyz() const noexcept { return reinterpret_cast<const vec<T, 3>&>(this->at(0)); }
    vec<T, 3>&       xyz() noexcept { return reinterpret_cast<vec<T, 3>&>(this->at(0)); }

private:
};

using quat = tquat<float>;
}    // namespace v1
}    // namespace gfx

namespace std {
template<typename T>
constexpr gfx::tquat<T> conj(const gfx::tquat<T>& q)
{
    return gfx::tquat<T>(q.w, -q.x, -q.y, -q.z);
}
}    // namespace std

namespace gfx {
inline namespace v1 {
using std::conj;

template<typename T>
constexpr vec<T, 3> operator*(const tquat<T>& q, const vec<T, 3>& v) noexcept
{
	const auto t = T(2) * cross(q.xyz(), v);
	return v + q.w * t + cross(q.xyz(), t);
}

template<typename T>
constexpr vec<T, 3> operator*(const vec<T, 3>& v, const tquat<T>& q) noexcept
{
	return q * inverse(v);
}

template<typename T>
constexpr tquat<T> operator/(const tquat<T>& q, const T& v) noexcept
{
	return tquat<T>(q.w / v, q.x / v, q.y / v, q.z / v);
}

template<typename T>
constexpr tquat<T> operator/(const T& v, const tquat<T>& q) noexcept
{
	return tquat<T>(v / q.w, v / q.x, v / q.y, v / q.z);
}

template<typename T>
constexpr tquat<T> operator+(const tquat<T>& q, const T& v) noexcept
{
	return tquat<T>(q.w + v, q.x + v, q.y + v, q.z + v);
}

template<typename T>
constexpr tquat<T> operator+(const T& v, const tquat<T>& q) noexcept
{
	return tquat<T>(v + q.w, v + q.x, v + q.y, v + q.z);
}

template<typename T>
constexpr tquat<T> operator-(const tquat<T>& q, const T& v) noexcept
{
	return tquat<T>(q.w - v, q.x - v, q.y - v, q.z - v);
}

template<typename T>
constexpr tquat<T> operator-(const T& v, const tquat<T>& q) noexcept
{
	return tquat<T>(v - q.w, v - q.x, v - q.y, v - q.z);
}

template<typename T>
constexpr mat<T, 3> to_mat(const tquat<T>& q) noexcept
{
	return static_cast<mat<T, 3>>(q);
}

template<typename T = float>
constexpr T to_deg(const long double& rad) noexcept
{
    return T(inv_pi<long double> * 180 * rad);
}

template<typename T = float>
constexpr T to_rad(const long double& deg) noexcept
{
    constexpr long double inv_180 = 1.0 / 180.0;
    return T(pi<long double> * deg * inv_180);
}

template<typename T>
constexpr gfx::tquat<T> inverse(const gfx::tquat<T>& q) noexcept
{
    const auto n = norm(q);
    return conj(q) / n;
}

template<typename T>
constexpr tquat<T> tquat<T>::operator/(const tquat<T>& other) const noexcept
{
    return *this * inverse(other);
}
template<typename T>
constexpr tquat<T>& tquat<T>::operator/=(const tquat& other) const noexcept
{
    return *this *= inverse(other);
}
}    // namespace gfx