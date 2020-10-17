#pragma once
#include <optional>
#include "math.hpp"

namespace rnu {
    template<typename Float>
    class camera
    {
    public:
        using float_type = Float;

        constexpr camera() = default;

        constexpr void axis(float_type delta_seconds, float_type f, float_type b, float_type l, float_type r, float_type u, float_type d) noexcept
        {
            m_translation += conj(m_rotation) * (delta_seconds * vec<float_type, 3>{
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
                const auto lr = angle_axis(x_delta, { 0.f, float(!z_up), float(z_up) });
                const auto ud = angle_axis(y_delta, { 1.f, 0.f, 0.f });

                m_rotation = ud * m_rotation * lr;
                m_last_x = x;
                m_last_y = y;
            }
            else
            {
                m_last_x = m_last_y = std::nullopt;
            }
        }

        [[nodiscard]] constexpr std::array<float_type, 16> matrix(bool row_major) const noexcept
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

            auto const t_rot = rotate(m_translation, m_rotation);

            std::array<float_type, 16> result{ 1.0f - 2.0f * qy * qy - 2.0f * qz * qz,
                2.0f * qx * qy - 2.0f * qz * qw, 2.0f * qx * qz + 2.0f * qy * qw, t_rot[0],
                2.0f * qx * qy + 2.0f * qz * qw, 1.0f - 2.0f * qx * qx - 2.0f * qz * qz,
                2.0f * qy * qz - 2.0f * qx * qw, t_rot[1], 2.0f * qx * qz - 2.0f * qy * qw,
                2.0f * qy * qz + 2.0f * qx * qw, 1.0f - 2.0f * qx * qx - 2.0f * qy * qy,
                t_rot[2], 0.0f, 0.0f, 0.0f, 1.0f };
            if (row_major)
                transpose(result);
            return result;
        }

        [[nodiscard]] constexpr static std::array<float_type, 16> projection(
            float_type fovy_radians, float_type aspect, float_type near, float_type far, bool row_major) noexcept
        {
            const float_type theta = fovy_radians * float_type(0.5);
            const float_type range = far - near;
            const float_type invtan = static_cast<float_type>(1.0 / tan(theta));

            std::array<float_type, 16> result{ 0 };
            result[0 * 4 + 0] = invtan / aspect;
            result[1 * 4 + 1] = invtan;
            result[2 * 4 + 2] = -(near + far) / range;
            result[3 * 4 + 2] = -1;
            result[2 * 4 + 3] = -2 * near * far / range;
            result[3 * 4 + 3] = 0;

            if (row_major)
                transpose(result);
            return result;
        }

    private:
        constexpr static void transpose(std::array<float_type, 16>& arr) noexcept
        {
            for (int i = 0; i < 4; ++i)
                for (int j = i; j < 4; ++j)
                    std::swap(arr[j * 4 + i], arr[i * 4 + j]);
        }

        [[nodiscard]] constexpr static quat<float_type> angle_axis(float_type radians, vec<float_type, 3> const& axis) noexcept
        {
            radians /= 2.0f;
            const float_type rcos = cos(radians);
            const float_type rsin = sin(radians);
            return { rcos, axis[0] * rsin, axis[1] * rsin, axis[2] * rsin };
        }

        [[nodiscard]] constexpr static vec<float_type, 3> rotate(vec<float_type, 3> const& v, quat<float_type> const& quat)
        {
            constexpr auto vmul = [](auto scalar, vec<float_type, 3> vector) {
                return vec<float_type, 3>{scalar* vector[0], scalar* vector[1], scalar* vector[2]};
            };

            auto const s = quat[0];
            const vec<float_type, 3> u{ quat[1], quat[2], quat[3] };

            const auto first = vmul(2 * dot(u, v), u);
            const auto second = vmul(s * s - dot(u, u), v);
            const auto third = vmul(2 * s, cross(u, v));

            return {
                first[0] + second[0] + third[0],
                first[1] + second[1] + third[1],
                first[2] + second[2] + third[2]
            };
        }

        quat<float_type> m_rotation{ 1.f, 0.f, 0.f, 0.f };
        vec<float_type, 3> m_translation{ 0.f, 0.f, 0.f };
        std::optional<float_type> m_last_x;
        std::optional<float_type> m_last_y;
    };
}