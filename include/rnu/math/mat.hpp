#pragma once

#include "vec.hpp"

namespace rnu
{
  namespace detail {
    template<typename T, size_t C, size_t R>
    struct row_reference {
      friend class mat<T, C, R>;
      using column_type = vec<T, R>;
      using row_type = vec<T, C>;

      constexpr row_type get() const {
        row_type result;
        for (size_t i = 0; i < C; ++i)
          result[i] = *_value_ptrs[i];
        return result;
      }

      template<typename Other> requires(std::convertible_to<T, Other>)
        constexpr operator vec<Other, C>() const {
        vec<Other, C> result;
        for (size_t i = 0; i < C; ++i)
          result[i] = static_cast<Other>(*_value_ptrs[i]);
        return result;
      }

      constexpr row_reference& operator=(row_type const& row) {
        for (size_t i = 0; i < C; ++i)
          *_value_ptrs[i] = row[i];
        return *this;
      }

      constexpr row_reference(row_reference&&) = delete;
      constexpr row_reference(row_reference const&) = delete;
      constexpr row_reference& operator=(row_reference&&) = delete;
      constexpr row_reference& operator=(row_reference const&) = delete;

    private:
      constexpr row_reference(std::array<column_type, C>& columns, size_t row)
        : row_reference(columns, row, std::make_index_sequence<C>()) {}

      template<size_t... Indices>
      constexpr row_reference(std::array<column_type, C>& columns, size_t row, std::index_sequence<Indices...>)
        : _value_ptrs{ &(columns[Indices][row])... }
      {
      }

      std::array<T*, C> _value_ptrs;
      size_t _row;
    };

    template<size_t I, typename V, size_t C, size_t R, typename T>
    constexpr static T value_if_diag(V&& v) {
      constexpr auto row = I % C;
      constexpr auto col = I / C;
      if constexpr (row == col)
        return static_cast<T>(v);
      else
        return T(0);
    }

    template<size_t I, size_t C, size_t R, typename T, typename... V>
    constexpr static T value_if_diag_va(std::tuple<V...> const& v) {
      constexpr auto row = I % C;
      constexpr auto col = I / C;
      if constexpr (row == col)
        return static_cast<T>(std::get<row>(v));
      else
        return T(0);
    }

    template<typename T>
    struct decay_matrix {
      using type = T;
    };
    template<typename T>
    struct decay_matrix<vec<T, 1>> {
      using type = T;
    };
    template<typename T, size_t R>
    struct decay_matrix<mat<T, 1, R>> {
      using type = typename decay_matrix<vec<T, R>>::type;
    };
    template<typename T>
    using decay_matrix_t = typename decay_matrix<T>::type;

    template<typename Fun, size_t... Rows>
    constexpr auto traverse_rows(Fun&& fun, size_t col, std::index_sequence<Rows...>)
    {
      constexpr auto rows = sizeof...(Rows);
      using result_type = decltype(fun(size_t{}, size_t{}));

      if constexpr (std::same_as<result_type, void>)
        (fun(col, Rows), ...);
      else if constexpr (std::is_reference_v<result_type>)
      {
        using vector_type = vec<std::reference_wrapper<std::decay_t<result_type>>, rows>;
        return vector_type{ fun(col, Rows)... };
      }
      else {
        using vector_type = vec<std::decay_t<result_type>, rows>;
        return vector_type{ fun(col, Rows)... };
      }
    }
    template<size_t R, typename Fun, size_t... Cols>
    constexpr auto traverse_impl(Fun&& fun, std::index_sequence<Cols...>)
    {
      constexpr auto cols = sizeof...(Cols);
      constexpr auto rows = R;
      using result_type = decltype(fun(size_t{}, size_t{}));

      if constexpr (std::same_as<result_type, void>)
        (traverse_rows<Fun>(std::forward<Fun>(fun), Cols, std::make_index_sequence<R>()), ...);
      else if constexpr (std::is_reference_v<result_type>)
      {
        using matrix_type = detail::decay_matrix_t<mat<std::reference_wrapper<std::decay_t<result_type>>, cols, rows>>;
        return matrix_type{ traverse_rows<Fun>(std::forward<Fun>(fun), Cols, std::make_index_sequence<R>())... };
      }
      else {
        using matrix_type = detail::decay_matrix_t<mat<std::decay_t<result_type>, cols, rows>>;
        return matrix_type{ traverse_rows<Fun>(std::forward<Fun>(fun), Cols, std::make_index_sequence<R>())... };
      }
    }

    template<size_t C, size_t R, typename Fun>
    constexpr auto traverse(Fun&& fun)
    {
      if constexpr (C == 1 && R == 1)
        return fun(size_t(0), size_t(0));
      else
        return traverse_impl<R>(std::forward<Fun>(fun), std::make_index_sequence<C>());
    }
  }

  template<typename T, size_t C, size_t R>
  class mat {
    static constexpr struct fill_sentinel_t {} fill;
    static constexpr struct fill_all_sentinel_t {} fill_all;
    static constexpr struct diag_sentinel_t {} diag;
    static constexpr struct diag_fill_sentinel_t {} diag_fill;

  public:
    using value_type = T;
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    using row_type = vec<T, C>;
    using row_reference = detail::row_reference<T, C, R>;
    using column_type = vec<T, R>;
    using column_reference = column_type&;
    using const_column_reference = const column_type&;

    static constexpr size_type columns = C;
    static constexpr size_type rows = R;
    static constexpr bool is_square = C == R;

    using iterator = typename std::array<column_type, R>::iterator;
    using const_iterator = typename std::array<column_type, R>::const_iterator;
    using reverse_iterator = typename std::array<column_type, R>::reverse_iterator;
    using const_reverse_iterator = typename std::array<column_type, R>::const_reverse_iterator;

    constexpr mat() : mat(T(1)) {}

    constexpr mat(T value) : mat(diag, std::make_index_sequence<R* C>(), std::move(value)) {

    }

    template<vector... V>
    constexpr mat(V&&... vs) requires ((detail::count_v<V> == R) && ...) && (sizeof...(V) == C) && (std::is_convertible_v<V, column_type> && ...)
      : _columns{ static_cast<column_type>(std::forward<V>(vs))... } {

    }

    template<typename... Ts>
    constexpr mat(Ts&&... values) requires ((detail::count_v<Ts> == 1) && ...) && (sizeof...(Ts) == R * C) && (std::is_convertible_v<Ts, T> && ...)
      : _linear_memory{ static_cast<T>(std::forward<Ts>(values))... } {

    }

    template<typename... Ts>
    constexpr mat(Ts&&... values) requires ((detail::count_v<Ts> == 1) && ...) && (sizeof...(Ts) == R) && (std::is_convertible_v<Ts, T> && ...) && is_square
      : mat(diag_fill, std::make_index_sequence<R* C>(), std::forward_as_tuple(values...))
    {

    }

    template<typename V>
    constexpr mat(mat<V, C, R> const& other) : mat(fill_all, std::make_index_sequence<R* C>(), other.data()) {

    }

    template<typename V, size_t Oc, size_t Or> requires (Oc < C&& Or <= R) || (Oc <= C && Or < R)
      constexpr mat(mat<V, Oc, Or> const& other) : mat(detail::traverse<C, R>([&](size_t c, size_t r) {
      if (c < Oc && r < Or)
        return static_cast<T>(other[c][r]);
      else if (c == r)
        return T(1);
      else return T(0);
        })) {

    }

    [[nodiscard]] constexpr size_type size() const { return R * C; }
    [[nodiscard]] constexpr pointer data() { return this->_linear_memory.data(); }
    [[nodiscard]] constexpr const_pointer data() const { return this->_linear_memory.data(); }

    [[nodiscard]] constexpr iterator begin() { return std::begin(this->_columns); }
    [[nodiscard]] constexpr const_iterator begin() const { return std::begin(this->_columns); }
    [[nodiscard]] constexpr const_iterator cbegin() const { return std::cbegin(this->_columns); }
    [[nodiscard]] constexpr reverse_iterator rbegin() { return std::rbegin(this->_columns); }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const { return std::rbegin(this->_columns); }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const { return std::crbegin(this->_columns); }

    [[nodiscard]] constexpr iterator end() { return std::end(this->_columns); }
    [[nodiscard]] constexpr const_iterator end() const { return std::end(this->_columns); }
    [[nodiscard]] constexpr const_iterator cend() const { return std::cend(this->_columns); }
    [[nodiscard]] constexpr reverse_iterator rend() { return std::rend(this->_columns); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const { return std::rend(this->_columns); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const { return std::crend(this->_columns); }

    [[nodiscard]] constexpr column_reference operator[](size_t column) { return this->_columns[column]; }
    [[nodiscard]] constexpr const_column_reference operator[](size_t column) const { return this->_columns[column]; }
    [[nodiscard]] constexpr column_reference at(size_t column) { return this->_columns[column]; }
    [[nodiscard]] constexpr const_column_reference at(size_t column) const { return this->_columns[column]; }

    [[nodiscard]] constexpr reference at(size_t column, size_t row) { return this->at(column).at(row); }
    [[nodiscard]] constexpr const_reference at(size_t column, size_t row) const { return this->at(column).at(row); }

    [[nodiscard]] constexpr column_reference col(size_t column) { return this->_columns[column]; }
    [[nodiscard]] constexpr const_column_reference col(size_t column) const { return this->_columns[column]; }

    [[nodiscard]] constexpr row_reference row(size_t row) {
      return row_reference(this->_columns, row);
    }
    [[nodiscard]] constexpr row_type row(size_t row) const {
      return row_reference(const_cast<mat*>(this)->_columns, row).get();
    }

    [[nodiscard]] constexpr operator bool() const noexcept
      requires std::same_as<T, bool>
    {
      auto const* d = data();
      auto const s = size();
      return std::all_of(d, d + s, [](auto const& v) { return v; });
    }

  private:
    template<typename V, size_t... Indices>
    constexpr mat(fill_sentinel_t, V&& v, std::index_sequence<Indices...>)
      : _linear_memory{ (Indices, v)... }
    {

    }

    template<typename V, size_t... Indices>
    constexpr mat(diag_sentinel_t, std::index_sequence<Indices...>, V&& v)
      : _linear_memory{ detail::value_if_diag<Indices, V, C, R, T>(std::forward<V>(v))... }
    {

    }

    template<size_t... Indices, typename... V>
    constexpr mat(diag_fill_sentinel_t, std::index_sequence<Indices...>, std::tuple<V...> v)
      : _linear_memory{ detail::value_if_diag_va<Indices, C, R, T>(v)... }
    {

    }

    template<size_t... Indices, typename V>
    constexpr mat(fill_all_sentinel_t, std::index_sequence<Indices...>, V* v)
      : _linear_memory{ static_cast<T>(v[Indices])... }
    {

    }

    union
    {
      std::array<column_type, C> _columns{};
      std::array<T, R* C> _linear_memory;
    };
  };

  namespace detail {
    template<size_t Col, typename T, size_t S, size_t... Is>
    constexpr vec<T, S - 1> construct_det_submat_vector(const mat<T, S, S>& m, std::index_sequence<Is...>) noexcept
    {
      return vec<T, S - 1>(m.at(Col, Is + 1)...);
    }

    template<typename T, size_t S, size_t Col, size_t... Is>
    constexpr mat<T, S - 1, S - 1> construct_det_submat(const mat<T, S, S>& m, std::index_sequence<Is...>) noexcept
    {
      return mat<T, S - 1, S - 1>{
        construct_det_submat_vector < Is >= Col ? Is + 1 : Is >(m, std::make_index_sequence<S - 1>{})...
      };
    }
    template<typename T, size_t S, size_t... Columns>
    constexpr T compute_det(const mat<T, S, S>& m, std::index_sequence<Columns...>) noexcept
    {
      return ((m.at(Columns, 0) * -(int(Columns & 0x1) * 2 - 1) * det(construct_det_submat<T, S, Columns>(m, std::make_index_sequence<S - 1>())))
        + ...);
    }

    template<matrix T>
    constexpr decltype(auto) get(T& t, size_t col, size_t row) { return t[col][row]; }
    template<vector T>
    constexpr decltype(auto) get(T& t, size_t col, size_t row) { return t[row]; }
    template<typename T>
    constexpr decltype(auto) get(T& t, size_t col, size_t row) { return t; }
    template<typename T>
    constexpr decltype(auto) get(T&& t, size_t col, size_t row) { return get(static_cast<T&>(t), col, row); }
  }

  template<typename T, size_t S> requires std::is_floating_point_v<T>
    [[nodiscard]] constexpr T det(const mat<T, S, S>& m) noexcept
    {
      if constexpr (S == 0)
        return T{ 1 };
      else if constexpr (S == 1)
        return m.at(0, 0);
      else if constexpr (S == 2)
        return m.at(0, 0) * m.at(1, 1) - m.at(1, 0) * m.at(0, 1);
      else if constexpr (S == 3)
      {
        const auto m_22_33 = m.at(1, 1) * m.at(2, 2);
        const auto m_21_32 = m.at(1, 0) * m.at(2, 1);
        const auto m_31_23 = m.at(2, 0) * m.at(1, 2);
        const auto m_31_22 = m.at(2, 0) * m.at(1, 1);
        const auto m_21_33 = m.at(1, 0) * m.at(2, 2);
        const auto m_32_23 = m.at(2, 1) * m.at(1, 2);

        return m.at(0, 0) * (m_22_33 - m_32_23) + m.at(0, 2) * (m_21_32 - m_31_22) + m.at(0, 1) * (m_31_23 - m_21_33);
      }
      else if constexpr (S == 4)
      {
        const auto m_33_44_34_43 = m.at(2, 2) * m.at(3, 3) - m.at(3, 2) * m.at(2, 3);
        const auto m_32_44_42_34 = m.at(2, 1) * m.at(3, 3) - m.at(3, 1) * m.at(2, 3);
        const auto m_32_43_42_33 = m.at(2, 1) * m.at(3, 2) - m.at(3, 1) * m.at(2, 2);
        const auto m_31_44_41_43 = m.at(2, 0) * m.at(3, 3) - m.at(3, 0) * m.at(2, 3);
        const auto m_31_43_41_33 = m.at(2, 0) * m.at(3, 2) - m.at(3, 0) * m.at(2, 2);
        const auto m_31_42_41_32 = m.at(2, 0) * m.at(3, 1) - m.at(3, 0) * m.at(2, 1);

        return m.at(0, 0) * (m.at(1, 1) * m_33_44_34_43 - m.at(1, 2) * m_32_44_42_34 + m.at(1, 3) * m_32_43_42_33)
          - m.at(0, 1) * (m.at(1, 0) * m_33_44_34_43 - m.at(1, 2) * m_31_44_41_43 + m.at(1, 3) * m_31_43_41_33)
          + m.at(0, 2) * (m.at(1, 0) * m_32_44_42_34 - m.at(1, 1) * m_31_44_41_43 + m.at(1, 3) * m_31_42_41_32)
          - m.at(0, 3) * (m.at(1, 0) * m_32_43_42_33 - m.at(1, 1) * m_31_43_41_33 + m.at(1, 2) * m_31_42_41_32);
      }
      else
      {
        return detail::compute_det(m, std::make_index_sequence<S>{});
      }
    }

    template<matrix M>
    [[nodiscard]] constexpr M inverse(M const& m) requires (std::decay_t<M>::is_square&& std::is_floating_point_v<typename std::decay_t<M>::value_type>)
    {
      constexpr auto S = std::decay_t<M>::rows;
      using T = typename std::decay_t<M>::value_type;
      if constexpr (S == 1)
        return { static_cast<T>(1.0 / m.at(0, 0)) };
      else if constexpr (S == 2)
      {
        const auto d = det(m);
        return mat<T, S, S>{
          vec<T, S>{m.at(1, 1) / d, m.at(0, 1) / d},
            vec<T, S>{m.at(1, 0) / d, m.at(0, 0) / d},
        };
      }
      else if constexpr (S == 3)
      {
        float a00 = m.at(0, 0), a01 = m.at(1, 0), a02 = m.at(2, 0);
        float a10 = m.at(0, 1), a11 = m.at(1, 1), a12 = m.at(2, 1);
        float a20 = m.at(0, 2), a21 = m.at(1, 2), a22 = m.at(2, 2);

        float b01 = a22 * a11 - a12 * a21;
        float b11 = -a22 * a10 + a12 * a20;
        float b21 = a21 * a10 - a11 * a20;

        float d = a00 * b01 + a01 * b11 + a02 * b21;

        return mat<T, S, S>{vec<T, S>{b01 / d, b11 / d, b21 / d},
          vec<T, S>{(-a22 * a01 + a02 * a21) / d, (a22* a00 - a02 * a20) / d, (-a21 * a00 + a01 * a20) / d},
          vec<T, S>{(a12* a01 - a02 * a11) / d, (-a12 * a00 + a02 * a10) / d, (a11* a00 - a01 * a10) / d}};
      }
      else if constexpr (S == 4)
      {
        float a00 = m.at(0, 0), a01 = m.at(1, 0), a02 = m.at(2, 0), a03 = m.at(3, 0), a10 = m.at(0, 1), a11 = m.at(1, 1), a12 = m.at(2, 1), a13 = m.at(3, 1),
          a20 = m.at(0, 2), a21 = m.at(1, 2), a22 = m.at(2, 2), a23 = m.at(3, 2), a30 = m.at(0, 3), a31 = m.at(1, 3), a32 = m.at(2, 3), a33 = m.at(3, 3),

          b00 = a00 * a11 - a01 * a10, b01 = a00 * a12 - a02 * a10, b02 = a00 * a13 - a03 * a10, b03 = a01 * a12 - a02 * a11,
          b04 = a01 * a13 - a03 * a11, b05 = a02 * a13 - a03 * a12, b06 = a20 * a31 - a21 * a30, b07 = a20 * a32 - a22 * a30,
          b08 = a20 * a33 - a23 * a30, b09 = a21 * a32 - a22 * a31, b10 = a21 * a33 - a23 * a31, b11 = a22 * a33 - a23 * a32,

          det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

        return mat<T, S, S>{vec<T, S>{(a11* b11 - a12 * b10 + a13 * b09) / det, (a12* b08 - a10 * b11 - a13 * b07) / det,
          (a10* b10 - a11 * b08 + a13 * b06) / det, (a11* b07 - a10 * b09 - a12 * b06) / det},
          vec<T, S>{(a02* b10 - a01 * b11 - a03 * b09) / det, (a00* b11 - a02 * b08 + a03 * b07) / det,
          (a01* b08 - a00 * b10 - a03 * b06) / det, (a00* b09 - a01 * b07 + a02 * b06) / det},
          vec<T, S>{(a31* b05 - a32 * b04 + a33 * b03) / det, (a32* b02 - a30 * b05 - a33 * b01) / det,
          (a30* b04 - a31 * b02 + a33 * b00) / det, (a31* b01 - a30 * b03 - a32 * b00) / det},
          vec<T, S>{(a22* b04 - a21 * b05 - a23 * b03) / det, (a20* b05 - a22 * b02 + a23 * b01) / det,
          (a21* b02 - a20 * b04 - a23 * b00) / det, (a20* b03 - a21 * b01 + a22 * b00) / det}};
      }
      else
      {
        M mine = m;
        M inve;
        for (size_t col = 0; col < std::decay_t<M>::columns; ++col)
        {
          for (size_t row = col; row < std::decay_t<M>::rows; ++row)
          {
            if (row == col)
            {
              const auto inv_val = T(1.0 / mine.at(col, row));
              for (size_t set_col = 0; set_col < S; ++set_col) {
                inve.at(set_col, row) *= inv_val;
                mine.at(set_col, row) *= inv_val;
              }
            }
            else
            {
              if (const auto fac = mine.at(col, row))
              {
                for (size_t set_col = 0; set_col < S; ++set_col) {
                  inve.at(set_col, row) -= fac * inve.at(set_col, col);
                  mine.at(set_col, row) -= fac * mine.at(set_col, col);
                }
              }
            }
          }
        }
        for (ptrdiff_t scol = std::decay_t<M>::columns - 1; scol >= 0; --scol)
        {
          for (ptrdiff_t srow = std::decay_t<M>::rows - 1; srow >= scol + 1; --srow)
          {
            const auto col = static_cast<size_t>(scol);
            const auto row = static_cast<size_t>(srow);

            if (const auto fac = mine.at(col, row))
            {
              for (size_t set_col = 0; set_col < S; ++set_col) {
                inve.at(set_col, row) -= fac * inve.at(set_col, col);
                mine.at(set_col, row) -= fac * mine.at(set_col, col);
              }
            }
          }
        }

        return inve;
      }
    }

    template<matrix M>
    constexpr M& transpose_inplace(M& m) noexcept requires (std::decay_t<M>::is_square)
    {
      for (size_t c = 0; c < std::decay_t<M>::columns; ++c)
        for (size_t r = c + 1; r < std::decay_t<M>::rows; ++r)
          std::swap(m.at(c, r), m.at(r, c));
      return m;
    }
    template<matrix M>
    [[nodiscard]] constexpr auto transpose(M m) noexcept
    {
      mat<typename M::value_type, std::decay_t<M>::rows, std::decay_t<M>::columns> result;
      for (size_t c = 0; c < std::decay_t<M>::columns; ++c)
        for (size_t r = 0; r < std::decay_t<M>::rows; ++r)
          result.at(r, c) = m.at(c, r);
      return result;
    }

    template<matrix M, vector V>
    [[nodiscard]] constexpr auto operator*(M&& matrix, V&& vector)
      requires requires(M m, V v) { m[0][0] * v[0]; } &&
      (std::decay_t<M>::rows == std::decay_t<V>::count)
    {
      constexpr auto columns = std::decay_t<M>::columns;
      constexpr auto rows = std::decay_t<M>::rows;

      using result_type = decltype(matrix[0][0] * vector[0]);
      using result_vector = vec<result_type, rows>;

      result_vector result{ 0 };
      detail::traverse<columns, rows>([&](size_t c, size_t r) {
        result[r] += matrix[c][r] * vector[c];
        });
      return result;
    }

    template<vector V, matrix M>
    [[nodiscard]] constexpr auto operator*(V&& vector, M&& matrix)
      requires requires(M m, V v) { v[0] * m[0][0]; }&& std::decay_t<M>::is_square &&
      (std::decay_t<M>::rows == std::decay_t<V>::count)
    {
      return inverse(std::forward<M>(matrix)) * std::forward<V>(vector);
    }

    namespace detail {
      template<typename T>
      struct columns {
        static constexpr size_t value = 1;
      };
      template<typename T, size_t C, size_t R>
      struct columns<mat<T, C, R>> {
        static constexpr size_t value = C;
      };
      template<typename T, size_t R>
      struct columns<vec<T, R>> {
        static constexpr size_t value = 1;
      };
      template<typename T>
      static constexpr size_t columns_v = columns<std::decay_t<T>>::value;
      template<typename T>
      struct rows {
        static constexpr size_t value = 1;
      };
      template<typename T, size_t C, size_t R>
      struct rows<mat<T, C, R>> {
        static constexpr size_t value = R;
      };
      template<typename T, size_t R>
      struct rows<vec<T, R>> {
        static constexpr size_t value = R;
      };
      template<typename T>
      static constexpr size_t rows_v = rows<std::decay_t<T>>::value;

      template<typename Lhs, typename Rhs>
      static constexpr size_t common_columns = std::max(columns_v<Lhs>, columns_v<Rhs>);
      template<typename Lhs, typename Rhs>
      static constexpr size_t common_rows = std::max(rows_v<Lhs>, rows_v<Rhs>);

      template<typename... Ts>
      constexpr size_t max_columns() {
        size_t max = 0;
        ((max = max < detail::columns_v<Ts> ? detail::columns_v<Ts> : max), ...);
        return max;
      }
      template<typename... Ts>
      constexpr size_t max_rows() {
        size_t max = 0;
        ((max = max < detail::rows_v<Ts> ? detail::rows_v<Ts> : max), ...);
        return max;
      }

      template<typename... Ts>
      constexpr bool compatible_mat() {
        constexpr auto cols = max_columns<Ts...>();
        constexpr auto rows = max_rows<Ts...>();
        return (((columns_v<Ts> == 1 || columns_v<Ts> == cols) && (rows_v<Ts> == 1 || rows_v<Ts> == rows)) && ...);
      }
    }

    template<typename Fun, typename... Vs> requires (detail::compatible_mat<Vs...>()) && requires(Fun fun, Vs&&... vs) {
      fun(detail::get(vs, 0, 0)...);
    }
    constexpr auto apply_2d(Fun&& fun, Vs&&... vs)
    {
      constexpr auto cols = detail::max_columns<Vs...>();
      constexpr auto rows = detail::max_rows<Vs...>();

      using result_type = decltype(fun(detail::get(vs, 0, 0)...));

      return detail::traverse<cols, rows>([&](size_t c, size_t r) {
        return fun(detail::get(vs, c, r)...);
        });
    }

#define arithmetic_matrix_scalar_assign_op(Op, Ope) \
  template<matrix Lhs, scalar Rhs> \
  constexpr decltype(auto) operator Ope(Lhs& lhs, Rhs&& rhs) \
    requires requires(basic_scalar_t<Lhs>& lhs, basic_scalar_t<Rhs> rhs) { lhs Ope rhs; } \
  { \
    constexpr auto columns = std::decay_t<Lhs>::columns; \
    constexpr auto rows = std::decay_t<Lhs>::rows; \
    detail::traverse<columns, rows>([&](size_t c, size_t r) { lhs[c][r] Ope rhs; }); \
    return lhs; \
  } 

#define arithmetic_matrix_matrix_assign_op(Ope) \
  template<typename A, typename B, size_t C, size_t R> \
  constexpr decltype(auto) operator Ope(mat<A, C, R>& lhs, mat<B, C, R> const& rhs) \
    requires requires(A& lhs, B rhs) { lhs Ope rhs; } \
  { \
    using Lhs = mat<A, C, R>; \
    constexpr auto columns = std::decay_t<Lhs>::columns; \
    constexpr auto rows = std::decay_t<Lhs>::rows; \
    detail::traverse<columns, rows>([&](size_t c, size_t r) { lhs[c][r] Ope rhs[c][r]; }); \
    return lhs; \
  }

#define arithmetic_matrix_scalar_op(Op) \
  template<typename Lhs, typename Rhs> \
  constexpr auto operator Op(Lhs&& lhs, Rhs&& rhs) \
    requires ((matrix<Lhs> || matrix<Rhs>) && (scalar<Lhs> || scalar<Rhs>)) && requires(basic_scalar_t<Lhs> lhs, basic_scalar_t<Rhs> rhs) { lhs Op rhs; } \
  { \
    constexpr auto columns = detail::common_columns<Lhs, Rhs>; \
    constexpr auto rows = detail::common_rows<Lhs, Rhs>; \
    using result_type = decltype(std::declval<basic_scalar_t<Lhs>>() Op std::declval<basic_scalar_t<Rhs>>()); \
    mat<result_type, columns, rows> result;\
    detail::traverse<columns, rows>([&](size_t c, size_t r) { result[c][r] = detail::get(lhs, c, r) Op detail::get(rhs, c, r); }); \
    return result; \
  }

#define arithmetic_matrix_matrix_op(Op) \
  template<typename A, typename B, size_t C, size_t R> \
  constexpr auto operator Op(mat<A, C, R> const& lhs, mat<B, C, R> const& rhs) \
    requires requires(A lhs, B rhs) { lhs Op rhs; } \
  { \
    constexpr auto columns = C; \
    constexpr auto rows = R; \
    using result_type = decltype(std::declval<A>() Op std::declval<B>()); \
    mat<result_type, columns, rows> result;\
    detail::traverse<columns, rows>([&](size_t c, size_t r) { result[c][r] = lhs[c][r] Op rhs[c][r]; }); \
    return result; \
  }

#define unary_matrix_op(Op) \
  template<typename A, size_t C, size_t R> \
  constexpr auto operator Op(mat<A, C, R> const& lhs) \
    requires requires(A lhs) { Op lhs; } \
  { \
    constexpr auto columns = C; \
    constexpr auto rows = R; \
    using result_type = decltype(Op std::declval<A>()); \
    mat<result_type, columns, rows> result;\
    detail::traverse<columns, rows>([&](size_t c, size_t r) { result[c][r] = Op lhs[c][r]; }); \
    return result; \
  }

#define arithmetic_matrix_assign_op(Op) \
  arithmetic_matrix_scalar_assign_op(Op, Op=) \
  arithmetic_matrix_scalar_op(Op) \
  arithmetic_matrix_matrix_assign_op(Op=) \
  arithmetic_matrix_matrix_op(Op)

#define arithmetic_matrix_op(Op) \
  arithmetic_matrix_scalar_op(Op) \
  arithmetic_matrix_matrix_op(Op)

    arithmetic_matrix_assign_op(+);
    arithmetic_matrix_assign_op(-);

    arithmetic_matrix_scalar_assign_op(*, *=);
    arithmetic_matrix_scalar_assign_op(/ , /=);

    arithmetic_matrix_assign_op(%);
    arithmetic_matrix_assign_op(>> );
    arithmetic_matrix_assign_op(<< );
    arithmetic_matrix_assign_op(| );
    arithmetic_matrix_assign_op(&);
    arithmetic_matrix_assign_op(^);

    arithmetic_matrix_op(== );
    arithmetic_matrix_op(<= );
    arithmetic_matrix_op(>= );
    arithmetic_matrix_op(< );
    arithmetic_matrix_op(> );
    arithmetic_matrix_op(|| );
    arithmetic_matrix_op(&&);

    unary_matrix_op(!);
    unary_matrix_op(~);
    unary_matrix_op(-);
    unary_matrix_op(+);


#undef unary_matrix_op
#undef arithmetic_matrix_op
#undef arithmetic_matrix_matrix_op
#undef arithmetic_matrix_scalar_op
#undef arithmetic_matrix_assign_op
#undef arithmetic_matrix_scalar_assign_op
#undef arithmetic_matrix_matrix_assign_op

    template<typename Lhs, typename Rhs>
    constexpr auto operator !=(Lhs&& lhs, Rhs&& rhs)
      requires ((matrix<Lhs> || matrix<Rhs>) && (scalar<Lhs> || scalar<Rhs>)) && requires(Lhs lhs, Rhs rhs) { lhs == rhs; }
    {
      return !(lhs == rhs);
    }

    template<typename A, typename B, size_t C, size_t R>
    constexpr auto operator !=(mat<A, C, R> const& lhs, mat<B, C, R> const& rhs)
      requires requires(mat<A, C, R> lhs, mat<B, C, R> rhs) { lhs == rhs; }
    {
      return !(lhs == rhs);
    }
}