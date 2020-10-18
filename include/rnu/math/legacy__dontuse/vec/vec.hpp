#pragma once

#include <array>
#include <complex>
#include <algorithm>
#include <numeric>

namespace gfx {
namespace detail {
	constexpr size_t max_static_vector_size = 24;

template<typename T, size_t S>
struct vec_components
{
    T components[S]{ 0 };
};

template<typename T>
struct vec_components<T, 1>
{
    union
    {
		T components[1]{ 0 };
        struct
        {
            T x;
        };
        struct
        {
            T r;
        };
        struct
        {
            T u;
        };
    };
};

template<typename T>
struct vec_components<T, 2>
{
    union
    {
		T components[2]{ 0 };
        struct
        {
            T x, y;
        };
        struct
        {
            T r, g;
        };
        struct
        {
            T u, v;
        };
    };
};

template<typename T>
struct vec_components<T, 3>
{
    union
    {
		T components[3]{ 0 };
        struct
        {
            T x, y, z;
        };
        struct
        {
            T r, g, b;
        };
        struct
        {
            T u, v, w;
        };
    };
};

template<typename T>
struct vec_components<T, 4>
{
    union
    {
		T components[4]{ 0 };
        struct
        {
            T x, y, z, w;
        };
        struct
        {
            T r, g, b, a;
        };
        struct
        {
            T u, v, s, t;
        };
    };
};
}    // namespace detail

template<typename T, size_t S>
class vec;

template<typename T>
struct vec_tuple;

template<size_t I, typename T>
struct vec_tuple<vec<T, I>>
{
    template<typename... Args>
    using type = typename vec_tuple<vec<T, I - 1>>::template type<T, Args...>;
};

template<typename T>
struct vec_tuple<vec<T, 0>>
{
    template<typename... Args>
    using type = std::tuple<Args...>;
};
template<typename T>
using vec_tuple_t = typename vec_tuple<T>::template type<>;

template<typename T, size_t S>
class vec : public detail::vec_components<T, S>
{
public:
    using reference       = T&;
    using const_reference = const T&;
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using pointer         = T*;
    using const_pointer   = const T*;

    using iterator               = typename std::array<value_type, S>::iterator;
    using const_iterator         = typename std::array<value_type, S>::const_iterator;
    using reverse_iterator       = typename std::array<value_type, S>::reverse_iterator;
    using const_reverse_iterator = typename std::array<value_type, S>::const_reverse_iterator;

    constexpr static size_t component_count = S;

    auto begin() { return reinterpret_cast<std::array<T, S>&>(this->components).begin(); }
    auto end() { return reinterpret_cast<std::array<T, S>&>(this->components).end(); }
    auto begin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).begin(); }
    auto end() const { return reinterpret_cast<const std::array<T, S>&>(this->components).end(); }
    auto cbegin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).begin(); }
    auto cend() const { return reinterpret_cast<const std::array<T, S>&>(this->components).end(); }
    auto rbegin() { return reinterpret_cast<std::array<T, S>&>(this->components).rbegin(); }
    auto rend() { return reinterpret_cast<std::array<T, S>&>(this->components).rend(); }
    auto rbegin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rbegin(); }
    auto rend() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rend(); }
    auto crbegin() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rbegin(); }
    auto crend() const { return reinterpret_cast<const std::array<T, S>&>(this->components).rend(); }

    explicit constexpr vec() noexcept;
    template<typename X, size_type D, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
    explicit constexpr vec(const vec<X, D>& other) noexcept;
    template<typename X, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
    explicit constexpr vec(const X* ptr);
    template<typename X, typename = std::enable_if_t<std::is_convertible_v<X, T>>>
    explicit constexpr vec(X* ptr);
    explicit constexpr vec(T&& value) noexcept;
    template<typename... Ts, typename = std::enable_if_t<S == sizeof...(Ts) && (S > 1)>>
    explicit constexpr vec(Ts&&... ts) noexcept;

private:
	template<std::size_t... Is, typename UnaryConvertFun>
	constexpr auto apply(std::index_sequence<Is...>, UnaryConvertFun&& fun) const noexcept;
	template<std::size_t... Is, typename UnaryConvertFun>
	constexpr auto apply(std::index_sequence<Is...>, const vec& other, UnaryConvertFun&& fun) const noexcept;

public:
	constexpr auto real() const noexcept;
	constexpr auto imag() const noexcept;

    constexpr reference       at(size_type index);
    constexpr const_reference at(size_type index) const;
    constexpr reference       operator[](size_type index);
    constexpr const_reference operator[](size_type index) const;
    constexpr pointer         data() noexcept;
    constexpr const_pointer   data() const noexcept;
    constexpr size_type       size() const noexcept;
    constexpr void            fill(const T& value);
	
    constexpr operator vec_tuple_t<vec>&() noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
    constexpr operator const vec_tuple_t<vec>&() noexcept { return reinterpret_cast<const vec_tuple_t<vec>&>(*this); }
    constexpr vec_tuple_t<vec>& tuple() noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
	constexpr const vec_tuple_t<vec>& tuple() const noexcept { return reinterpret_cast<vec_tuple_t<vec>&>(*this); }
	
private:

    template<std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, T&& value) noexcept;
    template<typename X, size_type D, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const vec<X, D>& other) noexcept;
    template<typename X, std::size_t... Is>
    explicit constexpr vec(std::index_sequence<Is...>, const X* other) noexcept;
};

template<typename T>
class vec<T, 0>
{};

template<typename T>
using tvec1 = vec<T, 1>;
template<typename T>
using tvec2 = vec<T, 2>;
template<typename T>
using tvec3 = vec<T, 3>;
template<typename T>
using tvec4 = vec<T, 4>;

template<typename T>
using tcvec1 = vec<std::complex<T>, 1>;
template<typename T>
using tcvec2 = vec<std::complex<T>, 2>;
template<typename T>
using tcvec3 = vec<std::complex<T>, 3>;
template<typename T>
using tcvec4 = vec<std::complex<T>, 4>;

using vec1    = tvec1<float>;
using vec2    = tvec2<float>;
using vec3    = tvec3<float>;
using vec4    = tvec4<float>;
using vec1d   = tvec1<double>;
using vec2d   = tvec2<double>;
using vec3d   = tvec3<double>;
using vec4d   = tvec4<double>;
using vec1i   = tvec1<int>;
using vec2i   = tvec2<int>;
using vec3i   = tvec3<int>;
using vec4i   = tvec4<int>;
using vec1ui   = tvec1<unsigned>;
using vec2ui   = tvec2<unsigned>;
using vec3ui   = tvec3<unsigned>;
using vec4ui   = tvec4<unsigned>;
using vec1b   = tvec1<bool>;
using vec2b   = tvec2<bool>;
using vec3b   = tvec3<bool>;
using vec4b   = tvec4<bool>;
using vec1i8 = tvec1<int8_t>;
using vec2i8 = tvec2<int8_t>;
using vec3i8 = tvec3<int8_t>;
using vec4i8 = tvec4<int8_t>;
using vec1ui8 = tvec1<uint8_t>;
using vec2ui8 = tvec2<uint8_t>;
using vec3ui8 = tvec3<uint8_t>;
using vec4ui8 = tvec4<uint8_t>;
using vec1i16 = tvec1<int16_t>;
using vec2i16 = tvec2<int16_t>;
using vec3i16 = tvec3<int16_t>;
using vec4i16 = tvec4<int16_t>;
using vec1ui16 = tvec1<uint16_t>;
using vec2ui16 = tvec2<uint16_t>;
using vec3ui16 = tvec3<uint16_t>;
using vec4ui16 = tvec4<uint16_t>;
using vec1i32 = tvec1<int32_t>;
using vec2i32 = tvec2<int32_t>;
using vec3i32 = tvec3<int32_t>;
using vec4i32 = tvec4<int32_t>;
using vec1ui32 = tvec1<uint32_t>;
using vec2ui32 = tvec2<uint32_t>;
using vec3ui32 = tvec3<uint32_t>;
using vec4ui32 = tvec4<uint32_t>;
using vec1i64 = tvec1<int64_t>;
using vec2i64 = tvec2<int64_t>;
using vec3i64 = tvec3<int64_t>;
using vec4i64 = tvec4<int64_t>;
using vec1ui64 = tvec1<uint64_t>;
using vec2ui64 = tvec2<uint64_t>;
using vec3ui64 = tvec3<uint64_t>;
using vec4ui64 = tvec4<uint64_t>;

template<typename T, size_t S>
constexpr T dot(const vec<T, S>& a, const vec<T, S>& b) noexcept;
template<typename T, size_t S>
constexpr auto length(const vec<T, S>& a) noexcept;
template<typename T, size_t S>
constexpr auto normalize(const vec<T, S>& a) noexcept;
template<typename T>
constexpr auto cross(const vec<T, 3>& a, const vec<T, 3>& b) noexcept;
template<size_t S>
constexpr bool any_of(const vec<bool, S>& v);
template<size_t S>
constexpr bool all_of(const vec<bool, S>& v);
template<size_t S>
constexpr bool none_of(const vec<bool, S>& v);
template<typename T, size_t S>
constexpr auto min_element(const vec<T, S>& v);
template<typename T, size_t S>
constexpr auto max_element(const vec<T, S>& v);
template<typename T, size_t S>
constexpr auto minmax_element(const vec<T, S>& v);
}    // namespace gfx

#include "vec/vec_apply.inl"
#include "vec/vec.inl"
#include "vec/vec_get.inl"
#include "vec/vec_ops.inl"
#include "vec/vec_std.inl"