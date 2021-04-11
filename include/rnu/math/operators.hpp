#pragma once

#include "vec_type.hpp"
#include "mat_type.hpp"
#include "quat_type.hpp"

namespace rnu
{
  namespace detail2 {
    template<typename... Ts>
    concept all_vectors_or_scalar = ((vector_type<std::decay_t<Ts>> || scalar_type<std::decay_t<Ts>>) && ...);

    template<typename... Ts>
    concept any_vector_type = all_vectors_or_scalar<Ts...> && (vector_type<std::decay_t<Ts>> || ...);
  }

    template<typename Type, typename... VectorTypes>
    concept dimensionally_equal = (vector_type<std::decay_t<Type>> &&
        (vector_type< std::decay_t<VectorTypes>> && ...) &&
        ((std::decay_t < Type>::component_count == std::decay_t < VectorTypes>::component_count) && ...)) ||
        (matrix_type< std::decay_t<Type>> &&
            (matrix_type< std::decay_t<VectorTypes>> && ...) &&
            ((std::decay_t < Type>::rows == std::decay_t < VectorTypes>::rows) && ...) &&
            ((std::decay_t<Type>::cols == std::decay_t < VectorTypes>::cols) && ...));

    template<typename Head, typename... Tail>
    using head_t = std::decay_t<Head>;

    template<typename T>
    using reference_type = std::conditional_t<std::is_const_v<std::remove_reference_t<T>>, typename std::decay_t<T>::const_reference, typename std::decay_t<T>::reference>;

    /// element_wise
    /// * Takes N vectors
    /// * The vector sizes are equal
    /// * Takes functor with N parameters
    /// * The value type of the i'th vector is convertible to the i'th parameter of the functor
    /// * The function never returns by-ref
    /// * If the functor has a return type, the function itself returns a vector with the common size and the return type of the function with const, volatile and references removed.
    /// * Otherwise the function returns void

    template<typename... VectorTypes>
    concept vector_types_compatible = (vector_type<std::decay_t<VectorTypes>> && ...) && dimensionally_equal<VectorTypes...>;

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