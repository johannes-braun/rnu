#pragma once
#ifndef RNU_MATH_CX_FUN_HPP
#define RNU_MATH_CX_FUN_HPP

#include <bit>
#include <cinttypes>
#include <numbers>
#include <cmath>
#include <type_traits>
#include "traits.hpp"

namespace rnu::detail
{
  constexpr double eps = 1e-6;
}

namespace rnu::cx
{
  /* Get two 32 bit ints from a double.  */
  constexpr auto extract_words(int32_t& hi, int32_t& lo, double d) {
    const uint64_t cvt = std::bit_cast<uint64_t>(d);
    hi = cvt >> 32;
    lo = (uint32_t)cvt;
  }
  /* Set a double from two 32 bit ints.  */
  constexpr auto insert_words(double& d, uint32_t hi, uint32_t lo) {
    d = std::bit_cast<double>(((uint64_t)(hi) << 32) | (uint32_t)(lo));
  }

  constexpr double modf(double x, double* iptr)
  {
    if (std::is_constant_evaluated()) {
      constexpr double one = 1.0;
      int32_t i0, i1, _j0;
      int32_t i;
      extract_words(i0, i1, x);
      _j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;	/* exponent of x */
      if (_j0 < 20) {			/* integer part in high x */
        if (_j0 < 0) {			/* |x|<1 */
          insert_words(*iptr, i0 & 0x80000000, 0);	/* *iptr = +-0 */
          return x;
        }
        else {
          i = (0x000fffff) >> _j0;
          if (((i0 & i) | i1) == 0) {		/* x is integral */
            *iptr = x;
            insert_words(x, i0 & 0x80000000, 0);	/* return +-0 */
            return x;
          }
          else {
            insert_words(*iptr, i0 & (~i), 0);
            return x - *iptr;
          }
        }
      }
      else if (_j0 > 51) {		/* no fraction part */
        *iptr = x * one;
        /* We must handle NaNs separately.  */
        if (_j0 == 0x400 && ((i0 & 0xfffff) | i1))
          return x * one;
        insert_words(x, i0 & 0x80000000, 0);	/* return +-0 */
        return x;
      }
      else {			/* fraction part in low x */
        i = ((int32_t)(0xffffffff)) >> (_j0 - 20);
        if ((i1 & i) == 0) { 		/* x is integral */
          *iptr = x;
          insert_words(x, i0 & 0x80000000, 0);	/* return +-0 */
          return x;
        }
        else {
          insert_words(*iptr, i0, i1 & (~i));
          return x - *iptr;
        }
      }
    }
    else
    {
      return std::modf(x, iptr);
    }
  }

  template<typename T> requires requires(T t) { { T(t < 0 ? -t : t) }; }
  constexpr T abs(T x) {
    return T(x < 0 ? -x : x);
  }
  template<std::floating_point T>
  constexpr T round(T x) {
    if (std::is_constant_evaluated()) {
      x += static_cast<T>(x > 0 ? -0.5 : 0.5);
      double v;
      modf(x, &v);
      return static_cast<T>(v);
    }
    else
    {
      return std::round(x);
    }
  }

  template<std::floating_point T>
  constexpr T sqrt(T x) {
    if (std::is_constant_evaluated()) {
      T a = 0, b = x > 1 ? x : 1;
      while (abs(a - b) > detail::eps) {
        T y = (a + b) / 2;
        if (y * y > x) b = y; else a = y;
      }
      return a;
    }
    else
    {
      return std::sqrt(x);
    }
  }

  template<std::floating_point T, typename R>
  constexpr T pow(T x, R y) {
    if (std::is_constant_evaluated()) {
      if constexpr (std::integral<R>)
      {
        T result = x;
        for (R i = 0; i < y; ++i)
          result *= x;
        return result;
      }
      else
      {
        auto _y = static_cast<double>(y);
        if (x < 0 && abs(round(_y) - _y) < detail::eps) {
          return pow(-x, _y) * ((int)round(_y) % 2 == 1 ? -1 : 1);
        }
        else if (_y < 0) {
          return 1 / pow(x, -_y);
        }
        else if (_y > 1) {
          return pow(x * x, _y / 2);
        }
        else {
          double fraction = 1;
          double result = 1;

          while (_y > detail::eps) {
            if (_y >= fraction) {
              _y -= fraction;
              result *= x;
            }

            fraction /= 2;
            x = sqrt(x);
          }
          return static_cast<T>(result);
        }
      }
    }
    else
    {
      return static_cast<T>(std::pow(x, y));
    }
  }

  constexpr double fmod(double x, double d) {
    if (std::is_constant_evaluated()) {
      double throwaway;
      return modf(x / d, &throwaway) * d;
    }
    else
    {
      return std::fmod(x, d);
    }
  }

  template<std::floating_point T>
  constexpr T sin(T x) {
    if (std::is_constant_evaluated()) {
      x = static_cast<T>(fmod(x, 2 * std::numbers::pi));
      const double x2 = x * x;
      double ex = x;
      double fac = 1.0;
      double r = 0.0;
      for (int i = 0; i < 16; ++i)
      {
        r += ex / fac;
        ex *= -x2;
        fac *= 2 * (i + 1) * ((i + 1) * 2 + 1);
      }
      return static_cast<T>(r);
    }
    else
    {
      return static_cast<T>(std::sin(x));
    }
  }

  template<std::floating_point T>
  constexpr T cos(T x) {
    if (std::is_constant_evaluated()) {
      x = static_cast<T>(fmod(x, 2 * std::numbers::pi));
      const auto x2 = x * x;
      double fac = 1.0;
      double ex = 1;
      double r = 0.0;
      for (int i = 0; i < 16; ++i)
      {
        r += ex / fac;
        ex *= -x2;
        fac *= ((i) * 2 + 1) * ((i) * 2 + 2);
      }
      return static_cast<T>(r);
    }
    else
    {
      return std::cos(x);
    }
  }
  template<std::floating_point T>
  constexpr T tan(T x) {
    if (std::is_constant_evaluated()) {
      return sin(x) / cos(x);
    }
    else
    {
      return std::tan(x);
    }
  }

  template<std::floating_point T>
  constexpr T asin(T x) {
    if (std::is_constant_evaluated()) {
      double a0 = 1.5707288;
      double a1 = -0.2121144;
      double a2 = 0.0742610;
      double a3 = -0.0187293;
      return std::numbers::pi / 2 - sqrt(1 - x) * (a0 + a1 * x + a2 * x * x + a3 * x * x * x);
    }
    else
    {
      return std::asin(x);
    }
  }

  template<typename T>
  constexpr T sign(T t) {
    return T(t < 0 ? -1 : (t > 0 ? 1 : 0));
  }


  template<std::floating_point T>
  constexpr T acos(T x) {
    if (std::is_constant_evaluated()) {
      return std::numbers::pi / 2 - asin(x);
    }
    else
    {
      return std::acos(x);
    }
  }

  template<std::floating_point T>
  constexpr T atan(T x) {
    if (std::is_constant_evaluated()) {
      x = fmod(x, 2 * std::numbers::pi);
      const double x2 = x * x;
      double ex = x;
      double div = 1;
      double r = 0;
      for (int i = 0; ; ++i)
      {
        const auto add = ex / div;
        if (abs(add) < 1e-5f)
          break;

        r += add;

        ex *= -x2;
        div += 2;
      }
      return r;
    }
    else
    {
      return std::acos(x);
    }
  }

  template<std::floating_point T>
  constexpr T atan2(T y, T x) {
    //http://pubs.opengroup.org/onlinepubs/009695399/functions/atan2.html
    //Volkan SALMA

    if (std::is_constant_evaluated()) {
      constexpr double pi_fourth = std::numbers::pi / 4.0;
      constexpr double three_pi_fourth = 3.0 * std::numbers::pi / 4.0;
      double r, angle;
      double abs_y = abs(y) + 1e-10f;      // kludge to prevent 0/0 condition
      if (x < 0.0f)
      {
        r = (x + abs_y) / (abs_y - x);
        angle = three_pi_fourth;
      }
      else
      {
        r = (x - abs_y) / (x + abs_y);
        angle = pi_fourth;
      }
      angle += (0.1963f * r * r - 0.9817f) * r;
      if (y < 0.0f)
        return(-angle);     // negate if in quad III or IV
      else
        return(angle);
    }
    else
    {
      return std::atan2(y, x);
    }
  }

  template<typename T, typename H>
  constexpr std::common_type_t<std::decay_t<T>, std::decay_t<H>> max(T a, H b) {
    using Ty = std::common_type_t<std::decay_t<T>, std::decay_t<H>>;
    return std::max(Ty(a), Ty(b));
  }

  template<typename T, typename H>
  constexpr std::common_type_t<std::decay_t<T>, std::decay_t<H>> min(T a, H b) {
    using Ty = std::common_type_t<std::decay_t<T>, std::decay_t<H>>;
    return std::min(Ty(a), Ty(b));
  }

  template<typename T, typename R, typename S>
  constexpr auto clamp(T t, R min, S max) requires requires(T t, R r, S s) {
    { cx::min(max, cx::max(min, t)) };
  }
  {
    return cx::min(max, cx::max(min, t));
  }

  template<typename T, typename H> requires requires(T t, H h) { { t < h }; }
  constexpr T step(T value, H threshold)
  {
    return value < threshold ? T(0) : T(1);
  }

  template<typename E0, typename E1, typename T> requires requires(E0 e0, E1 e1, T t) { { clamp<T>((t - e0) / (e1 - e0), 0.0, 1.0) }; }
  constexpr T smoothstep(E0 edge0, E1 edge1, T x) {
    // Scale, bias and saturate x to 0..1 range
    x = clamp<T>(T((x - edge0) / (edge1 - edge0)), T(0.0), T(1.0));
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
  }

  template<typename T> requires requires(T t) { {t / T(180) * std::numbers::pi_v<double>}; }
  [[nodiscard]] constexpr auto radians(T deg) noexcept {
    return to_float_t<T>(deg / to_float_t<T>(180) * std::numbers::pi_v<double>);
  }
  template<typename T> requires requires(T t) { { t * T(180)* std::numbers::inv_pi_v<double> }; }
  [[nodiscard]] constexpr auto degrees(T rad) noexcept {
    return to_float_t<T>(rad * to_float_t<T>(180) * std::numbers::inv_pi_v<double>);
  }

  template<typename T, std::floating_point Alpha>
  constexpr T mix(T a, T b, Alpha t)
  {
    return (1 - t) * a + t * b;
  }
}
#endif //RNU_MATH_CX_FUN_HPP