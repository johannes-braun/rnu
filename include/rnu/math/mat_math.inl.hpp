#include <algorithm>

namespace rnu
{
    template<matrix_type M> requires (M::cols == M::rows)
        constexpr M& transpose_inplace(M& m) noexcept
    {
        for (size_t c = 0; c < M::cols; ++c)
            for (size_t r = c + 1; r < M::rows; ++r)
                std::swap(m.at(c, r), m.at(r, c));
        return m;
    }
    template<matrix_type M>
    [[nodiscard]] constexpr auto transpose(M m) noexcept
    {
        mat<typename M::value_type, M::rows, M::cols> result;
        for (size_t c = 0; c < M::cols; ++c)
            for (size_t r = 0; r < M::rows; ++r)
                result.at(r, c) = m.at(c, r);
        return result;
    }

    template<matrix_type M>
    [[nodiscard]] constexpr bool any_of(const M& m)
    {
        return std::any_of(m.data(), m.data() + m.size(), [](bool b) { return b; });
    }
    template<matrix_type M>
    [[nodiscard]] constexpr bool all_of(const M& m)
    {
        return std::all_of(m.data(), m.data() + m.size(), [](bool b) { return b; });
    }
    template<matrix_type M>
    [[nodiscard]] constexpr bool none_of(const M& m)
    {
        return std::none_of(m.data(), m.data() + m.size(), [](bool b) { return b; });
    }

    namespace detail
    {
        template<size_t Col, typename T, size_t S, size_t... Is>
        constexpr vec<T, S - 1> construct_det_submat_vector(const mat<T, S, S>& m, std::index_sequence<Is...>) noexcept
        {
            return vec<T, S - 1>(m.at(Col, Is + 1)...);
        }

        template<typename T, size_t S, size_t Col, size_t... Is>
        constexpr mat<T, S - 1, S - 1> construct_det_submat(const mat<T, S, S>& m, std::index_sequence<Is...>) noexcept
        {
            return mat<T, S - 1, S - 1>{
                construct_det_submat_vector < Is >= Col ? Is + 1 : Is >(m, std::make_index_sequence<S - 1>{})...
            };
        }

        template<typename T, size_t S, size_t... Columns>
        constexpr T compute_det(const mat<T, S, S>& m, std::index_sequence<Columns...>) noexcept
        {
            return ((m.at(Columns, 0) * -(int(Columns & 0x1) * 2 - 1) * det(construct_det_submat<T, S, Columns>(m, std::make_index_sequence<S - 1>())))
                + ...);
        }
    }

    template<typename T, size_t S>
    [[nodiscard]] constexpr T det(const mat<T, S, S>& m) noexcept
    {
        if constexpr (S == 0)
            return T{ 1 };
        else if constexpr (S == 1)
            return m.at(0, 0);
        else if constexpr (S == 2)
            return m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
        else if constexpr (S == 3)
        {
            const auto m_22_33 = m.at(1, 1) * m.at(2, 2);
            const auto m_21_32 = m.at(1, 0) * m.at(2, 1);
            const auto m_31_23 = m.at(2, 0) * m.at(1, 2);
            const auto m_31_22 = m.at(2, 0) * m.at(1, 1);
            const auto m_21_33 = m.at(1, 0) * m.at(2, 2);
            const auto m_32_23 = m.at(2, 1) * m.at(1, 2);

            return m.at(0, 0) * (m_22_33 - m_32_23) + m.at(0, 2) * (m_21_32 - m_31_22) + m.at(0, 1) * (m_31_23 - m_21_33);
        }
        else if constexpr (S == 4)
        {
            const auto m_33_44_34_43 = m.at(2, 2) * m.at(3, 3) - m.at(3, 2) * m.at(2, 3);
            const auto m_32_44_42_34 = m.at(2, 1) * m.at(3, 3) - m.at(3, 1) * m.at(2, 3);
            const auto m_32_43_42_33 = m.at(2, 1) * m.at(3, 2) - m.at(3, 1) * m.at(2, 2);
            const auto m_31_44_41_43 = m.at(2, 0) * m.at(3, 3) - m.at(3, 0) * m.at(2, 3);
            const auto m_31_43_41_33 = m.at(2, 0) * m.at(3, 2) - m.at(3, 0) * m.at(2, 2);
            const auto m_31_42_41_32 = m.at(2, 0) * m.at(3, 1) - m.at(3, 0) * m.at(2, 1);

            return m.at(0, 0) * (m.at(1, 1) * m_33_44_34_43 - m.at(1, 2) * m_32_44_42_34 + m.at(1, 3) * m_32_43_42_33)
                - m.at(0, 1) * (m.at(1, 0) * m_33_44_34_43 - m.at(1, 2) * m_31_44_41_43 + m.at(1, 3) * m_31_43_41_33)
                + m.at(0, 2) * (m.at(1, 0) * m_32_44_42_34 - m.at(1, 1) * m_31_44_41_43 + m.at(1, 3) * m_31_42_41_32)
                - m.at(0, 3) * (m.at(1, 0) * m_32_43_42_33 - m.at(1, 1) * m_31_43_41_33 + m.at(1, 2) * m_31_42_41_32);
        }
        else
        {
            return detail::compute_det(m, std::make_index_sequence<S>{});
        }
    }


    template<matrix_type M>
    [[nodiscard]] constexpr M inverse(M m)
    {
        constexpr auto S = M::rows;
        using T = typename M::scalar_type;
        if constexpr (S == 1)
            return { static_cast<T>(1.0 / m.at(0, 0)) };
        else if constexpr (S == 2)
        {
            const auto d = det(m);
            return mat<T, S, S>{
                vec<T, S>{m.at(1, 1) / d, m.at(0, 1) / d},
                    vec<T, S>{m.at(1, 0) / d, m.at(0, 0) / d},
            };
        }
        else if constexpr (S == 3)
        {
            float a00 = m.at(0, 0), a01 = m.at(1, 0), a02 = m.at(2, 0);
            float a10 = m.at(0, 1), a11 = m.at(1, 1), a12 = m.at(2, 1);
            float a20 = m.at(0, 2), a21 = m.at(1, 2), a22 = m.at(2, 2);

            float b01 = a22 * a11 - a12 * a21;
            float b11 = -a22 * a10 + a12 * a20;
            float b21 = a21 * a10 - a11 * a20;

            float d = a00 * b01 + a01 * b11 + a02 * b21;

            return mat<T, S, S>{vec<T, S>{b01 / d, b11 / d, b21 / d},
                vec<T, S>{(-a22 * a01 + a02 * a21) / d, (a22* a00 - a02 * a20) / d, (-a21 * a00 + a01 * a20) / d},
                vec<T, S>{(a12* a01 - a02 * a11) / d, (-a12 * a00 + a02 * a10) / d, (a11* a00 - a01 * a10) / d}};
        }
        else if constexpr (S == 4)
        {
            float a00 = m.at(0, 0), a01 = m.at(1, 0), a02 = m.at(2, 0), a03 = m.at(3, 0), a10 = m.at(0, 1), a11 = m.at(1, 1), a12 = m.at(2, 1), a13 = m.at(3, 1),
                a20 = m.at(0, 2), a21 = m.at(1, 2), a22 = m.at(2, 2), a23 = m.at(3, 2), a30 = m.at(0, 3), a31 = m.at(1, 3), a32 = m.at(2, 3), a33 = m.at(3, 3),

                b00 = a00 * a11 - a01 * a10, b01 = a00 * a12 - a02 * a10, b02 = a00 * a13 - a03 * a10, b03 = a01 * a12 - a02 * a11,
                b04 = a01 * a13 - a03 * a11, b05 = a02 * a13 - a03 * a12, b06 = a20 * a31 - a21 * a30, b07 = a20 * a32 - a22 * a30,
                b08 = a20 * a33 - a23 * a30, b09 = a21 * a32 - a22 * a31, b10 = a21 * a33 - a23 * a31, b11 = a22 * a33 - a23 * a32,

                det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

            return mat<T, S, S>{vec<T, S>{(a11* b11 - a12 * b10 + a13 * b09) / det, (a12* b08 - a10 * b11 - a13 * b07) / det,
                (a10* b10 - a11 * b08 + a13 * b06) / det, (a11* b07 - a10 * b09 - a12 * b06) / det},
                vec<T, S>{(a02* b10 - a01 * b11 - a03 * b09) / det, (a00* b11 - a02 * b08 + a03 * b07) / det,
                (a01* b08 - a00 * b10 - a03 * b06) / det, (a00* b09 - a01 * b07 + a02 * b06) / det},
                vec<T, S>{(a31* b05 - a32 * b04 + a33 * b03) / det, (a32* b02 - a30 * b05 - a33 * b01) / det,
                (a30* b04 - a31 * b02 + a33 * b00) / det, (a31* b01 - a30 * b03 - a32 * b00) / det},
                vec<T, S>{(a22* b04 - a21 * b05 - a23 * b03) / det, (a20* b05 - a22 * b02 + a23 * b01) / det,
                (a21* b02 - a20 * b04 - a23 * b00) / det, (a20* b03 - a21 * b01 + a22 * b00) / det}};
        }
        else
        {
            M mine = m;
            M inve;
            for (size_t col = 0; col < M::cols; ++col)
            {
                for (size_t row = col; row < M::rows; ++row)
                {
                    if (row == col)
                    {
                        const auto inv_val = T(1.0 / mine.at(col, row));
                        apply_each<S, 1>([&](size_t set_col, size_t)
                            {
                                inve.at(set_col, row) *= inv_val;
                                mine.at(set_col, row) *= inv_val;
                            });
                    }
                    else
                    {
                        if (const auto fac = mine.at(col, row))
                        {
                            apply_each<S, 1>([&](size_t set_col, size_t)
                                {
                                    inve.at(set_col, row) -= fac * inve.at(set_col, col);
                                    mine.at(set_col, row) -= fac * mine.at(set_col, col);
                                });
                        }
                    }
                }
            }
            for (ptrdiff_t scol = M::cols - 1; scol >= 0; --scol)
            {
                for (ptrdiff_t srow = M::rows - 1; srow >= scol + 1; --srow)
                {
                    const auto col = static_cast<size_t>(scol);
                    const auto row = static_cast<size_t>(srow);

                    if (const auto fac = mine.at(col, row))
                    {
                        apply_each<S, 1>([&](size_t set_col, size_t)
                            {
                                inve.at(set_col, row) -= fac * inve.at(set_col, col);
                                mine.at(set_col, row) -= fac * mine.at(set_col, col);
                            });
                    }
                }
            }

            return inve;
        }
    }
}