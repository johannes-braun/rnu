#pragma once
#include "traits.hpp"
#include "vec_apply.hpp"

namespace rnu
{
    template<vector_type V> requires requires(typename V::value_type v) { {++v}; }
    constexpr V& operator++(V& v) noexcept {
        return detail::apply_for_each(v, [=](auto vv) {return ++vv; });
    }
    template<vector_type V> requires requires(typename V::value_type v) { {v++}; }
    constexpr V operator++(V& v, int) noexcept {
        V result = v;
        detail::apply_for_each(v, [=](auto vv) {return vv++; });
        return result;
    }
    template<vector_type V> requires requires(typename V::value_type v) { {--v}; }
    constexpr V& operator--(V& v) noexcept {
        return detail::apply_for_each(v, [=](auto vv) {return --vv; });
    }
    template<vector_type V> requires requires(typename V::value_type v) { {v--}; }
    constexpr V operator--(V& v, int) noexcept {
        V result = v;
        detail::apply_for_each(v, [=](auto vv) {return vv--; });
        return result;
    }
    template<vector_type V> requires requires(typename V::value_type v) { {!v}; }
    [[nodiscard]] constexpr auto operator!(const V& v) noexcept {
        return detail::apply_for_each(v, [=](auto vv) {return !vv; });
    }
    template<vector_type V> requires requires(typename V::value_type v) { {~v}; }
    [[nodiscard]] constexpr auto operator~(const V& v) noexcept {
        return detail::apply_for_each(v, [=](auto vv) {return ~vv; });
    }
    template<vector_type V> requires requires(typename V::value_type v) { {-v}; }
    [[nodiscard]] constexpr auto operator-(const V& v) noexcept {
        return detail::apply_for_each(v, [=](auto vv) {return -vv; });
    }
    template<vector_type V> requires requires(typename V::value_type v) { {+v}; }
    [[nodiscard]] constexpr auto operator+(const V& v) noexcept {
        return detail::apply_for_each(v, [=](auto vv) {return +vv; });
    }

#define make_assign_operator(Op, CN) \
    template<vector_type A, vector_type B> requires requires(typename A::value_type a, typename B::value_type b) { {a Op b}; }\
    constexpr decltype(auto) operator Op(A CN& a, const B b) noexcept { return detail::apply_for_each(a, b, [](auto aa, auto bb) {return aa Op bb; }); }\
    template<vector_type A, scalar_type B> requires requires(typename A::scalar_type a, B b) { {a Op b}; }\
    constexpr decltype(auto) operator Op(A CN& a, const B b) noexcept { return detail::apply_for_each(a, [=](auto aa) {return aa Op b; }); }\
    template<vector_type B, scalar_type A> requires requires(typename B::scalar_type b, A a) { {a Op b}; }\
    constexpr decltype(auto) operator Op(A CN& a, const B b) noexcept { return detail::apply_for_each(b, [=](auto bb) {return a Op bb; }); }
#define make_binary_operator(Op) \
    template<vector_type A, vector_type B> requires requires(typename A::value_type a, typename B::value_type b) { {a Op b}; }\
    [[nodiscard]] constexpr decltype(auto) operator Op(const A a, const B b) noexcept { return detail::apply_for_each(a, b, [](auto aa, auto bb) {return aa Op bb; }); }\
    template<vector_type A, scalar_type B> requires requires(typename A::scalar_type a, B b) { {a Op b}; }\
    [[nodiscard]] constexpr decltype(auto) operator Op(const A a, const B b) noexcept { return detail::apply_for_each(a, [=](auto aa) {return aa Op b; }); }\
    template<vector_type B, scalar_type A> requires requires(typename B::scalar_type b, A a) { {a Op b}; }\
    [[nodiscard]] constexpr decltype(auto) operator Op(const A a, const B b) noexcept { return detail::apply_for_each(b, [=](auto bb) {return a Op bb; }); }
#define make_operator(Op, CN) \
    make_binary_operator(Op) \
    make_assign_operator(Op##=, CN)

    make_operator(+, )
    make_operator(-, )
    make_operator(*, )
    make_operator(/ , )
    make_operator(%, )
    make_operator(>> , )
    make_operator(<< , )
    make_operator(&, )
    make_binary_operator(&&)
    make_operator(| , )
    make_binary_operator(|| )
    make_operator(^, )
    make_operator(< , const)
    make_operator(> , const)
    make_assign_operator(== , const)
    make_assign_operator(!= , const)
}