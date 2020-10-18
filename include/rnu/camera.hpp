#pragma once
#include <optional>
#include "math/math.hpp"
#include <concepts>

namespace rnu {
    template<std::floating_point Float>
    class camera
    {
    public:
        using float_type = Float;
        using vec_type = vec<float_type, 3>;
        using quat_type = quat_t<float_type>;
        using mat_type = mat<float_type, 4, 4>;

        constexpr camera() = default;
        constexpr camera(vec_type translation) : m_translation(translation) {};

        constexpr void axis(float_type delta_seconds, float_type f, float_type b, float_type l, float_type r, float_type u, float_type d) noexcept
        {
            m_translation += conj(m_rotation) * (delta_seconds * vec_type{
                (l-r), (d-u), (f-b)
            });
        }

        constexpr void mouse(float_type x, float_type y, bool down, bool z_up = false) noexcept
        {
            if (down)
            {
                if (!m_last_x)
                {
                    m_last_x = x;
                    m_last_y = y;
                    return;
                }

                const float_type x_delta = (x - m_last_x.value()) / 100.0f;
                const float_type y_delta = (y - m_last_y.value()) / 100.0f;
                const auto lr = quat_type(x_delta, { 0.f, float(!z_up), float(z_up) });
                const auto ud = quat_type(y_delta, { 1.f, 0.f, 0.f });

                m_rotation = ud * m_rotation * lr;
                m_last_x = x;
                m_last_y = y;
            }
            else
            {
                m_last_x = m_last_y = std::nullopt;
            }
        }

        [[nodiscard]] constexpr mat_type matrix(bool row_major) const noexcept
        {
            float_type qw = m_rotation[0];
            float_type qx = m_rotation[1];
            float_type qy = m_rotation[2];
            float_type qz = m_rotation[3];
            const float_type n = 1.0f / sqrt(qx * qx + qy * qy + qz * qz + qw * qw);
            qx *= n;
            qy *= n;
            qz *= n;
            qw *= n;

            auto const t_rot = m_rotation * m_translation;

            mat_type result{ 1.0f - 2.0f * qy * qy - 2.0f * qz * qz,
                2.0f * qx * qy - 2.0f * qz * qw, 2.0f * qx * qz + 2.0f * qy * qw, t_rot[0],
                2.0f * qx * qy + 2.0f * qz * qw, 1.0f - 2.0f * qx * qx - 2.0f * qz * qz,
                2.0f * qy * qz - 2.0f * qx * qw, t_rot[1], 2.0f * qx * qz - 2.0f * qy * qw,
                2.0f * qy * qz + 2.0f * qx * qw, 1.0f - 2.0f * qx * qx - 2.0f * qy * qy,
                t_rot[2], 0.0f, 0.0f, 0.0f, 1.0f };
            if (row_major)
                transpose_inplace(result);
            return result;
        }

        [[nodiscard]] constexpr static mat_type projection(
            float_type fovy_radians, float_type aspect, float_type near, float_type far, bool row_major) noexcept
        {
            const float_type theta = fovy_radians * float_type(0.5);
            const float_type range = far - near;
            const float_type invtan = static_cast<float_type>(1.0 / tan(theta));

            mat_type result{ 0 };
            result.at(0, 0) = invtan / aspect;
            result.at(1, 1) = invtan;
            result.at(2, 2) = -(near + far) / range;
            result.at(3, 2) = -1;
            result.at(2, 3) = -2 * near * far / range;
            result.at(3, 3) = 0;

            if (row_major)
                transpose_inplace(result);
            return result;
        }
        [[nodiscard]] constexpr static mat_type orthographic(float_type left, float_type right, float_type top, float_type bottom, float_type near, float_type far)
        {
            const auto rml = right - left;
            const auto fmn = far - near;
            const auto tmb = top - bottom;

            mat_type result;
            result.at(0, 0) = 2 / rml;
            result.at(1, 1) = 2 / tmb;
            result.at(2, 2) = -2 / fmn;
            result.at(3, 0) = -((right + left) / rml);
            result.at(3, 1) = -((top + bottom) / tmb);
            result.at(3, 2) = -((far + near) / fmn);
            return result;
        }

        [[nodiscard]] constexpr vec_type position() const noexcept {
            return m_translation;
        }

    private:
        quat_type m_rotation{ 1.f, 0.f, 0.f, 0.f };
        vec_type m_translation{ 0.f, 0.f, 0.f };
        std::optional<float_type> m_last_x;
        std::optional<float_type> m_last_y;
    };
}