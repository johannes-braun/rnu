#pragma once

namespace gfx
{
    template<size_t Maj, size_t Min>
    constexpr bool any_of(const mat<bool, Maj, Min>& v) noexcept
    {
        return std::any_of(v.begin(), v.end(), [](const auto& b) { return any_of(b); });
    }
    template<size_t Maj, size_t Min>
    constexpr bool all_of(const mat<bool, Maj, Min>& v) noexcept
    {
        return std::all_of(v.begin(), v.end(), [](const auto& b) { return all_of(b); });
    }
    template<size_t Maj, size_t Min>
    constexpr bool none_of(const mat<bool, Maj, Min>& v) noexcept
    {
        return std::none_of(v.begin(), v.end(), [](const auto& b) { return none_of(b); });
    }
    template<typename T, size_t Maj, size_t Min>
    constexpr auto min_element(const mat<T, Maj, Min>& m) noexcept
    {
        auto min = m.begin()->begin();
        for (auto it = m.begin(); it != m.end(); ++it)
            for (auto r = it->begin(); r != it->end(); ++r)
                if (*min > *r) min = r;
        return min;
    }
    template<typename T, size_t Maj, size_t Min>
    constexpr auto max_element(const mat<T, Maj, Min>& m) noexcept
    {
        auto max = m.begin()->begin();
        for (auto it = m.begin(); it != m.end(); ++it)
            for (auto r = it->begin(); r != it->end(); ++r)
                if (*max < *r) max = r;
        return max;
    }
    template<typename T, size_t Maj, size_t Min>
    constexpr auto minmax_element(const mat<T, Maj, Min>& m) noexcept
    {
        auto minmax = std::make_pair(m.begin()->begin(), m.begin()->begin());
        for (auto it = m.begin(); it != m.end(); ++it)
            for (auto r = it->begin(); r != it->end(); ++r)
            {
                if (*minmax.first > *r) minmax.first = r;
                if (*minmax.second < *r) minmax.second = r;
            }

        return minmax;
    }
}