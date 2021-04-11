#pragma once
#include <type_traits>
#include <concepts>

namespace rnu
{
    template<typename T, size_t S> class vec;
    template<std::floating_point T> class quat_t;
    template<typename T, size_t Cols, size_t Rows> class mat;

    template<typename T> struct scalar_type_of { using type = T; };
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
    template<std::floating_point T> struct is_quat<quat_t<T>> : std::true_type {};
    template<typename T> constexpr bool is_quat_v = is_quat<T>::value;

    template<typename T> concept matrix_type = is_matrix_v<std::decay_t<T>>;
    template<typename T> concept vector_type = is_vector_v<std::decay_t<T>> && requires {
      { T::component_count };
      { T::value_type };
    };
    template<typename T> concept quaternion_type = is_quat_v<std::decay_t<T>>;
    template<typename T> concept scalar_type = !is_matrix_v<std::decay_t<T>> && !is_vector_v<std::decay_t<T>> && !is_quat_v<std::decay_t<T>>;

    template<typename T>
    concept floating_point_vector = vector_type<T> && std::floating_point<scalar_type_of_t<T>>;
    template<typename T>
    concept floating_point_matrix = matrix_type<T> && std::floating_point<scalar_type_of_t<T>>;
    template<typename T>
    concept floating_point_quat = quaternion_type<T>;
}