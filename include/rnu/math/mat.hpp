#pragma once

#include "mat_type.hpp"
#include <cstdint>

namespace rnu
{
    template<typename T> using mat2_t = mat<T, 2, 2>;
    template<typename T> using mat3_t = mat<T, 3, 3>;
    template<typename T> using mat4_t = mat<T, 4, 4>;

    using mat2 = mat2_t<float>;
    using mat3 = mat3_t<float>;
    using mat4 = mat4_t<float>;
    using mat2d = mat2_t<double>;
    using mat3d = mat3_t<double>;
    using mat4d = mat4_t<double>;
    using mat2i = mat2_t<int>;
    using mat3i = mat3_t<int>;
    using mat4i = mat4_t<int>;
    using mat2ui = mat2_t<unsigned>;
    using mat3ui = mat3_t<unsigned>;
    using mat4ui = mat4_t<unsigned>;
    using mat2i8 = mat2_t<std::int8_t>;
    using mat3i8 = mat3_t<std::int8_t>;
    using mat4i8 = mat4_t<std::int8_t>;
    using mat2ui8 = mat2_t<std::uint8_t>;
    using mat3ui8 = mat3_t<std::uint8_t>;
    using mat4ui8 = mat4_t<std::uint8_t>;
    using mat2i16 = mat2_t<std::int16_t>;
    using mat3i16 = mat3_t<std::int16_t>;
    using mat4i16 = mat4_t<std::int16_t>;
    using mat2ui16 = mat2_t<std::uint16_t>;
    using mat3ui16 = mat3_t<std::uint16_t>;
    using mat4ui16 = mat4_t<std::uint16_t>;
    using mat2i32 = mat2_t<std::int32_t>;
    using mat3i32 = mat3_t<std::int32_t>;
    using mat4i32 = mat4_t<std::int32_t>;
    using mat2ui32 = mat2_t<std::uint32_t>;
    using mat3ui32 = mat3_t<std::uint32_t>;
    using mat4ui32 = mat4_t<std::uint32_t>;
    using mat2i64 = mat2_t<std::int64_t>;
    using mat3i64 = mat3_t<std::int64_t>;
    using mat4i64 = mat4_t<std::int64_t>;
    using mat2ui64 = mat2_t<std::uint64_t>;
    using mat3ui64 = mat3_t<std::uint64_t>;
    using mat4ui64 = mat4_t<std::uint64_t>;

    template<std::floating_point T>
    [[noexcept]] constexpr auto translation(vec3_t<T> vector) noexcept {
        vec4 const hom(vector.x, vector.y, vector.z, 1);
        return mat4_t<T>{ {}, {}, {}, hom };
    }
    template<std::floating_point T>
    [[noexcept]] constexpr auto rotation(quat_t<T> quat) noexcept {
        return mat4_t<T>(quat.matrix());
    }
    template<std::floating_point T>
    [[noexcept]] constexpr auto scale(vec3_t<T> scales) noexcept {
        mat4_t result;
        result.at(0, 0) = scales.x;
        result.at(1, 1) = scales.y;
        result.at(2, 2) = scales.z;
        return result;
    }
}