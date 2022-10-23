#pragma once

#include <bit>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <concepts>

namespace rnu
{
  template<size_t RequiredSize>
  using sized_int_t = std::conditional_t<RequiredSize <= sizeof(uint8_t), uint8_t,
    std::conditional_t<RequiredSize <= sizeof(uint16_t), uint16_t,
    std::conditional_t<RequiredSize <= sizeof(uint32_t), uint32_t,
    uint64_t>>>;

  template<bool Signed, std::size_t Exponent, std::size_t Mantissa, std::floating_point T>
  [[nodiscard]] constexpr auto small_float(T x) noexcept
  {
    using result_type = sized_int_t<Exponent + Mantissa + size_t(Signed)>;
    using source_int_type = sized_int_t<sizeof(T)>;

    static_assert(std::numeric_limits<T>::is_iec559);
    constexpr auto one = source_int_type(1);

    constexpr auto input_size = sizeof(T) * std::numeric_limits<std::underlying_type_t<std::byte>>::digits;
    constexpr auto input_mantissa = std::numeric_limits<T>::digits;

    constexpr auto input_exponent = input_size - input_mantissa - one;
    constexpr auto input_exponent_mask = ((one << (input_exponent + one)) - one) << (input_mantissa - one);
    constexpr auto exponent_difference = input_exponent - Exponent + 1;

    constexpr auto base_shift = input_mantissa - Mantissa - one;
    constexpr auto exponent_mask = ((one << Exponent) - one) << Mantissa;
    constexpr auto mantissa_mask = ((one << Mantissa) - one);
    constexpr auto sign_shift = input_size - one - (Exponent + Mantissa);

    constexpr auto exponent_difference_mask = ((one << exponent_difference) - one) << (input_size - one - one - exponent_difference);

    auto const f = std::bit_cast<source_int_type>(x);
    auto const packed = ((((f & input_exponent_mask) - exponent_difference_mask) >> base_shift) & exponent_mask) |
      ((f >> base_shift) & mantissa_mask);

    if constexpr (Signed)
      return result_type(packed | ((f & (one << (input_size - one))) >> sign_shift));
    else
      return result_type(packed);
  }

  template<std::floating_point T>
  [[nodiscard]] constexpr auto to_float10(T f) noexcept
  {
    return small_float<false, 5, 5>(f);
  }

  template<std::floating_point T>
  [[nodiscard]] constexpr auto to_float11(T f) noexcept
  {
    return small_float<false, 5, 6>(f);
  }

  template<std::floating_point T>
  [[nodiscard]] constexpr auto to_half(T f) noexcept
  {
    return small_float<true, 5, 10>(f);
  }

  template<std::floating_point R, std::floating_point G, std::floating_point B>
  [[nodiscard]] constexpr std::uint32_t to_r11g11b10(R r, G g, B b) noexcept
  {
    return static_cast<std::uint32_t>(
      ((to_float11(r) & ((1 << 11) - 1)) << 0) |
      ((to_float11(g) & ((1 << 11) - 1)) << 11) |
      ((to_float10(b) & ((1 << 10) - 1)) << 22));
  }
}