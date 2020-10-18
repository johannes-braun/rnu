#pragma once

#include "mat_type.hpp"

namespace rnu
{
    template<size_t C, size_t R, typename Fun, size_t... Cs, size_t ... Rs>
    void apply_each_impl(Fun&& fun, std::index_sequence<Cs...>, std::index_sequence<Rs...>)
    {
        size_t col; ((col = Cs, (fun(col, Rs), ...)), ...);
    }

    template<size_t C, size_t R, typename Fun>
    void apply_each(Fun&& fun)
    {
        return apply_each_impl<C, R>(std::forward<Fun>(fun), std::make_index_sequence<C>(), std::make_index_sequence<R>());
    }
}