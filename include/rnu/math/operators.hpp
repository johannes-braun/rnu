#pragma once

#ifndef RNU_MATH_OPERATORS_HPP
#define RNU_MATH_OPERATORS_HPP

namespace rnu
{
#define define_binary_const_callable(Name, Op) \
    template<typename Lhs, typename Rhs = Lhs> requires requires{ {std::declval<Lhs>() Op std::declval<Rhs>()}; } \
    struct Name { \
        [[nodiscard]] constexpr auto operator()(Lhs&& lhs, Rhs && rhs) const noexcept(noexcept(std::declval<Lhs>() Op std::declval<Rhs>())) \
        { return lhs Op rhs; } \
    }; \
    template<typename Lhs, typename Rhs = Lhs> requires requires(Lhs lhs, Rhs rhs){ {lhs Op rhs}; } \
    constexpr auto call_##Name(Lhs && lhs, Rhs && rhs) { \
      return Name<Lhs, Rhs>{}(lhs, rhs); \
    } 

#define define_binary_callable(Name, Op) \
    define_binary_const_callable(Name, Op) \
    template<typename Lhs, typename Rhs = Lhs> requires requires(Lhs lhs, Rhs rhs){ {lhs Op rhs}; } \
    constexpr decltype(auto) call_##Name##_assign(Lhs& lhs, Rhs && rhs) { \
      return lhs = Name<Lhs, Rhs>{}(lhs, rhs); \
    }

#define define_unary_const_callable(Name, Op) \
    template<typename Lhs> requires requires{ {Op std::declval<Lhs>()}; } \
    struct Name { \
        [[nodiscard]] constexpr auto operator()(Lhs && lhs) const noexcept(noexcept(Op std::declval<Lhs>())) \
        { return Op lhs; } \
    }; \
    template<typename Lhs> requires requires(Lhs lhs){ {Op lhs}; } \
    constexpr auto call_##Name(Lhs && lhs) { \
      return Name<Lhs>{}(lhs); \
    }

    define_binary_callable(plus, +);
    define_binary_callable(minus, -);
    define_binary_callable(multiplies, *);
    define_binary_callable(divides, /);
    define_binary_callable(modulus, %);
    define_unary_const_callable(negate, -);
    define_binary_const_callable(equal_to, ==);
    define_binary_const_callable(not_equal_to, !=);
    define_binary_const_callable(greater, >);
    define_binary_const_callable(less, <);
    define_binary_const_callable(greater_equal, >=);
    define_binary_const_callable(less_equal, <=);
    define_binary_callable(bit_shl, <<);
    define_binary_callable(bit_shr, >>);
    define_binary_callable(bit_and, &);
    define_binary_callable(bit_or, |);
    define_binary_callable(bit_xor, ^);
    define_unary_const_callable(bit_not, ~);
    define_binary_const_callable(logical_and, &&);
    define_binary_const_callable(logical_or, ||);
    define_unary_const_callable(logical_not, !);

#undef define_binary_const_callable
#undef define_unary_const_callable
}
#include "mat_op.inl.hpp"

#endif // RNU_MATH_OPERATORS_HPP