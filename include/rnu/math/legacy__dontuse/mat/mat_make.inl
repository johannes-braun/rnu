#pragma once

namespace gfx
{
    template<typename T, size_t Maj, size_t Min>
    constexpr mat<T, Maj, Min> make_diag(T value) noexcept
    {
        return mat<T, Maj, Min>(value, diagonal);
    }
    template<typename T, size_t Maj, size_t Min>
    constexpr mat<T, Maj, Min> make_identity() noexcept
    {
        return mat<T, Maj, Min>(identity);
    }

    template<typename T>
    mat<T, 4> make_persp(T fov, T aspect, T near, T far, bool left_handed = true)
    {
        const T depth = far - near;
        const T inv_depth = 1 / depth;

        mat<T, 4> result(gfx::identity);
        result[1][1] = 1 / tan(0.5f * fov);
        result[0][0] = (left_handed ? 1 : -1) * result[1][1] / aspect;
        result[2][2] = far * inv_depth;
        result[3][2] = (-far * near) * inv_depth;
        result[2][3] = 1;
        result[3][3] = 0;
        return result;
    }

    template<typename T>
    mat<T, 4> make_persp(T fov, T width, T height, T near, T far, bool left_handed = true)
    {
        return make_persp(fov, width / height, near, far, left_handed);
    }

    template<typename T>
    mat<T, 4> make_ortho(float left, float right, float top, float bottom, float near, float far)
    {
        const T rml = right - left;
        const T fmn = far - near;
        const T tmb = top - bottom;

        mat<T, 4> result(gfx::identity);
        result[0][0] = 2 / rml;
        result[1][1] = 2 / tmb;
        result[2][2] = -2 / fmn;

        result[3][0] = -((right + left) / rml);
        result[3][1] = -((top + bottom) / tmb);
        result[3][2] = -((far + near) / fmn);
        return result;
    }
}