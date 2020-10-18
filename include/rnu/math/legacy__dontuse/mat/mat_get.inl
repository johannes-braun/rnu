#pragma once

namespace gfx
{
    template<typename T, size_t Maj, size_t Min>
    constexpr auto rows(const mat<T, Maj, Min>& matrix) noexcept
    {
        return Min;
    }
    template<typename T, size_t Maj, size_t Min>
    constexpr auto cols(const mat<T, Maj, Min>& matrix) noexcept
    {
        return Maj;
    }
    template<typename T, size_t S>
    constexpr auto rows(const vec<T, S>& vector) noexcept
    {
        return S;
    }
    template<typename T, size_t S>
    constexpr auto cols(const vec<T, S>& vector) noexcept
    {
        return 1;
    }
}