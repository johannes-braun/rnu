#pragma once

#include <concepts>
#include <rnu/math/math.hpp>
#include <rnu/math/cx_fun.hpp>

namespace rnu {
  template<typename T>
  struct smooth
  {
    static constexpr T finished_epsilon = 1e-3;

    [[nodiscard]] constexpr smooth() noexcept : smooth(T{}) {}

    [[nodiscard]] constexpr smooth(T val) noexcept : _val(val), _dst(val) {}

    constexpr smooth& to(T dst) noexcept
    { 
      _dst = dst;
      return *this; 
    }

    constexpr void update(double delta_s) noexcept
    {
      _val += (_dst - _val) * delta_s;
    }

    [[nodiscard]] constexpr bool finished() const noexcept
    {
      return rnu::abs(_dst - _val) < finished_epsilon;
    }

    [[nodiscard]] constexpr T& value() noexcept
    {
      return _val;
    }

    [[nodiscard]] constexpr T const& value() const noexcept
    {
      return _val;
    }

  private:
    T _val;
    T _dst;
  };
}