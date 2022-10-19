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

    constexpr auto operator==(box const& other) const
    {
      return position == other.position && size == other.size;
    }

    constexpr auto operator!=(box const& other) const
    {
      return position != other.position || size != other.size;
    }

    constexpr bool contains(rnu::vec<TPosition, Dimensions> point) const
    {
      return (point >= lower()).all() && (point <= upper()).all();
    }

    vec<position_type, dimensions> position;
    vec<size_type, dimensions> size;
  };
}