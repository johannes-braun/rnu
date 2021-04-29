#pragma once
#ifndef RNU_MATH_MAT_TYPE_HPP
#define RNU_MATH_MAT_TYPE_HPP

#include <array>
#include <algorithm>
#include "vec_type.hpp"

namespace rnu
{
  template<typename T, size_t Cols, size_t Rows>
  class mat;

  namespace detail
  {
    template<typename V, size_t Cols, size_t Rows>
    concept vector_like = vector_type<V> && std::min(Cols, Rows) == 1 &&
      std::max(Cols, Rows) == V::component_count;

    template<typename V, typename M>
    concept vector_matrix = matrix_type<M> && detail::vector_like<V, M::cols, M::rows>;
  }

  template<typename T, size_t Cols, size_t Rows>
  class mat {
  public:
    constexpr static size_t cols = Cols;
    constexpr static size_t rows = Rows;

    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using scalar_type = scalar_type_of_t<value_type>;
    using column_type = vec<T, Rows>;

    template<typename T>
    constexpr static bool vector_matrix = detail::vector_matrix<T, mat>;

    template<typename... ScalarList>
    constexpr static bool scalars_compatible = (sizeof...(ScalarList) == Cols * Rows) &&
      (std::is_convertible_v<ScalarList, value_type> && ...);

    [[nodiscard]] constexpr mat() noexcept;

    [[nodiscard]] constexpr mat(T diag) noexcept;

    template<typename... Scalars>
    [[nodiscard]] constexpr mat(Scalars&&... scalars)
      requires scalars_compatible<Scalars...>;

    [[nodiscard]] constexpr mat(std::initializer_list<column_type> values) noexcept;

    template<matrix_type Other>
    [[nodiscard]] constexpr mat(Other const& other) noexcept
      requires(Other::cols != cols || Other::rows != rows);

    template<typename V>
    [[nodiscard]] explicit constexpr mat(V v) noexcept
      requires vector_matrix<V>;

    template<typename V>
    [[nodiscard]] explicit constexpr operator V& ()
      requires vector_matrix<V>;

    template<typename V>
    [[nodiscard]] explicit constexpr operator V const& () const
      requires vector_matrix<V>;

    [[nodiscard]] constexpr T& at(size_t col, size_t row);
    [[nodiscard]] constexpr T const& at(size_t col, size_t row) const;
    [[nodiscard]] constexpr value_type& element(size_t linear_index);
    [[nodiscard]] constexpr value_type const& element(size_t linear_index) const;
    [[nodiscard]] constexpr column_type& col(size_t col);
    [[nodiscard]] constexpr column_type const& col(size_t col, size_t row) const;

    [[nodiscard]] constexpr T* data() noexcept;
    [[nodiscard]] constexpr T const* data() const noexcept;
    [[nodiscard]] constexpr size_type size() const noexcept;

    [[nodiscard]] constexpr operator bool() noexcept
      requires std::same_as<T, bool>;

  private:
    void fill_diag(T value);

    union {
      std::array<column_type, Cols> m_data{};
      std::array<value_type, Cols* Rows> m_linear_data;
    };
  };
}

#include "mat_type.inl.hpp"
#include "mat_math.inl.hpp"

#endif // RNU_MATH_MAT_TYPE_HPP
