#pragma once

#include "vec_type.hpp"
#include "mat_type.hpp"
#include "quat_type.hpp"

namespace rnu
{
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

    template<typename... QuaternionTypes, typename NAryFun>
    requires (quaternion_type<std::decay_t<QuaternionTypes>> && ...) &&
        requires(reference_type<QuaternionTypes>... values, NAryFun&& fun) { fun(values...); }
    constexpr auto element_wise(NAryFun&& fun, QuaternionTypes&&... quats)
    {
        using return_value_type = std::invoke_result_t<NAryFun, reference_type<QuaternionTypes>...>;
        if constexpr (std::is_same_v<return_value_type, void>)
        {
            fun(quats.at(0)...);
            fun(quats.at(1)...);
            fun(quats.at(2)...);
            fun(quats.at(3)...);
        }
        else
        {
            return quat_t<return_value_type>{
                fun(quats.at(0)...),
                    fun(quats.at(1)...),
                    fun(quats.at(2)...),
                    fun(quats.at(3)...)
            };
        }
    }


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

    namespace detail
    {
        template<typename FunctorType, typename... VectorTypes, size_t... Component>
        void element_wise_void(FunctorType&& functor, VectorTypes&&... vectors, std::index_sequence<Component...>)
        {
            size_t c = 0;
            ((c = Component, (functor(vectors[c]...))), ...);
        }
        template<typename ResultType, typename FunctorType, typename... VectorTypes, size_t... Component>
        auto element_wise_vtype(FunctorType&& functor, VectorTypes&&... vectors, std::index_sequence<Component...>)
        {
            using result_t = vec<ResultType, head_t<VectorTypes...>::component_count>;
            size_t c = 0;
            return result_t{ (c = Component, (functor(vectors[c]...)))... };
        }
    }

    template<typename FunctorType, typename... VectorTypes>
    auto element_wise(FunctorType&& functor, VectorTypes&&... vectors) requires requires(reference_type<VectorTypes>... values, FunctorType&& fun) { fun(values...); }
    {
        using return_value_type = std::invoke_result_t<FunctorType, reference_type<VectorTypes>...>;
        if constexpr (std::is_same_v<return_value_type, void>)
            detail::element_wise_void<FunctorType, VectorTypes...>(std::forward<FunctorType>(functor), std::forward<VectorTypes>(vectors)..., std::make_index_sequence<head_t<VectorTypes...>::component_count>());
        else
            return detail::element_wise_vtype<return_value_type, FunctorType, VectorTypes...>(std::forward<FunctorType>(functor), std::forward<VectorTypes>(vectors)..., std::make_index_sequence<head_t<VectorTypes...>::component_count>());
    }

    template<typename... MatrixTypes, typename NAryFun>
    requires (matrix_type<std::decay_t<MatrixTypes>> && ...) && dimensionally_equal<MatrixTypes...>&&
        requires(reference_type<MatrixTypes>... values, NAryFun fun) { fun(values...); }
    constexpr auto element_wise(NAryFun&& fun, MatrixTypes&&... matrices) {

        using return_value_type = std::invoke_result_t<NAryFun, reference_type<MatrixTypes>...>;
        if constexpr (std::is_same_v<return_value_type, void>)
        {
            constexpr auto make_result = []<size_t... i>(MatrixTypes&&... matrices, auto && fun, std::index_sequence<i...>) {
                constexpr auto set_at = [](size_t index, MatrixTypes&&... matrices, auto&& fun) { fun(matrices.element(index)...); };
                (set_at(i, std::forward<MatrixTypes>(matrices)..., fun), ...);
            };

            make_result(std::forward<MatrixTypes>(matrices)..., std::forward<NAryFun>(fun),
                std::make_index_sequence<head_t<MatrixTypes...>::cols* head_t<MatrixTypes...>::rows>());
        }
        else
        {
            constexpr auto make_result = []<size_t... i>(MatrixTypes&&... matrices, auto && fun, std::index_sequence<i...>) {
                constexpr auto set_at = [](size_t index, MatrixTypes&&... matrices, auto&& fun) { return fun(matrices.element(index)...); };
                return mat<return_value_type, head_t<MatrixTypes...>::cols, head_t<MatrixTypes...>::rows>(
                    set_at(i, std::forward<MatrixTypes>(matrices)..., fun)...
                    );
            };

            return make_result(std::forward<MatrixTypes>(matrices)..., std::forward<NAryFun>(fun),
                std::make_index_sequence<head_t<MatrixTypes...>::cols* head_t<MatrixTypes...>::rows>());
        }
    }

    template<size_t C, size_t R, typename Fun, size_t... Cs, size_t ... Rs>
    constexpr void index_wise_impl(Fun&& fun, std::index_sequence<Cs...>, std::index_sequence<Rs...>)
    {
        size_t col; ((col = Cs, (fun(col, Rs), ...)), ...);
    }

    template<size_t C, size_t R, typename Fun>
    constexpr void index_wise(Fun&& fun)
    {
        return index_wise_impl<C, R>(std::forward<Fun>(fun), std::make_index_sequence<C>(), std::make_index_sequence<R>());
    }

#define define_binary_const_callable(Name, Op) \
    template<typename Lhs, typename Rhs = Lhs> requires requires{ {std::declval<Lhs>() Op std::declval<Rhs>()}; } \
    struct Name { \
        [[nodiscard]] constexpr auto operator()(Lhs const& lhs, Rhs const& rhs) const noexcept(noexcept(std::declval<Lhs>() Op std::declval<Rhs>())) \
        { return lhs Op rhs; } \
    }; \
    template<typename Lhs, typename Rhs = Lhs> requires requires(Lhs lhs, Rhs rhs){ {lhs Op rhs}; } \
    constexpr auto call_##Name(Lhs const& lhs, Rhs const& rhs) { \
      return Name<Lhs, Rhs>{}(lhs, rhs); \
    } 

#define define_binary_callable(Name, Op) \
    define_binary_const_callable(Name, Op) \
    template<typename Lhs, typename Rhs = Lhs> requires requires(Lhs lhs, Rhs rhs){ {lhs Op rhs}; } \
    constexpr decltype(auto) call_##Name##_assign(Lhs& lhs, Rhs const& rhs) { \
      return lhs = Name<Lhs, Rhs>{}(lhs, rhs); \
    }

#define define_unary_const_callable(Name, Op) \
    template<typename Lhs> requires requires{ {Op std::declval<Lhs>()}; } \
    struct Name { \
        [[nodiscard]] constexpr auto operator()(Lhs const& lhs) const noexcept(noexcept(Op std::declval<Lhs>())) \
        { return Op lhs; } \
    }; \
    template<typename Lhs> requires requires(Lhs lhs){ {Op lhs}; } \
    constexpr auto call_##Name(Lhs const& lhs) { \
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