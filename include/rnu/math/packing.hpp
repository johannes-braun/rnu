#pragma once

#include <bit>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <concepts>

namespace rnu
{
  template<typename T>
  using int_t = std::conditional_t<sizeof(T) == sizeof(uint8_t), uint8_t,
    std::conditional_t<sizeof(T) == sizeof(uint16_t), uint16_t,
    std::conditional_t<sizeof(T) == sizeof(uint32_t), uint32_t,
    uint64_t>>>;

  template<bool Signed, std::size_t Exponent, std::size_t Mantissa, std::floating_point T>
  constexpr int_t<T> small_float32(T x)
  {
    static_assert(std::numeric_limits<T>::is_iec559);

    constexpr auto input_size = sizeof(T) * std::numeric_limits<std::underlying_type_t<std::byte>>::digits;
    constexpr auto input_mantissa = std::numeric_limits<T>::digits;

    constexpr auto input_exponent = input_size - input_mantissa - 1;
    constexpr auto input_exponent_mask = ((1u << (input_exponent + 1)) - 1) << (input_mantissa - 1);
    constexpr auto exponent_difference = input_exponent - Exponent + 1;

    constexpr auto base_shift = input_mantissa - Mantissa - 1;
    constexpr auto exponent_mask = ((1u << Exponent) - 1u) << Mantissa;
    constexpr auto mantissa_mask = ((1u << Mantissa) - 1u);
    constexpr auto sign_shift = input_size - 1 - (Exponent + Mantissa);

    constexpr auto exponent_difference_mask = ((1u << exponent_difference) - 1u) << (input_size - 2u - exponent_difference);

    auto const f = std::bit_cast<int_t<T>>(x);
    auto const packed = ((((f & input_exponent_mask) - exponent_difference_mask) >> base_shift) & exponent_mask) |
      ((f >> base_shift) & mantissa_mask);

    if constexpr (Signed)
      return packed | ((f & (1u << (input_size - 1))) >> sign_shift);
    else
      return packed;
  }

  constexpr std::uint32_t to_float10(float f)
  {
    return small_float32<false, 5, 5>(f);
  }
  constexpr std::uint32_t to_float11(float f)
  {
    return small_float32<false, 5, 6>(f);
  }
  constexpr std::uint32_t to_half(float f)
  {
    return small_float32<true, 5, 10>(f);
  }

  constexpr std::uint32_t to_r11g11b10(float r, float g, float b)
  {
    return
      ((to_float11(r) & ((1 << 11) - 1)) << 0) |
      ((to_float11(g) & ((1 << 11) - 1)) << 11) |
      ((to_float10(b) & ((1 << 10) - 1)) << 22);
  }
}