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
    };
#define define_unary_const_callable(Name, Op) \
    template<typename Lhs> requires requires{ {Op std::declval<Lhs>()}; } \
    struct Name { \
        [[nodiscard]] constexpr auto operator()(Lhs const& lhs) const noexcept(noexcept(Op std::declval<Lhs>())) \
        { return Op lhs; } \
    };

    define_binary_const_callable(plus, +);
    define_binary_const_callable(minus, -);
    define_binary_const_callable(multiplies, *);
    define_binary_const_callable(divides, /);
    define_binary_const_callable(modulus, %);
    define_unary_const_callable(negate, -);
    define_binary_const_callable(equal_to, ==);
    define_binary_const_callable(not_equal_to, !=);
    define_binary_const_callable(greater, >);
    define_binary_const_callable(less, <);
    define_binary_const_callable(greater_equal, >=);
    define_binary_const_callable(less_equal, <=);
    define_binary_const_callable(bit_shl, <<);
    define_binary_const_callable(bit_shr, >>);
    define_binary_const_callable(bit_and, &);
    define_binary_const_callable(bit_or, |);
    define_binary_const_callable(bit_xor, ^);
    define_unary_const_callable(bit_not, ~);
    define_binary_const_callable(logical_and, &&);
    define_binary_const_callable(logical_or, ||);
    define_unary_const_callable(logical_not, !);
#undef define_binary_const_callable
#undef define_unary_const_callable

    template<typename Lhs, typename Rhs> concept vector_and_scalar = vector_type<Lhs> && scalar_type<Rhs>;
    template<typename Lhs, typename Rhs> concept scalar_and_vector = scalar_type<Lhs> && vector_type<Rhs>;
    template<typename Lhs, typename Rhs> concept vector_and_vector = vector_type<Lhs> && vector_type<Rhs>;
    template<typename Lhs, typename Rhs> concept same_or_any_scalar =
        vector_and_vector<Lhs, Rhs> || vector_and_scalar<Lhs, Rhs> || scalar_and_vector<Lhs, Rhs>;
    template<typename Lhs, typename Rhs, template<typename...> typename Fun> concept callable_exists = same_or_any_scalar<Lhs, Rhs> &&
        requires{ Fun<scalar_type_of_t<Lhs>, scalar_type_of_t<Rhs>>{}; };
    template<typename Lhs, template<typename...> typename Fun> concept callable_exists_unary = vector_type<Lhs> &&
        requires{ Fun<scalar_type_of_t<Lhs>>{}; };
    template<typename Lhs, typename Rhs, template<typename...> typename Fun> concept assignment_callable_exists = (vector_and_scalar<Lhs, Rhs> || vector_and_vector<Lhs, Rhs>) &&
        requires{ Fun<scalar_type_of_t<Lhs>, scalar_type_of_t<Rhs>>{}; };

    template<template<typename...> typename Fun, typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, Fun>
    constexpr auto operator_fun(Lhs const& lhs, Rhs const& rhs) noexcept
    {
        const Fun<scalar_type_of_t<Lhs>, scalar_type_of_t<Rhs>> fun{};
        if constexpr (vector_and_vector<Lhs, Rhs>)
            return element_wise([&](auto aa, auto bb) {return fun(aa, bb); }, lhs, rhs);
        else if constexpr (vector_and_scalar<Lhs, Rhs>)
            return element_wise([&](auto aa) {return fun(aa, rhs); }, lhs);
        else if constexpr (scalar_and_vector<Lhs, Rhs>)
            return element_wise([&](auto bb) {return fun(lhs, bb); }, rhs);
        else
            static_assert(false, "Detected missing case!");
    }
    template<template<typename...> typename Fun, typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, plus>
    constexpr Lhs& operator_assign_fun(Lhs& lhs, Rhs const& rhs) noexcept
    {
        lhs = operator_fun<Fun>(lhs, rhs);
        return lhs;
    }
    template<template<typename...> typename Fun, typename Lhs> requires callable_exists_unary<Lhs, Fun>
    constexpr auto const_operator_fun(Lhs const& lhs) noexcept
    {
        const Fun<scalar_type_of_t<Lhs>> fun{};
        return element_wise([&](auto aa) {return fun(aa); }, lhs);
    }

    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, plus>
    [[nodiscard]] constexpr auto operator+(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<plus>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, minus>
    [[nodiscard]] constexpr auto operator-(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<minus>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, multiplies>
    [[nodiscard]] constexpr auto operator*(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<multiplies>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, divides>
    [[nodiscard]] constexpr auto operator/(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<divides>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, modulus>
    [[nodiscard]] constexpr auto operator%(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<modulus>(lhs, rhs); }
    template<typename Lhs> requires callable_exists_unary<Lhs, negate>
    [[nodiscard]] constexpr auto operator-(Lhs const& lhs) noexcept { return const_operator_fun<negate>(lhs); }

    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, logical_and>
    [[nodiscard]] constexpr auto operator&&(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<logical_and>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, logical_or>
    [[nodiscard]] constexpr auto operator||(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<logical_or>(lhs, rhs); }
    template<typename Lhs> requires callable_exists_unary<Lhs, logical_not>
    [[nodiscard]] constexpr auto operator!(Lhs const& lhs) noexcept { return const_operator_fun<logical_not>(lhs); }

    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, bit_and>
    [[nodiscard]] constexpr auto operator&(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<bit_and>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, bit_or>
    [[nodiscard]] constexpr auto operator|(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<bit_or>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, bit_xor>
    [[nodiscard]] constexpr auto operator^(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<bit_xor>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, bit_shl>
    [[nodiscard]] constexpr auto operator<<(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<bit_shl>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, bit_shr>
    [[nodiscard]] constexpr auto operator>>(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<bit_shr>(lhs, rhs); }
    template<typename Lhs> requires callable_exists_unary<Lhs, bit_not>
    [[nodiscard]] constexpr auto operator~(Lhs const& lhs) noexcept { return const_operator_fun<bit_not>(lhs); }

    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, equal_to>
    [[nodiscard]] constexpr auto operator==(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<equal_to>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, not_equal_to>
    [[nodiscard]] constexpr auto operator!=(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<not_equal_to>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, greater>
    [[nodiscard]] constexpr auto operator>(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<greater>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, less>
    [[nodiscard]] constexpr auto operator<(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<less>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, greater_equal>
    [[nodiscard]] constexpr auto operator>=(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<greater_equal>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires callable_exists<Lhs, Rhs, less_equal>
    [[nodiscard]] constexpr auto operator<=(Lhs const& lhs, Rhs const& rhs) noexcept { return operator_fun<less_equal>(lhs, rhs); }

    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, plus>
    constexpr Lhs& operator+=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<plus>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, minus>
    constexpr Lhs& operator-=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<minus>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, multiplies>
    constexpr Lhs& operator*=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<multiplies>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, divides>
    constexpr Lhs& operator/=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<divides>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, modulus>
    constexpr Lhs& operator%=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<modulus>(lhs, rhs); }
    
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, bit_and>
    constexpr Lhs& operator&=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<bit_and>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, bit_or>
    constexpr Lhs& operator|=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<bit_or>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, bit_xor>
    constexpr Lhs& operator^=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<bit_xor>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, bit_shl>
    constexpr Lhs& operator<<=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<bit_shl>(lhs, rhs); }
    template<typename Lhs, typename Rhs> requires assignment_callable_exists<Lhs, Rhs, bit_shr>
    constexpr Lhs& operator>>=(Lhs& lhs, Rhs const& rhs) noexcept { return operator_assign_fun<bit_shr>(lhs, rhs); }

    template<vector_type V> requires requires(typename V::value_type v) { {++v}; }
    constexpr V& operator++(V& v) noexcept { element_wise([=](auto& vv) {return ++vv; }, v); return v; }
    template<vector_type V> requires requires(typename V::value_type v) { {v++}; }
    [[nodiscard]] constexpr V operator++(V& v, int) noexcept { V result = v; element_wise([=](auto& vv) {return vv++; }, v); return result; }
    template<vector_type V> requires requires(typename V::value_type v) { {--v}; }
    constexpr V& operator--(V& v) noexcept { element_wise([=](auto& vv) {return --vv; }, v); return v; }
    template<vector_type V> requires requires(typename V::value_type v) { {v--}; }
    [[nodiscard]] constexpr V operator--(V& v, int) noexcept { V result = v; element_wise([=](auto& vv) {return vv--; }, v); return result; }
}
#include "mat_op.inl.hpp"