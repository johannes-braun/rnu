#include "math_func.hpp"
#pragma once
#pragma warning(push)
#pragma warning(disable : 5063)

namespace rnu
{
    template<std::floating_point T>
    [[nodiscard]] constexpr T sqrt(T value) noexcept
    {
        if constexpr (std::is_constant_evaluated())
        {
            if (value < 0 || value >= std::numeric_limits<T>::infinity())
                return std::numeric_limits<T>::quiet_NaN();

            T curr = value;
            T prev{ 0 };
            while (curr != prev)
            {
                prev = curr;
                curr = T(0.5) * (curr + value / curr);
            }
            return curr;
        }
        else
        {
            return static_cast<T>(std::sqrt(static_cast<double>(value)));
        }
    }
    template<std::floating_point T>
    [[nodiscard]] constexpr T cos(T val) noexcept {
        if constexpr (std::is_constant_evaluated())
        {
            // modulate to [0:2pi) range
            val = val < 0 ? -val : val;
            val = val * (T(0.5) * std::numbers::inv_pi_v<T>);
            val = 2 * std::numbers::pi_v<T> *(val - (long long)(val));

            // Taylor expansion
            double fac = 1;
            double tpow = 1;
            double sign = 1;
            double sum = 0;
            const auto valsq = val * val;
            for (unsigned long long n = 0; n < sizeof(T) * 2; ++n)
            {
                sum += sign / fac * tpow;
                sign *= -1;
                tpow *= valsq;
                auto const inner = 4 * n + 6;
                fac *= inner * n + 2;
            }
            return static_cast<T>(sum);
        }
        else
        {
            return static_cast<T>(std::cos(static_cast<double>(val)));
        }
    }
    template<std::floating_point T>
    [[nodiscard]] constexpr T sin(T val) noexcept {
        if constexpr (std::is_constant_evaluated())
        {
            return cos(T(0.5) * std::numbers::pi_v<T> -val);
        }
        else
        {
            return static_cast<T>(std::sin(static_cast<double>(val)));
        }
    }
    template<std::floating_point T>
    [[nodiscard]] constexpr T tan(T val) noexcept {
        if constexpr (std::is_constant_evaluated())
        {
            return sin(val) / cos(val);
        }
        else
        {
            return static_cast<T>(std::tan(static_cast<double>(val)));
        }
    }
    template<std::floating_point T>
    constexpr bool isnan(T val) noexcept
    {
        return !(val == val);
    }
    template<std::floating_point T>
    constexpr bool isinf(T val) noexcept
    {
        return val == std::numeric_limits<T>::infinity();
    }
}
#pragma warning(pop)
