#pragma once

#include "vec_type.hpp"
#include "vec_op.hpp"
#include <cstdint>

namespace rnu {
    template<typename T>
    using vec1_t = vec<T, 1>;
    template<typename T>
    using vec2_t = vec<T, 2>;
    template<typename T>
    using vec3_t = vec<T, 3>;
    template<typename T>
    using vec4_t = vec<T, 4>;

    template<typename T>
    using complex_vec1_t = vec1_t<std::complex<T>>;
    template<typename T>
    using complex_vec2_t = vec2_t<std::complex<T>>;
    template<typename T>
    using complex_vec3_t = vec3_t<std::complex<T>>;
    template<typename T>
    using complex_vec4_t = vec4_t<std::complex<T>>;

    using vec1 = vec1_t<float>;
    using vec2 = vec2_t<float>;
    using vec3 = vec3_t<float>;
    using vec4 = vec4_t<float>;
    using vec1d = vec1_t<double>;
    using vec2d = vec2_t<double>;
    using vec3d = vec3_t<double>;
    using vec4d = vec4_t<double>;
    using vec1i = vec1_t<int>;
    using vec2i = vec2_t<int>;
    using vec3i = vec3_t<int>;
    using vec4i = vec4_t<int>;
    using vec1ui = vec1_t<unsigned>;
    using vec2ui = vec2_t<unsigned>;
    using vec3ui = vec3_t<unsigned>;
    using vec4ui = vec4_t<unsigned>;
    using vec1b = vec1_t<bool>;
    using vec2b = vec2_t<bool>;
    using vec3b = vec3_t<bool>;
    using vec4b = vec4_t<bool>;
    using vec1i8 = vec1_t<int8_t>;
    using vec2i8 = vec2_t<int8_t>;
    using vec3i8 = vec3_t<int8_t>;
    using vec4i8 = vec4_t<int8_t>;
    using vec1ui8 = vec1_t<uint8_t>;
    using vec2ui8 = vec2_t<uint8_t>;
    using vec3ui8 = vec3_t<uint8_t>;
    using vec4ui8 = vec4_t<uint8_t>;
    using vec1i16 = vec1_t<int16_t>;
    using vec2i16 = vec2_t<int16_t>;
    using vec3i16 = vec3_t<int16_t>;
    using vec4i16 = vec4_t<int16_t>;
    using vec1ui16 = vec1_t<uint16_t>;
    using vec2ui16 = vec2_t<uint16_t>;
    using vec3ui16 = vec3_t<uint16_t>;
    using vec4ui16 = vec4_t<uint16_t>;
    using vec1i32 = vec1_t<int32_t>;
    using vec2i32 = vec2_t<int32_t>;
    using vec3i32 = vec3_t<int32_t>;
    using vec4i32 = vec4_t<int32_t>;
    using vec1ui32 = vec1_t<uint32_t>;
    using vec2ui32 = vec2_t<uint32_t>;
    using vec3ui32 = vec3_t<uint32_t>;
    using vec4ui32 = vec4_t<uint32_t>;
    using vec1i64 = vec1_t<int64_t>;
    using vec2i64 = vec2_t<int64_t>;
    using vec3i64 = vec3_t<int64_t>;
    using vec4i64 = vec4_t<int64_t>;
    using vec1ui64 = vec1_t<uint64_t>;
    using vec2ui64 = vec2_t<uint64_t>;
    using vec3ui64 = vec3_t<uint64_t>;
    using vec4ui64 = vec4_t<uint64_t>;
}