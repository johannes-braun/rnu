#pragma once

#include <algorithm>

namespace rnu {
    template<floating_point_vector V>
    [[nodiscard]] constexpr auto dot(V a, V b) noexcept
    {
        return detail::dot_impl(std::make_index_sequence < V{}.size() > (), a.components, b.components);
    }
    template<floating_point_vector V>
    [[nodiscard]] constexpr auto norm(V a) noexcept
    {
        return sqrt(dot(a, a));
    }
    template<floating_point_vector V>
    [[nodiscard]] constexpr auto normalize(V a) noexcept
    {
        return detail::apply_for_each(a, [inv_len = (1) / norm(a)](const auto& val) { return val * inv_len; });
    }
    template<typename T>
    [[nodiscard]] constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept
    {
        return vec<T, 3>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    template<size_t S>
    [[nodiscard]] constexpr bool any_of(const vec<bool, S>& v)
    {
        return std::any_of(v.begin(), v.end(), [](bool b) { return b; });
    }
    template<size_t S>
    [[nodiscard]] constexpr bool all_of(const vec<bool, S>& v)
    {
        return std::all_of(v.begin(), v.end(), [](bool b) { return b; });
    }
    template<size_t S>
    [[nodiscard]] constexpr bool none_of(const vec<bool, S>& v)
    {
        return std::none_of(v.begin(), v.end(), [](bool b) { return b; });
    }
}    // namespace gfx