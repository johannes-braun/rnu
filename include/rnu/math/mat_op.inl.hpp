#pragma once

namespace rnu {
#define make_scalar_mat_ops(Op) \
    template<matrix_type M, scalar_type S> \
    requires requires(typename M::scalar_type a, S b) { {a Op b}; } \
    [[nodiscard]] constexpr auto operator Op(M const& a, S const& b) { \
        using result_value_type = \
            decltype(std::declval<typename M::scalar_type>() Op std::declval<S>()); \
        mat<result_value_type, M::cols, M::rows> result; \
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { result.at(c, r) = a.at(c, r) Op b; }); \
        return result; \
    } \
    template<matrix_type M, scalar_type S> \
    requires requires(S a, typename M::scalar_type b) { {a + b}; } \
    [[nodiscard]] constexpr auto operator Op(S const& a, M const& b) { \
        using result_value_type = \
            decltype(std::declval<S>() Op std::declval<typename M::scalar_type>()); \
        mat<result_value_type, M::cols, M::rows> result; \
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { result.at(c, r) = a Op b.at(c, r); }); \
        return result; \
    }

#define make_scalar_mat_assign_op(Op)\
    template<matrix_type M, scalar_type S>\
    requires requires(typename M::scalar_type a, S b) { {a Op##= b}; }\
    [[nodiscard]] constexpr decltype(auto) operator Op##=(M& a, S const& b) {\
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { a.at(c, r) Op##= b; });\
        return a;\
    }

#define make_both_scalar_mat_ops(Op) \
    make_scalar_mat_ops(Op)\
    make_scalar_mat_assign_op(Op)

#define make_mat_comparison_op(Op) \
    template<matrix_type A, matrix_type B> \
    requires (A::cols == B::cols && A::rows == B::rows) && requires(typename A::scalar_type a, typename B::scalar_type b) { {a Op b}; } \
    [[nodiscard]] constexpr auto operator Op(A const& a, B const& b) \
    { \
        using result_value_type = \
            decltype(std::declval<typename A::scalar_type>() Op std::declval<typename B::scalar_type>()); \
        mat<result_value_type, A::cols, A::rows> result; \
        index_wise<A::cols, A::rows>([&](size_t c, size_t r) { result.at(c, r) = a.at(c, r) Op b.at(c, r); }); \
        return result; \
    }
    make_mat_comparison_op(!= )
        make_mat_comparison_op(== )
        make_mat_comparison_op(<= )
        make_mat_comparison_op(>= )
        make_mat_comparison_op(< )
        make_mat_comparison_op(> )
        make_mat_comparison_op(&&)
        make_mat_comparison_op(||)

    make_both_scalar_mat_ops(+);
    make_both_scalar_mat_ops(-);
    make_both_scalar_mat_ops(*);
    make_both_scalar_mat_ops(/ );
    make_both_scalar_mat_ops(%);
    make_both_scalar_mat_ops(&);
    make_both_scalar_mat_ops(| );
    make_both_scalar_mat_ops(>>);
    make_both_scalar_mat_ops(<<);

    make_scalar_mat_ops(&&);
    make_scalar_mat_ops(|| );
    make_scalar_mat_ops(== );
    make_scalar_mat_ops(!= );
    make_scalar_mat_ops(<= );
    make_scalar_mat_ops(>= );
    make_scalar_mat_ops(< );
    make_scalar_mat_ops(> );

    template<matrix_type Lhs, matrix_type Rhs>
    requires (Lhs::cols == Rhs::rows)
        constexpr auto operator*(const Lhs& a, const Rhs& b) noexcept
    {
        using result_scalar_type = decltype(typename Lhs::scalar_type{} *typename Rhs::scalar_type{});

        mat<result_scalar_type, Rhs::cols, Lhs::rows> result;
        index_wise<Rhs::cols, Lhs::rows>([&](size_t c, size_t r) {
            result.at(c, r) = 0;
            for (size_t i = 0; i < Lhs::cols; ++i) { result.at(c, r) += a.at(i, r) * b.at(c, i); }
            });
        return result;
    }
    template<matrix_type Lhs, matrix_type Rhs>
    requires (Lhs::cols == Rhs::rows)
        constexpr decltype(auto) operator*=(Lhs& a, const Rhs& b) noexcept
    {
        Lhs result;
        index_wise<Rhs::cols, Lhs::rows>([&](size_t c, size_t r) {
            result.at(c, r) = 0;
            for (size_t i = 0; i < Lhs::cols; ++i) { result.at(c, r) += a.at(i, r) * b.at(c, i); }
            });
        std::swap(a, result);
        return a;
    }
    template<matrix_type Lhs, vector_type Rhs>
    requires (Lhs::cols == Rhs::component_count)
        constexpr auto operator*(const Lhs& a, const Rhs& b) noexcept
    {
        using result_scalar_type = decltype(typename Lhs::scalar_type{} *typename Rhs::scalar_type{});

        vec<result_scalar_type, Rhs::component_count> result;
        index_wise<1, Lhs::rows>([&](size_t c, size_t r) {
            result.at(r) = 0;
            for (size_t i = 0; i < Lhs::cols; ++i) { result.at(r) += a.at(i, r) * b.at(i); }
            });
        return result;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {~a}; }
    [[nodiscard]] constexpr auto operator~(M const& a) {
        using result_value_type =
            decltype(~std::declval<typename M::value_type>());
        mat<result_value_type, M::cols, M::rows> result;
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { result.at(c, r) = ~a.at(c, r); });
        return result;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {!a}; }
    [[nodiscard]] constexpr auto operator!(M const& a) {
        using result_value_type =
            decltype(~std::declval<typename M::value_type>());
        mat<result_value_type, M::cols, M::rows> result;
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { result.at(c, r) = !a.at(c, r); });
        return result;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {-a}; }
    [[nodiscard]] constexpr auto operator-(M const& a) {
        using result_value_type =
            decltype(~std::declval<typename M::value_type>());
        mat<result_value_type, M::cols, M::rows> result;
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { result.at(c, r) = -a.at(c, r); });
        return result;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {+a}; }
    [[nodiscard]] constexpr auto operator+(M const& a) {
        using result_value_type =
            decltype(~std::declval<typename M::value_type>());
        mat<result_value_type, M::cols, M::rows> result;
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { result.at(c, r) = +a.at(c, r); });
        return result;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {++a}; }
    constexpr M& operator++(M& a) {
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { ++a.at(c, r); });
        return a;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {--a}; }
    constexpr M& operator--(M& a) {
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { --a.at(c, r); });
        return a;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {a++}; }
    [[nodiscard]] constexpr M operator++(M& a, int) {
        M result = a;
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { a.at(c, r)++; });
        return result;
    }
    template<matrix_type M>
    requires requires(typename M::value_type a) { {a--}; }
    [[nodiscard]] constexpr M operator--(M& a, int) {
        M result = a;
        index_wise<M::cols, M::rows>([&](size_t c, size_t r) { a.at(c, r)--; });
        return result;
    }
}
