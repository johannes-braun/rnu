#pragma once

namespace gfx
{
    template<typename T, size_t SizeMajorA, size_t SizeMinorA, size_t SizeMajorB>
    constexpr mat<T, SizeMajorB, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const mat<T, SizeMajorB, SizeMajorA>& b) noexcept
    {
        mat<T, SizeMajorB, SizeMinorA> result = detail::apply_for_each_indexed<T, SizeMajorB, SizeMinorA>([&](size_t col, size_t row) {
            T val = T();
            for (size_t i = 0; i < SizeMajorA; ++i) { val += a[i][row] * b[col][i]; }
            return val;
            });
        return result;
    }
    template<typename T, size_t SizeMajorA, size_t SizeMinorA>
    constexpr vec<T, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const vec<T, SizeMajorA>& b) noexcept
    {
        const auto result = detail::apply_for_each<T, SizeMinorA>([&](size_t min) {
            T val = T();
            for (size_t i = 0; i < SizeMajorA; ++i) { val += a[i][min] * b[i]; }
            return val;
            });
        return reinterpret_cast<const vec<T, SizeMinorA>&>(result);
    }
    template<typename T, size_t SizeMajorA, size_t SizeMinorA, size_t SizeMajorB>
    constexpr mat<T, SizeMajorB, SizeMinorA> operator/(const mat<T, SizeMajorA, SizeMinorA>& a, const mat<T, SizeMajorB, SizeMajorA>& b) noexcept
    {
        return a * inverse(b);
    }
    template<typename T, size_t SizeMajorA, size_t SizeMinorA>
    constexpr vec<T, SizeMinorA> operator*(const vec<T, SizeMajorA>& b, const mat<T, SizeMajorA, SizeMinorA>& a) noexcept
    {
        return inverse(a) * b;
    }

    template<typename T, size_t A, size_t B>
    constexpr auto outer_prod(const vec<T, A>& a, const vec<T, B>& b) noexcept
    {
        const auto& a_mat = reinterpret_cast<const mat<T, 1, A>&>(a);
        const auto& b_mat = reinterpret_cast<const mat<T, B, 1>&>(b);

        return a_mat * b_mat;
    }
}