#pragma once

#include "vec_type.hpp"
#include "mat_type.hpp"
#include "math_func.hpp"
#include <array>
#include <concepts>

namespace rnu {
    template<std::floating_point T = float>
    class quat_t {
    public:
        using reference = T&;
        using const_reference = const T&;
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using scalar_type = value_type;

    private:
        using array_type = std::array<value_type, 4>;
        enum class init_from_angleaxis;
    public:
        using iterator = typename array_type::iterator;
        using const_iterator = typename array_type::const_iterator;
        using reverse_iterator = typename array_type::reverse_iterator;
        using const_reverse_iterator = typename array_type::const_reverse_iterator;

        constexpr static size_t component_count = 4;

        [[nodiscard]] constexpr quat_t() noexcept = default;
        [[nodiscard]] constexpr quat_t(quat_t const& other) noexcept : components{ other.w, other.x, other.y, other.z } {}
        [[nodiscard]] constexpr quat_t(T w, T x, T y, T z) noexcept : components{ w, x, y, z } {}
        [[nodiscard]] constexpr quat_t(T angle, const vec<T, 3>& axis) : quat_t(init_from_angleaxis{}, std::cos(angle / T(2)), std::sin(angle / T(2))* axis) {}
    
    private:
        [[nodiscard]] constexpr quat_t(init_from_angleaxis, T w, vec<T, 3> xyz) : components{ w, xyz.x, xyz.y, xyz.z } {}

    public:
        [[nodiscard]] constexpr mat<T, 3, 3> matrix() const noexcept {
            const auto x2 = this->x * this->x;
            const auto y2 = this->y * this->y;
            const auto z2 = this->z * this->z;

            const auto xy = this->x * this->y;
            const auto xz = this->x * this->z;
            const auto yz = this->y * this->z;
            const auto wx = this->w * this->x;
            const auto wy = this->w * this->y;
            const auto wz = this->w * this->z;
            return mat<T, 3, 3>(vec<T, 3>{1 - 2 * y2 - 2 * z2, 2 * xy - 2 * wz, 2 * xz + 2 * wy},
                vec<T, 3>{2 * xy + 2 * wz, 1 - 2 * x2 - 2 * z2, 2 * xz - 2 * wx},
                vec<T, 3>{2 * xz - 2 * wy, 2 * yz + 2 * wx, 1 - 2 * x2 - 2 * y2});
        }

        template<typename T>
        [[nodiscard]] explicit constexpr operator quat_t<T>() noexcept {
            return quat_t<T>(
                static_cast<T>(w),
                static_cast<T>(x), 
                static_cast<T>(y), 
                static_cast<T>(z));
        }
        constexpr quat_t operator*(const quat_t& other) const noexcept
        {
            const auto w0 = this->w;
            const auto w1 = other.w;

            const vec<T, 3> v0(x, y, z);
            const vec<T, 3> v1(other.x, other.y, other.z);

            const auto xyz = w0 * v1 + w1 * v0 + cross(v0, v1);
            const auto  w = w0 * w1 - dot(v0, v1);
            return quat_t(w, xyz.x, xyz.y, xyz.z);
        }
        constexpr quat_t& operator*=(const quat_t& other) noexcept
        {
            *this = *this * other;
            return *this;
        }
        [[nodiscard]] constexpr vec<T, 3> operator*(const vec<T, 3>& v) const noexcept
        {
            const auto t = T(2) * cross(vec<T, 3>(x, y, z), v);
            return v + w * t + cross(vec<T, 3>(x, y, z), t);
        }

        constexpr reference       at(size_type index) { return components[index]; }
        constexpr const_reference at(size_type index) const { return components[index]; }
        constexpr reference       operator[](size_type index) { return components[index]; }
        constexpr const_reference operator[](size_type index) const { return components[index]; }

        [[nodiscard]] auto begin() { return reinterpret_cast<array_type&>(this->components).begin(); }
        [[nodiscard]] auto end() { return reinterpret_cast<array_type&>(this->components).end(); }
        [[nodiscard]] auto begin() const { return reinterpret_cast<const array_type&>(this->components).begin(); }
        [[nodiscard]] auto end() const { return reinterpret_cast<const array_type&>(this->components).end(); }
        [[nodiscard]] auto cbegin() const { return reinterpret_cast<const array_type&>(this->components).begin(); }
        [[nodiscard]] auto cend() const { return reinterpret_cast<const array_type&>(this->components).end(); }
        [[nodiscard]] auto rbegin() { return reinterpret_cast<array_type&>(this->components).rbegin(); }
        [[nodiscard]] auto rend() { return reinterpret_cast<array_type&>(this->components).rend(); }
        [[nodiscard]] auto rbegin() const { return reinterpret_cast<const array_type&>(this->components).rbegin(); }
        [[nodiscard]] auto rend() const { return reinterpret_cast<const array_type&>(this->components).rend(); }
        [[nodiscard]] auto crbegin() const { return reinterpret_cast<const array_type&>(this->components).rbegin(); }
        [[nodiscard]] auto crend() const { return reinterpret_cast<const array_type&>(this->components).rend(); }
        
        union {
            struct {
                T w, x, y, z;
            };
            T components[4]{ 1, 0, 0, 0 };
        };
    };

    template<quaternion_type Q>
    [[nodiscard]] constexpr Q conj(const Q& q) noexcept
    {
        return Q(q.w, -q.x, -q.y, -q.z);
    }
    template<quaternion_type Q>
    [[nodiscard]] constexpr auto norm(const Q& q) noexcept
    {
        return norm(vec<typename Q::value_type, 4>(q.w, q.x, q.y, q.z));
    }
    template<quaternion_type Q>
    [[nodiscard]] constexpr auto normalize(Q q) noexcept
    {
        const auto n = norm(q);
        q.w /= n;
        q.x /= n;
        q.y /= n;
        q.z /= n;
        return q;
    }
    template<quaternion_type Q>
    [[nodiscard]] constexpr Q inverse(const Q& q) noexcept
    {
        auto c = conj(q);
        return normalize(conj(q));
    }
}