#pragma once

#include <rnu/math/vec.hpp>

namespace rnu
{
  template<std::size_t Dimensions, typename TPosition, typename TSize = decltype(std::declval<TPosition>() - std::declval<TPosition>())>
  struct box
  {
    static constexpr std::size_t dimensions = Dimensions;
    using position_type = TPosition;
    using size_type = TSize;

    constexpr auto upper() const {
      return position + size;
    }

    constexpr auto lower() const {
      return position;
    }

    vec<position_type, dimensions> position;
    vec<size_type, dimensions> size;
  };
}