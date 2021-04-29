#pragma once

#ifndef RNU_MATH_TRAITS_HPP
#define RNU_MATH_TRAITS_HPP

#include <type_traits>
#include <concepts>

namespace rnu
{
    template<typename T, size_t S> class vec;
    template<typename T> class quat_t;
    template<typename T, size_t Cols, size_t Rows> class mat;

    template<typename T> struct scalar_type_of { using type = std::decay_t<T>; };

    template<typename T, size_t S>
    struct scalar_type_of<vec<T, S>> : scalar_type_of<T>{};

    template<typename T, size_t Cols, size_t Rows>
    struct scalar_type_of<mat<T, Cols, Rows>> : scalar_type_of<T>{};

    template<typename T>
    struct scalar_type_of<quat_t<T>> : scalar_type_of<T> {};

    template<typename T>
    using scalar_type_of_t = typename scalar_type_of<T>::type;

    template<typename T> struct is_matrix : std::false_type {};
    template<typename T, size_t Cols, size_t Rows> struct is_matrix<mat<T, Cols, Rows>> : std::true_type {};
    template<typename T> constexpr bool is_matrix_v = is_matrix<T>::value;

    template<typename T> struct is_vector : std::false_type {};
    template<typename T, size_t S> struct is_vector<vec<T, S>> : std::true_type {};
    template<typename T> constexpr bool is_vector_v = is_vector<T>::value;

    template<typename T> struct is_quat : std::false_type {};
    template<typename T> struct is_quat<quat_t<T>> : std::true_type {};
    template<typename T> constexpr bool is_quat_v = is_quat<T>::value;

    template<typename T> concept matrix_type = is_matrix_v<std::decay_t<T>>;
    template<typename T> concept vector_type = is_vector_v<std::decay_t<T>>;
    template<typename T> concept quaternion_type = is_quat_v<std::decay_t<T>> && requires(T t) {
      t.matrix();
    };
    template<typename T> concept scalar_type = !is_matrix_v<std::decay_t<T>> && !is_vector_v<std::decay_t<T>> && !is_quat_v<std::decay_t<T>>;

    template<typename Int>
    [[nodiscard]] constexpr auto to_float(Int integer)
      requires (std::convertible_to<Int, float> || std::convertible_to<Int, double>)
    {
      if constexpr (sizeof(Int) <= sizeof(float))
        return static_cast<float>(integer);
      else
        return static_cast<double>(integer);
    }

    template<typename Int>
    using to_float_t = decltype(to_float(std::declval<Int>()));
}

#endif // RNU_MATH_TRAITS_HPP
