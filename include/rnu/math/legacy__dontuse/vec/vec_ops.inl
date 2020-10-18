//#pragma once
//
//namespace gfx {
//
//#define logic_op(OP) \
//	template<typename T1, typename T2, size_t S, typename = decltype(std::declval<T1>() OP std::declval<T2>())> \
//	constexpr vec<std::decay_t<decltype(std::declval<T1>() OP std::declval<T2>())>, S> operator OP(const vec<T1, S>& v1, const vec<T2, S>& v2) noexcept \
//	{ \
//		return detail::apply_for_each(v1, v2, [](const auto& x, const auto& y) { return x OP y; }); \
//	} \
//	template<typename T1, size_t S, typename = decltype(std::declval<T1>() OP std::declval<T1>())> \
//	constexpr vec<std::decay_t<decltype(std::declval<T1>() OP std::declval<T1>())>, S> operator OP(const vec<T1, S>& v1, const T1& v2) noexcept \
//	{ \
//		return detail::apply_for_each(v1, [&](const auto& x) { return x OP v2; }); \
//	} \
//	template<typename T2, size_t S, typename = decltype(std::declval<T2>() OP std::declval<T2>())> \
//	constexpr vec<std::decay_t<decltype(std::declval<T2>() OP std::declval<T2>())>, S> operator OP(const T2& v1, const vec<T2, S>& v2) noexcept \
//	{ \
//		return detail::apply_for_each(v2, [&](const auto& y) { return v1 OP y; }); \
//	} 
//
//logic_op(==)
//logic_op(!=)
//logic_op(<=)
//logic_op(>=)
//logic_op(<)
//logic_op(>)
//logic_op(^)
//logic_op(|)
//logic_op(&)
//logic_op(||)
//logic_op(&&)
//logic_op(+)
//logic_op(-)
//logic_op(*)
//logic_op(/)
//logic_op(%)
//logic_op(<<)
//logic_op(>>)
//
//#undef logic_op
//
//#define assign_op(OP) \
//	template<typename T1, typename T2, size_t S, typename = decltype(std::declval<T1>() OP std::declval<T2>())> \
//	constexpr vec<T1, S>& operator OP(vec<T1, S>& v1, const vec<T2, S>& v2) noexcept \
//	{ \
//		detail::apply_for_each(v1, v2, [](const auto& x, const auto& y) { x OP y; }); \
//		return v1; \
//	} \
//	template<typename T1, typename T2, size_t S, typename = decltype(std::declval<T1>() OP std::declval<T2>())> \
//	constexpr vec<T1, S>& operator OP(vec<T1, S>& v1, const T2& v2) noexcept \
//	{ \
//		detail::apply_for_each(v1, [&](const auto& x) { x OP v2; }); \
//		return v1; \
//	}
//
//assign_op(&=)
//assign_op(|=)
//assign_op(^=)
//assign_op(+=)
//assign_op(-=)
//assign_op(*=)
//assign_op(/=)
//assign_op(%=)
//assign_op(<<= )
//assign_op(>>= )
//
//#undef assign_op
//
//#define unary_op(OP, Pre) \
//	template<typename T1, size_t S, typename = decltype(OP std::declval<T1>())> \
//	constexpr auto operator OP(Pre vec<T1, S>& v1) noexcept \
//	{ \
//		return detail::apply_for_each(v1, [](Pre auto& x) { return OP x; }); \
//	}
//
//unary_op(*, const)
//unary_op(!, const)
//unary_op(-, const)
//unary_op(+, const)
//unary_op(~, const)
//
//#undef unary_op
//
//template<typename T1, size_t S, typename = decltype(++ std::declval<T1>())>
//constexpr auto operator++(vec<T1, S>& v1) noexcept
//{
//	detail::apply_for_each(v1, [](auto& x) { ++ x; });
//	return v1;
//}
//template<typename T1, size_t S, typename = decltype(--std::declval<T1>())>
//constexpr auto operator--(vec<T1, S>& v1) noexcept
//{
//	detail::apply_for_each(v1, [](auto& x) { --x; });
//	return v1;
//}
//template<typename T1, size_t S, typename = decltype(std::declval<T1>()++)>
//constexpr auto operator++(vec<T1, S>& v1, int) noexcept
//{
//	return detail::apply_for_each(v1, [](auto& x) { return x++; });
//}
//template<typename T1, size_t S, typename = decltype(std::declval<T1>()--)>
//constexpr auto operator--(vec<T1, S>& v1, int) noexcept
//{
//	return detail::apply_for_each(v1, [](auto& x) { return x--; });
//}
//
//}    // namespace gfx