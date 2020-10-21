#pragma once

namespace rnu {
namespace detail {
template<typename T, std::size_t... Is>
constexpr T dot_impl(std::index_sequence<Is...>, const T* s1, const T* s2) noexcept
{
    return ((s1[Is] * s2[Is]) + ...);
}
//
//template<typename UnaryConvertFun, typename T, size_t S, std::size_t... Is>
//constexpr auto apply_for_each_impl(std::index_sequence<Is...>, UnaryConvertFun&& fun) noexcept
//{
//    using return_type = decltype(fun(size_t{}));
//    return vec<return_type, S>(fun(Is)...);
//}
//template<typename T, size_t S, typename UnaryConvertFun>
//constexpr auto apply_for_each(UnaryConvertFun&& fun) noexcept
//{
//	if constexpr(S <= max_static_vector_size)
//		return apply_for_each_impl<UnaryConvertFun, T, S>(std::make_index_sequence<S>{}, std::forward<UnaryConvertFun&&>(fun));
//	else
//	{
//		vec<decltype(fun(size_t{})), S> result;
//
//		for (size_t i = 0; i < S; ++i)
//			result[i] = fun(i);
//		return result;
//	}
//}
//
//template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
//constexpr auto apply_for_each_impl(std::index_sequence<Is...>, const vec<T, S>& vector, UnaryConvertFun&& fun) noexcept
//{
//    using return_type = decltype(fun(vector.components[0]));
//    return vec<return_type, S>(fun(vector.components[Is])...);
//}
//template<typename UnaryConvertFun, typename T, size_t S>
//constexpr auto apply_for_each(const vec<T, S>& vector, UnaryConvertFun&& fun) noexcept
//{
//	if constexpr (S <= max_static_vector_size)
//		return apply_for_each_impl(std::make_index_sequence<S>{}, vector, fun);
//	else
//	{
//		vec<decltype(fun(size_t{})), S> result;
//
//		for (size_t i = 0; i < S; ++i)
//			result[i] = fun(vector[i]);
//		return result;
//	}
//}
//template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
//constexpr decltype(auto) apply_for_each_impl(std::index_sequence<Is...>, vec<T, S>& vector, UnaryConvertFun&& fun) noexcept
//{
//    using return_type = decltype(fun(vector.components[0]));
//	vector = { fun(vector.components[Is])... };
//    return vector;
//}
//template<typename UnaryConvertFun, typename T, size_t S>
//constexpr decltype(auto) apply_for_each(vec<T, S>& vector, UnaryConvertFun&& fun) noexcept
//{
//	if constexpr (S <= max_static_vector_size)
//		return apply_for_each_impl(std::make_index_sequence<S>{}, vector, fun);
//	else
//	{
//		vec<decltype(fun(size_t{})), S> result;
//
//		for (size_t i = 0; i < S; ++i)
//			result[i] = fun(vector[i]);
//		vector = result;
//		return vector;
//	}
//}
//template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
//constexpr auto apply_for_each_impl(std::index_sequence<Is...>, const vec<T, S>& vector1, const vec<T, S>& vector2,
//                                   UnaryConvertFun&& fun) noexcept
//{
//    using return_type = decltype(fun(vector1.components[0], vector2.components[0]));
//    return vec<return_type, S>(fun(vector1.components[Is], vector2.components[Is])...);
//}
//template<typename UnaryConvertFun, typename T, size_t S>
//constexpr auto apply_for_each(const vec<T, S>& vector1, const vec<T, S>& vector2, UnaryConvertFun&& fun) noexcept
//{
//	if constexpr (S <= max_static_vector_size)
//		return apply_for_each_impl(std::make_index_sequence<S>{}, vector1, vector2, fun);
//	else
//	{
//		vec<decltype(fun(vector1.components[0], vector2.components[0])), S> result;
//
//		for (size_t i = 0; i < S; ++i)
//			result[i] = fun(vector1[i], vector2[i]);
//		return result;
//	}
//}
//
//
//template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
//constexpr decltype(auto) apply_for_each_impl(std::index_sequence<Is...>, vec<T, S>& vector1, const vec<T, S>& vector2, UnaryConvertFun&& fun) noexcept
//{
//    using return_type = decltype(fun(vector1.components[0], vector2.components[0]));
//	vector1 = { fun(vector1.components[Is], vector2.components[Is])... };
//	return vector1;
//}
//template<typename UnaryConvertFun, typename T, size_t S>
//constexpr decltype(auto) apply_for_each(vec<T, S>& vector1, const vec<T, S>& vector2, UnaryConvertFun&& fun) noexcept
//{
//	if constexpr (S <= max_static_vector_size)
//		return apply_for_each_impl(std::make_index_sequence<S>{}, vector1, vector2, fun);
//	else
//	{
//		vec<decltype(fun(vector1.components[0], vector2.components[0])), S> result;
//
//		for (size_t i = 0; i < S; ++i)
//			result[i] = fun(vector1[i], vector2[i]);
//		vector1 = result;
//		return vector1;
//	}
//}
//
//template<std::size_t... Is, typename UnaryConvertFun, typename T, size_t S>
//constexpr auto apply_for_each_impl(std::index_sequence<Is...>, const vec<T, S>& vector1, const vec<T, S>& vector2, const vec<T, S>& vector3,
//                                   UnaryConvertFun&& fun) noexcept
//{
//    using return_type = decltype(fun(vector1.components[0], vector2.components[0], vector3.components[0]));
//    return vec<return_type, S>(fun(vector1.components[Is], vector2.components[Is], vector3.components[Is])...);
//}
//template<typename UnaryConvertFun, typename T, size_t S>
//constexpr auto apply_for_each(const vec<T, S>& vector1, const vec<T, S>& vector2, const vec<T, S>& vector3, UnaryConvertFun&& fun) noexcept
//{
//	if constexpr (S <= max_static_vector_size)
//		return apply_for_each_impl(std::make_index_sequence<S>{}, vector1, vector2, vector3, fun);
//	else
//	{
//		using return_type = decltype(fun(vector1.components[0], vector2.components[0], vector3.components[0]));
//		vec<return_type, S> result;
//
//		for (size_t i = 0; i < S; ++i)
//			result[i] = fun(vector1[i], vector2[i], vector3[i]);
//		return result;
//	}
//}
}    // namespace detail
}    // namespace rnu