#pragma once

#include <array>
#include <algorithm>
#include "vec_type.hpp"

namespace rnu
{
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

        [[nodiscard]] constexpr mat() noexcept;
        [[nodiscard]] constexpr mat(std::initializer_list<T> values) noexcept;
        [[nodiscard]] constexpr mat(std::initializer_list<column_type> values) noexcept;

        template<matrix_type Other>
        [[nodiscard]] constexpr mat(Other const& other) noexcept
            requires(Other::cols != cols || Other::rows != rows)
            : mat()
        {
            for (size_t c = 0; c < std::min(cols, Other::cols); ++c)
            {
                for (size_t r = 0; r < std::min(rows, Other::rows); ++r)
                {
                    at(c, r) = other.at(c, r);
                }
            }
        }

        template<vector_type V>
        [[nodiscard]] explicit constexpr mat(V v) noexcept
            requires(std::min(Cols, Rows) == 1 && std::max(Cols, Rows) == V::component_count)
        {
            reinterpret_cast<V&>(*this) = v;
        }

        template<vector_type V>
        requires(std::min(Cols, Rows) == 1 && std::max(Cols, Rows) == V::component_count)
            [[nodiscard]] explicit constexpr operator V&() {
            return reinterpret_cast<V&>(*this);
        }
        template<vector_type V>
        requires(std::min(Cols, Rows) == 1 && std::max(Cols, Rows) == V::component_count)
            [[nodiscard]] explicit constexpr operator V const&() const {
            return reinterpret_cast<V const&>(*this);
        }

        [[nodiscard]] constexpr T& at(size_t col, size_t row);
        [[nodiscard]] constexpr T const& at(size_t col, size_t row) const;

        [[nodiscard]] constexpr T* data() noexcept;
        [[nodiscard]] constexpr T const* data() const noexcept;
        [[nodiscard]] constexpr size_type size() const noexcept;

        [[nodiscard]] constexpr operator bool() noexcept
            requires std::same_as<T, bool>
        {
            return std::any_of(data(), data() + size(), [](auto v) { return v; });
        }

    private:
        void fill_diag(T value);

        std::array<column_type, Cols> m_data{};
    };


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
    constexpr mat<T, Cols, Rows>::mat(std::initializer_list<T> values) noexcept
    {
        T* output = data();
        auto iter = std::begin(values);

        auto values_end = std::end(values);
        auto output_end = data() + size();
        while (iter != values_end && output != output_end)
        {
            *(output++) = *(iter++);
        }
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
}

#include "mat_apply.inl.hpp"
#include "mat_op.inl.hpp"
#include "mat_math.inl.hpp"