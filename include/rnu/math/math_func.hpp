#pragma once
#include "vec_type.hpp"
#include <cmath>
#include <algorithm>
#include <numbers>
#include <concepts>
#include <complex>
#include <numeric>

namespace rnu 
{
    template<std::floating_point T> [[nodiscard]] constexpr T sqrt(T value) noexcept;
    template<std::floating_point T> [[nodiscard]] constexpr T cos(T val) noexcept;
    template<std::floating_point T> [[nodiscard]] constexpr T sin(T val) noexcept;
    template<std::floating_point T> [[nodiscard]] constexpr T tan(T val) noexcept;
    template<std::floating_point T> [[nodiscard]] constexpr bool isnan(T val) noexcept;
    template<std::floating_point T> [[nodiscard]] constexpr bool isinf(T val) noexcept;
    template<std::floating_point T> [[nodiscard]] constexpr T real(T val) noexcept { return std::real(val); }
    template<std::floating_point T> [[nodiscard]] constexpr T imag(T val) noexcept { return std::imag(val); }
    template<scalar_type T> [[nodiscard]] constexpr T max(T val, T val2) noexcept { return std::max(val, val2); }
    template<scalar_type T> [[nodiscard]] constexpr T min(T val, T val2) noexcept { return std::min(val, val2); }
    template<scalar_type T> [[nodiscard]] constexpr T clamp(T val, T low, T high) noexcept { return std::clamp(val, low, high); }

#define vectorize_fun(Name, Typename) \
    template<Typename T> \
    [[nodiscard]] constexpr auto Name(T val) noexcept(noexcept(Name<typename T::value_type>(typename T::value_type{}))) \
    { return detail::apply_for_each(val, &Name<typename T::value_type>); }
#define vectorize_fun2(Name, Typename) \
    template<Typename T> \
    [[nodiscard]] constexpr auto Name(T val, T val2) noexcept(noexcept(Name<typename T::value_type>(typename T::value_type{}, typename T::value_type{}))) \
    { return detail::apply_for_each(val, val2, &Name<typename T::value_type>); }

    vectorize_fun(sqrt, floating_point_vector);
    vectorize_fun(cos, floating_point_vector);
    vectorize_fun(sin, floating_point_vector);
    vectorize_fun(tan, floating_point_vector);
    vectorize_fun(isnan, floating_point_vector);
    vectorize_fun(isinf, floating_point_vector);
    vectorize_fun(real, floating_point_vector);
    vectorize_fun(imag, floating_point_vector);

    vectorize_fun2(max, vector_type);
    vectorize_fun2(min, vector_type);

    template<typename T> 
    [[nodiscard]] constexpr auto clamp(T val, T low, T high) 
        noexcept(noexcept(clamp<typename T::value_type>(typename T::value_type{}, typename T::value_type{}, typename T::value_type{})))
    { 
        for (size_t i = 0; i < T::component_count; ++i)
        {
            val[i] = clamp(val[i], low[i], high[i]);
        }
        return val;
    }

#undef vectorize_fun
}
#include "math_func.inl.hpp"
#include "vec_math.inl.hpp"