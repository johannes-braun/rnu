#pragma once

namespace rnu {
namespace detail {
template<typename T, std::size_t... Is>
constexpr T dot_impl(std::index_sequence<Is...>, const T* s1, const T* s2) noexcept
{
    return ((s1[Is] * s2[Is]) + ...);
}
}    // namespace detail
}    // namespace rnu