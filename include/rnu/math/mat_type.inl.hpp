
namespace rnu
{

  template<typename T, size_t Cols, size_t Rows>
  template<typename... Scalars>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::mat(Scalars&&... scalars) requires scalars_compatible<Scalars...>
    : m_linear_data{ static_cast<value_type>(scalars)... }
  {

  }

  template<typename T, size_t Cols, size_t Rows>
  template<matrix_type Other>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::mat(Other const& other) noexcept
    requires(Other::cols != cols || Other::rows != rows)
    : mat()
  {
    for (size_t c = 0; c < std::min(cols, Other::cols); ++c)
      for (size_t r = 0; r < std::min(rows, Other::rows); ++r)
        at(c, r) = other.at(c, r);
  }

  template<typename T, size_t Cols, size_t Rows>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::mat(T diag) noexcept
  {
    fill_diag(diag);
  }

  template<typename T, size_t Cols, size_t Rows>
  template<typename V>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::mat(V v) noexcept
    requires vector_matrix<V>
  {
    col(0) = std::move(v);
  }

  template<typename T, size_t Cols, size_t Rows>
  template<typename V>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::operator V& ()
    requires vector_matrix<V> {
    return col(0);
  }

  template<typename T, size_t Cols, size_t Rows>
  template<typename V>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::operator V const& () const
    requires vector_matrix<V> {
    return col(0);
  }

  template<typename T, size_t Cols, size_t Rows>
  constexpr T& mat<T, Cols, Rows>::at(size_t col, size_t row)
  {
    return m_data[col][row];
  }
  template<typename T, size_t Cols, size_t Rows>
  constexpr T const& mat<T, Cols, Rows>::at(size_t col, size_t row) const
  {
    return m_data[col][row];
  }
  template<typename T, size_t Cols, size_t Rows>
  constexpr typename mat<T, Cols, Rows>::column_type& mat<T, Cols, Rows>::col(size_t col)
  {
    return m_data[col];
  }
  template<typename T, size_t Cols, size_t Rows>
  constexpr typename mat<T, Cols, Rows>::column_type const& mat<T, Cols, Rows>::col(size_t col, size_t row) const
  {
    return m_data[col];
  }
  template<typename T, size_t Cols, size_t Rows>
  constexpr typename mat<T, Cols, Rows>::value_type& mat<T, Cols, Rows>::element(size_t linear_index)
  {
    return m_linear_data[linear_index];
  }
  template<typename T, size_t Cols, size_t Rows>
  constexpr typename mat<T, Cols, Rows>::value_type const& mat<T, Cols, Rows>::element(size_t linear_index) const
  {
    return m_linear_data[linear_index];
  }

  template<typename T, size_t Cols, size_t Rows>
  inline constexpr T* mat<T, Cols, Rows>::data() noexcept
  {
    return m_data[0].data();
  }

  template<typename T, size_t Cols, size_t Rows>
  inline constexpr T const* mat<T, Cols, Rows>::data() const noexcept
  {
    return m_data[0].data();
  }

  template<typename T, size_t Cols, size_t Rows>
  inline constexpr typename mat<T, Cols, Rows>::size_type mat<T, Cols, Rows>::size() const noexcept
  {
    return Cols * Rows;
  }

  template<typename T, size_t Cols, size_t Rows>
  void mat<T, Cols, Rows>::fill_diag(T value)
  {
    for (size_t i = 0; i < std::min(Cols, Rows); ++i)
      at(i, i) = value;
  }

  template<typename T, size_t Cols, size_t Rows>
  constexpr mat<T, Cols, Rows>::mat() noexcept
  {
    fill_diag(T(1));
  }

  template<typename T, size_t Cols, size_t Rows>
  constexpr mat<T, Cols, Rows>::mat(std::initializer_list<column_type> values) noexcept
  {
    auto output = m_data.begin();
    auto iter = std::begin(values);

    auto values_end = std::end(values);
    auto output_end = m_data.end();
    while (iter != values_end && output != output_end)
    {
      *(output++) = *(iter++);
    }
  }

  template<typename T, size_t Cols, size_t Rows>
  [[nodiscard]] constexpr mat<T, Cols, Rows>::operator bool() noexcept
    requires std::same_as<T, bool>
  {
    return std::any_of(std::cbegin(m_linear_data), std::cend(m_linear_data), [](auto v) { return v; });
  }
}