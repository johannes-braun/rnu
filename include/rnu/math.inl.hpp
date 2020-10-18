#pragma once
#include <array>
#include <cmath>
#include <type_traits>
#include <numbers>

#pragma warning(push)
#pragma warning(disable : 5063)

namespace rnu {
    template<typename T> constexpr T pi = static_cast<T>(3.14159265358979323846264338327950);
    template<typename T> constexpr T pi_inv = static_cast<T>(1.0 / 3.14159265358979323846264338327950);
    
    template<typename Float>
    struct quat
    {
        using float_type = Float;

        [[nodiscard]] constexpr float_type const& operator[](ptrdiff_t index) const noexcept
        {
            return *(&w + index);
        }

        [[nodiscard]] constexpr float_type& operator[](ptrdiff_t index) noexcept
        {
            return *(&w + index);
        }

        [[nodiscard]] constexpr quat<Float> operator*(quat<Float> const& b) const {
            const auto& a = *this;
            return {
                a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3],
                a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2],
                a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1],
                a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0],
            };
        }
        
        template<typename OtherFloat>
        [[nodiscard]] constexpr explicit operator quat<OtherFloat>() const noexcept{
            return {
                static_cast<OtherFloat>(w),
                static_cast<OtherFloat>(x),
                static_cast<OtherFloat>(y),
                static_cast<OtherFloat>(z)
            };
        }

        float_type w;
        float_type x;
        float_type y;
        float_type z;
    };

    template<typename Float, size_t S>
    struct vec;

    template<typename Float>
    struct vec<Float, 3>
    {
        using float_type = Float;

        [[nodiscard]] constexpr float_type const& operator[](ptrdiff_t index) const noexcept
        {
            return *(&x + index);
        }
        [[nodiscard]] constexpr float_type& operator[](ptrdiff_t index) noexcept
        {
            return *(&x + index);
        }

        template<typename OtherFloat>
        [[nodiscard]] constexpr explicit operator vec<OtherFloat, 3>() const noexcept{
            return {
                static_cast<OtherFloat>(x),
                static_cast<OtherFloat>(y),
                static_cast<OtherFloat>(z)
            };
        }

        float_type x;
        float_type y;
        float_type z;
    };

    template<typename Float, size_t S>
    [[nodiscard]] constexpr vec<Float, S> constant(Float scalar) noexcept {
        vec<Float, S> v{0};
        for (size_t i = 0; i < S; ++i)
            v[i] = scalar;
        return v;
    }

    template<typename Float, size_t S>
    [[maybe_unused]] constexpr vec<Float, S>& operator+=(vec<Float, S>& lhs, vec<Float, S> const& rhs) noexcept
    {
        for (size_t i = 0; i < S; ++i)
            lhs[i] = lhs[i] + rhs[i];
        return lhs;
    }
    template<typename Float, size_t S>
    [[nodiscard]] constexpr vec<Float, S> operator+(vec<Float, S> const& lhs, vec<Float, S> const& rhs) noexcept
    {
        vec<Float, S> value{};
        for (size_t i = 0; i < S; ++i)
            value[i] = lhs[i] + rhs[i];
        return value;
    }
    template<typename Float, size_t S>
    [[nodiscard]] constexpr vec<Float, S> operator*(Float scalar, vec<Float, S> const& vector) noexcept
    {
        vec<Float, S> value{};
        for (size_t i = 0; i < S; ++i)
            value[i] = scalar * vector[i];
        return value;
    }
    template<typename Float, size_t S>
    [[nodiscard]] constexpr vec<Float, S> operator*(vec<Float, S> const& vector, Float scalar) noexcept
    {
        vec<Float, S> value{};
        for (size_t i = 0; i < S; ++i)
            value[i] = vector[i] * scalar;
        return value;
    }

    template<typename T>
    [[nodiscard]] constexpr T sqrt(T value)
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
    template<typename T>
    [[nodiscard]] constexpr T cos(T val) {
        if constexpr (std::is_constant_evaluated())
        {
            // modulate to [0:2pi) range
            val = val < 0 ? -val : val;
            val = val * (T(0.5) * pi_inv<T>);
            val = 2 * pi<T> * (val - (long long)(val));

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
    template<typename T>
    [[nodiscard]] constexpr T sin(T val) {
        if constexpr (std::is_constant_evaluated())
        {
            return cos(T(0.5) * pi<T> -val);
        }
        else
        {
            return static_cast<T>(std::sin(static_cast<double>(val)));
        }
    }
    template<typename T>
    [[nodiscard]] constexpr T tan(T val) {
        if constexpr (std::is_constant_evaluated())
        {
            return sin(val) / cos(val);
        }
        else
        {
            return static_cast<T>(std::tan(static_cast<double>(val)));
        }
    }

    template<typename T>
    [[nodiscard]] constexpr vec<T, 3> cross(vec<T, 3> lhs, vec<T, 3> rhs) {
        return {
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]
        };
    }

    template<typename T, size_t S>
    constexpr T dot(vec<T, S> const& lhs, vec<T, S> const& rhs) {
        T value = 0;
        for (size_t i = 0; i < S; ++i)
            value += lhs[i] * rhs[i];
        return value;
    }
    
    template<typename Float>
    [[nodiscard]] constexpr quat<Float> conj(quat<Float> const& quat)
    {
        return {quat.w, -quat.x, -quat.y, -quat.z};
    }

    template<typename Float, size_t S>
    [[nodiscard]] constexpr vec<Float, S> operator*(quat<Float> const& quat, vec<Float, S> const& v) noexcept
    {
        auto const s = quat[0];
        const vec<Float, 3> u{ quat[1], quat[2], quat[3] };

        const auto first = 2 * dot(u, v) * u;
        const auto second = (s * s - dot(u, u)) * v;
        const auto third = 2 * s * cross(u, v);

        return first + second + third;
    }
}
#pragma warning(pop)