#pragma once

#include <rnu/math/math.hpp>

namespace rnu
{
  enum class cull_result
  {
    fully_outside,
    fully_inside,
    intersecting
  };

  template<typename T>
  static constexpr cull_result cull_aabb(rnu::mat4_t<T> const& view_projection, rnu::vec3_t<T> min, rnu::vec3_t<T> max)
  {
    rnu::vec3_t<T> const bounds[]{ min, max };
    cull_result result = cull_result::fully_inside;

    rnu::vec4_t<T> corners[8]{};
    for (std::uint32_t corner = 0; corner < 8; ++corner)
    {
      const std::uint32_t factor_x = (corner >> 0) & 1;
      const std::uint32_t factor_y = (corner >> 1) & 1;
      const std::uint32_t factor_z = (corner >> 2) & 1;
      corners[corner] = view_projection * rnu::vec4(
        bounds[factor_x].x,
        bounds[factor_y].y,
        bounds[factor_z].z,
        1);
    }

    for (int plane = 0; plane < 6; ++plane)
    {
      bool outside_plane = true;
      for (std::uint32_t corner = 0; corner < 8; ++corner)
      {
        const T sign = T(plane / 3) * 2.0 - 1.0;
        if (sign < 0)
          outside_plane = outside_plane && (corners[corner][plane % 3] > corners[corner].w);
        else
          outside_plane = outside_plane && (corners[corner][plane % 3] < -corners[corner].w);

        if (outside_plane)
          result = cull_result::intersecting;
      }

      if (outside_plane)
        return cull_result::fully_outside;
    }
    return result;
  }
}