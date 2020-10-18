#pragma once

namespace gfx
{
    namespace detail
    {
        template<size_t Col, typename T, size_t S, size_t... Is>
        constexpr vec<T, S - 1> construct_det_submat_vector(const mat<T, S, S>& m, std::index_sequence<Is...>) noexcept
        {
            return vec<T, S - 1>(m[Col][Is + 1]...);
        }

        template<typename T, size_t S, size_t Col, size_t... Is>
        constexpr mat<T, S - 1, S - 1> construct_det_submat(const mat<T, S, S>& m, std::index_sequence<Is...>) noexcept
        {
            return mat<T, S - 1, S - 1>(construct_det_submat_vector < Is >= Col ? Is + 1 : Is >(m, std::make_index_sequence<S - 1>{})...);
        }

        template<typename T, size_t S, size_t... Columns>
        constexpr T compute_det(const mat<T, S, S>& m, std::index_sequence<Columns...>) noexcept
        {
            return ((m[Columns][0] * -(int(Columns & 0x1) * 2 - 1) * det(construct_det_submat<T, S, Columns>(m, std::make_index_sequence<S - 1>())))
                + ...);
        }
    }

    template<typename T, size_t S>
    constexpr T det(const mat<T, S, S>& m) noexcept
    {
        if constexpr (S == 0)
            return T{ 1 };
        else if constexpr (S == 1)
            return m[0][0];
        else if constexpr (S == 2)
            return m[0][0] * m[1][1] - m[1][0] * m[0][1];
        else if constexpr (S == 3)
        {
            const auto m_22_33 = m[1][1] * m[2][2];
            const auto m_21_32 = m[1][0] * m[2][1];
            const auto m_31_23 = m[2][0] * m[1][2];
            const auto m_31_22 = m[2][0] * m[1][1];
            const auto m_21_33 = m[1][0] * m[2][2];
            const auto m_32_23 = m[2][1] * m[1][2];

            return m[0][0] * (m_22_33 - m_32_23) + m[0][2] * (m_21_32 - m_31_22) + m[0][1] * (m_31_23 - m_21_33);
        }
        else if constexpr (S == 4)
        {
            const auto m_33_44_34_43 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            const auto m_32_44_42_34 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            const auto m_32_43_42_33 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            const auto m_31_44_41_43 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            const auto m_31_43_41_33 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            const auto m_31_42_41_32 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            return m[0][0] * (m[1][1] * m_33_44_34_43 - m[1][2] * m_32_44_42_34 + m[1][3] * m_32_43_42_33)
                - m[0][1] * (m[1][0] * m_33_44_34_43 - m[1][2] * m_31_44_41_43 + m[1][3] * m_31_43_41_33)
                + m[0][2] * (m[1][0] * m_32_44_42_34 - m[1][1] * m_31_44_41_43 + m[1][3] * m_31_42_41_32)
                - m[0][3] * (m[1][0] * m_32_43_42_33 - m[1][1] * m_31_43_41_33 + m[1][2] * m_31_42_41_32);
        }
        else
        {
            return detail::compute_det(m, std::make_index_sequence<S>{});
        }
    }
}