#pragma once

namespace rnu {
template<typename T, size_t S>
constexpr T dot(const vec<T, S>& a, const vec<T, S>& b) noexcept
{
    return detail::dot_impl(std::make_index_sequence<S>(), a.components, b.components);
}
template<typename T, size_t S>
constexpr auto length(const vec<T, S>& a) noexcept
{
    return std::sqrt<T>(dot<T, S>(a, a)).real();
}

template<typename T, size_t S>
constexpr auto normalize(const vec<T, S>& a) noexcept
{
    return detail::apply_for_each(a, [inv_len = T(1) / length(a)](const auto& val) { return val * inv_len; });
}

template<typename T>
constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept
{
    return vec<T, 3>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

template<size_t S>
constexpr bool any_of(const vec<bool, S>& v)
{
    return std::any_of(v.begin(), v.end(), [](bool b) { return b; });
}
template<size_t S>
constexpr bool all_of(const vec<bool, S>& v)
{
    return std::all_of(v.begin(), v.end(), [](bool b) { return b; });
}
template<size_t S>
constexpr bool none_of(const vec<bool, S>& v)
{
    return std::none_of(v.begin(), v.end(), [](bool b) { return b; });
}
template<typename T, size_t S>
constexpr auto min_element(const vec<T, S>& v)
{
    return std::min_element(v.begin(), v.end(), std::less<T>());
}
template<typename T, size_t S>
constexpr auto max_element(const vec<T, S>& v)
{
    return std::max_element(v.begin(), v.end(), std::less<T>());
}
template<typename T, size_t S>
constexpr auto minmax_element(const vec<T, S>& v)
{
    return std::minmax_element(v.begin(), v.end(), std::less<T>());
}
}    // namespace gfx