#pragma once

#include "vec.hpp"
#include <iostream>
#include <utility>

namespace gfx {
struct identity_t
{};
constexpr identity_t identity;
struct diagonal_t
{};
constexpr diagonal_t diagonal;

template<typename T, size_t SizeMajor, size_t SizeMinor = SizeMajor>
class mat : public vec<vec<T, SizeMinor>, SizeMajor>
{
public:
    using nested_vector_type = vec<vec<T, SizeMinor>, SizeMajor>;

    using nested_vector_type::vec;
    constexpr mat(const nested_vector_type& val) : nested_vector_type(val) {}

private:
    constexpr static mat make_ident()
    {
        return detail::apply_for_each<T, SizeMajor>([](size_t idx) mutable {
            vec<T, SizeMinor> rv(0);
            rv[SizeMinor - 1 - idx] = 1;
            return rv;
        });
    }
    constexpr static mat make_diag(T value)
    {
        return detail::apply_for_each<T, SizeMajor>([value](size_t idx) mutable {
            vec<T, SizeMinor> rv(0);
            rv[SizeMinor - 1 - idx] = value;
            return rv;
        });
    }

public:
    template<typename X, size_t Maj, size_t Min>
    constexpr mat<X, Maj, Min> sub(size_t offset_maj, size_t offset_min, size_t copy_range_maj, size_t copy_range_min) const
    {
        mat<X, Maj, Min> m;
        for (auto maj = offset_maj; maj < offset_maj + copy_range_maj; ++maj)
            for (auto min = offset_min; min < offset_min + copy_range_min; ++min) m[maj - offset_maj][min - offset_min] = (*this)[maj][min];
        return m;
    }
    
    constexpr mat(identity_t ident = gfx::identity) : nested_vector_type(make_ident()) {}

    constexpr mat(T value, diagonal_t) : nested_vector_type(make_diag(value)) {}

    template<size_t X, size_t Y>
    constexpr mat(const mat<T, X, Y>& m)
    {
        const size_t copy_range_maj = std::min(X, SizeMajor);
        const size_t copy_range_min = std::min(Y, SizeMinor);
        for (auto maj = 0; maj < copy_range_maj; ++maj)
            for (auto min = 0; min < copy_range_min; ++min) (*this)[maj][min] = (m)[maj][min];
    }

    constexpr static size_t rows() noexcept { return SizeMinor; }
    constexpr static size_t cols() noexcept { return SizeMajor; }
};

using mat4  = mat<float, 4>;
using mat3  = mat<float, 3>;
using mat2  = mat<float, 2>;
using dmat4 = mat<double, 4>;
using dmat3 = mat<double, 3>;
using dmat2 = mat<double, 2>;
template<typename T, size_t Maj, size_t Min>
using cmat = mat<std::complex<T>, Maj, Min>;

template<typename T, size_t Maj, size_t Min>
constexpr auto rows(const mat<T, Maj, Min>& matrix) noexcept;
template<typename T, size_t Maj, size_t Min>
constexpr auto cols(const mat<T, Maj, Min>& matrix) noexcept;
template<typename T, size_t S>
constexpr auto rows(const vec<T, S>& vector) noexcept;
template<typename T, size_t S>
constexpr auto cols(const vec<T, S>& vector) noexcept;

template<typename T, size_t Maj, size_t Min = Maj>
constexpr mat<T, Maj, Min> make_diag(T value) noexcept;
template<typename T, size_t Maj, size_t Min = Maj>
constexpr mat<T, Maj, Min> make_identity() noexcept;

template<typename T, size_t S>
constexpr T det(const mat<T, S, S>& m) noexcept;

template<typename T, size_t SizeMajorA, size_t SizeMinorA, size_t SizeMajorB>
constexpr mat<T, SizeMajorB, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a,
                                                   const mat<T, SizeMajorB, SizeMajorA>& b) noexcept;
template<typename T, size_t SizeMajorA, size_t SizeMinorA>
constexpr vec<T, SizeMinorA> operator*(const mat<T, SizeMajorA, SizeMinorA>& a, const vec<T, SizeMajorA>& b) noexcept;
template<typename T, size_t A, size_t B>
constexpr auto outer_prod(const vec<T, A>& a, const vec<T, B>& b) noexcept;

template<size_t Maj, size_t Min>
constexpr bool any_of(const mat<bool, Maj, Min>& v) noexcept;
template<size_t Maj, size_t Min>
constexpr bool all_of(const mat<bool, Maj, Min>& v) noexcept;
template<size_t Maj, size_t Min>
constexpr bool none_of(const mat<bool, Maj, Min>& v) noexcept;
template<typename T, size_t Maj, size_t Min>
constexpr auto min_element(const mat<T, Maj, Min>& m) noexcept;
template<typename T, size_t Maj, size_t Min>
constexpr auto max_element(const mat<T, Maj, Min>& m) noexcept;
template<typename T, size_t Maj, size_t Min>
constexpr auto minmax_element(const mat<T, Maj, Min>& m) noexcept;

template<typename T, size_t S>
constexpr void transpose(mat<T, S, S>& m) noexcept;
template<typename T, size_t Maj, size_t Min>
constexpr mat<T, Min, Maj> transposed(const mat<T, Maj, Min>& in) noexcept;
template<typename T, size_t S>
constexpr mat<T, S, S> transposed(const mat<T, S, S>& in) noexcept;

template<typename T, size_t S>
constexpr mat<T, S, S> inverse(const mat<T, S, S>& m) noexcept;
template<typename T, size_t S>
constexpr mat<T, S, S> adjoint(const mat<T, S, S>& m) noexcept;
}    // namespace gfx

#include "mat/mat_apply.inl"
#include "mat/mat_cond.inl"
#include "mat/mat_det.inl"
#include "mat/mat_get.inl"
#include "mat/mat_make.inl"
#include "mat/mat_modify.inl"
#include "mat/mat_mul.inl"
#include "mat/mat_ops.inl"