#pragma once

namespace gfx
{
    namespace detail {
        template<typename UnaryConvertFun, size_t... Is, typename T, size_t Min>
        constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, size_t maj, const vec<T, Min>& vector,
            UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t(), vector.components[0]));
            if constexpr (std::is_same_v<return_type, void>)
                (fun(maj, Is, vector.components[Is]), ...);
            else
                return vec<return_type, Min>(fun(maj, Is, vector.components[Is])...);
        }
        template<typename UnaryConvertFun, size_t... Is, typename T, size_t Maj, size_t Min>
        constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, const mat<T, Maj, Min>& matrix, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t(), matrix.components[0][0]));
            if constexpr (std::is_same_v<return_type, void>)
                (apply_for_each_indexed_impl(std::make_index_sequence<Min>{}, Is, matrix.components[Is], std::forward<UnaryConvertFun&&>(fun)),
                    ...);
            else
                return mat<return_type, Maj, Min>(apply_for_each_indexed_impl(std::make_index_sequence<Min>{}, Is, matrix.components[Is],
                    std::forward<UnaryConvertFun&&>(fun))...);
        }
        template<typename UnaryConvertFun, typename T, size_t Maj, size_t Min>
        constexpr auto apply_for_each_indexed(const mat<T, Maj, Min>& matrix, UnaryConvertFun&& fun) noexcept
        {
            return apply_for_each_indexed_impl(std::make_index_sequence<Maj>{}, matrix, std::forward<UnaryConvertFun&&>(fun));
        }

        template<typename T, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, size_t maj, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (fun(maj, Is), ...);
            else
                return vec<return_type, Min>(fun(maj, Is)...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (apply_for_each_indexed_impl<T, Min>(std::make_index_sequence<Min>{}, Is, std::forward<UnaryConvertFun&&>(fun)), ...);
            else
                return mat<return_type, Maj, Min>(
                    apply_for_each_indexed_impl<T, Min>(std::make_index_sequence<Min>{}, Is, std::forward<UnaryConvertFun&&>(fun))...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun>
        constexpr auto apply_for_each_indexed(UnaryConvertFun&& fun) noexcept
        {
            return apply_for_each_indexed_impl<T, Maj, Min>(std::make_index_sequence<Maj>{}, std::forward<UnaryConvertFun&&>(fun));
        }


        template<typename T, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_half_impl(std::index_sequence<Is...>, size_t maj, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (fun(maj, maj + Is + 1), ...);
            else
                return vec<return_type, Min>(fun(maj, maj + Is + 1)...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_half_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (apply_for_each_indexed_half_impl<T, Min>(std::make_index_sequence<Min - Is - 1>{}, Is, std::forward<UnaryConvertFun&&>(fun)), ...);
            else
                return mat<return_type, Maj, Min>(apply_for_each_indexed_half_impl<T, Min>(std::make_index_sequence<Min - Is - 1>{}, Is,
                    std::forward<UnaryConvertFun&&>(fun))...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun>
        constexpr auto apply_for_each_half_indexed(UnaryConvertFun&& fun) noexcept
        {
            return apply_for_each_indexed_half_impl<T, Maj, Min>(std::make_index_sequence<Maj>{}, std::forward<UnaryConvertFun&&>(fun));
        }


        template<typename T, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_half_bw_impl(std::index_sequence<Is...>, size_t maj, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (fun(maj, maj - 1 - Is), ...);
            else
                return vec<return_type, Min>(fun(maj, maj - 1 - Is)...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_half_bw_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (apply_for_each_indexed_half_bw_impl<T, Min>(std::make_index_sequence<Min - Is - 1>{}, Maj - 1 - Is, std::forward<UnaryConvertFun&&>(fun)), ...);
            else
                return mat<return_type, Maj, Min>(apply_for_each_indexed_half_bw_impl<T, Min>(std::make_index_sequence<Min - Is - 1>{}, Maj - 1 - Is,
                    std::forward<UnaryConvertFun&&>(fun))...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun>
        constexpr auto apply_for_each_half_bw_indexed(UnaryConvertFun&& fun) noexcept
        {
            return apply_for_each_indexed_half_bw_impl<T, Maj, Min>(std::make_index_sequence<Maj>{}, std::forward<UnaryConvertFun&&>(fun));
        }


        template<typename T, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_half_incl_impl(std::index_sequence<Is...>, size_t maj, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (fun(maj, maj + Is), ...);
            else
                return vec<return_type, Min>(fun(maj, maj + Is)...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun, size_t... Is>
        constexpr auto apply_for_each_indexed_half_incl_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
        {
            using return_type = decltype(fun(size_t(), size_t()));
            if constexpr (std::is_same_v<return_type, void>)
                (apply_for_each_indexed_half_incl_impl<T, Min>(std::make_index_sequence<Min - Is>{}, Is, std::forward<UnaryConvertFun&&>(fun)), ...);
            else
                return mat<return_type, Maj, Min>(apply_for_each_indexed_half_incl_impl<T, Min>(std::make_index_sequence<Min - Is>{}, Is,
                    std::forward<UnaryConvertFun&&>(fun))...);
        }
        template<typename T, size_t Maj, size_t Min, typename UnaryConvertFun>
        constexpr auto apply_for_each_half_incl_indexed(UnaryConvertFun&& fun) noexcept
        {
            return apply_for_each_indexed_half_incl_impl<T, Maj, Min>(std::make_index_sequence<Maj>{}, std::forward<UnaryConvertFun&&>(fun));
        }
    }    // namespace detail
}