#pragma once

#include <rnu/math/math.hpp>
#include <cmath>
#include <limits>
#include <span>
#include <vector>
#include <optional>

namespace rnu
{
  struct skyline_pack_area
  {
    int width;
    int height;
  };

  class skyline_packer
  {
  public:
    /// <summary>
    /// Packs the rectangles in a skyline-algorithm. Mutates the rectangles in the given span, but only their position.
    /// </summary>
    /// <param name="rectangles">A range of rectangles to pack. Only the height is important, the position will be overwritten.</param>
    /// <param name="width">The maximum width of the available packing space.</param>
    /// <returns>The required height to pack all rectangles.</returns>
    [[nodiscard]] std::optional<skyline_pack_area> pack(std::span<rnu::rect2f> rectangles, int max_width);

  private:
    struct node
    {
      int x;
      int y;
      int width;
    };

    std::vector<node> _nodes;
  };
}