#pragma once

namespace gfx {

#define logic_op(OP) \
	template<typename T1, size_t Maj, size_t Min, typename = decltype(std::declval<T1>() OP std::declval<T1>())> \
	constexpr mat<std::decay_t<decltype(std::declval<T1>() OP std::declval<T1>())>, Min, Maj> operator OP(const mat<T1, Min, Maj>& v1, const T1& v2) noexcept \
	{ \
		return detail::apply_for_each(v1, [&](const auto& x) { return x OP v2; }); \
	} \
	template<typename T2, size_t Maj, size_t Min, typename = decltype(std::declval<T2>() OP std::declval<T2>())> \
	constexpr mat<std::decay_t<decltype(std::declval<T2>() OP std::declval<T2>())>, Min, Maj> operator OP(const T2& v1, const mat<T2, Min, Maj>& v2) noexcept \
	{ \
		return detail::apply_for_each(v2, [&](const auto& y) { return v1 OP y; }); \
	} 

#define logic_op_direct(OP) \
	template<typename T1, typename T2, size_t Maj, size_t Min, typename = decltype(std::declval<T1>() OP std::declval<T2>())> \
	constexpr mat<std::decay_t<decltype(std::declval<T1>() OP std::declval<T2>())>, Min, Maj> operator OP(const mat<T1, Min, Maj>& v1, const mat<T2, Min, Maj>& v2) noexcept \
	{ \
		return detail::apply_for_each(v1, v2, [](const auto& x, const auto& y) { return x OP y; }); \
	} 

logic_op(==)
logic_op(!=)
logic_op(<=)
logic_op(>=)
logic_op(<)
logic_op(>)
logic_op(^)
logic_op(|)
logic_op(&)
logic_op(||)
logic_op(&&)
logic_op(+)
logic_op(-)
logic_op(*)
logic_op(/)
logic_op(%)
logic_op(<<)
logic_op(>>)

#undef logic_op

logic_op_direct(==)
logic_op_direct(!=)
logic_op_direct(<=)
logic_op_direct(>=)
logic_op_direct(<)
logic_op_direct(>)
logic_op_direct(+)
logic_op_direct(-)
logic_op_direct(|)
logic_op_direct(&)
logic_op_direct(^)
logic_op_direct(||)
logic_op_direct(&&)
logic_op_direct(>>)
logic_op_direct(<<)

#undef logic_op_direct

#define assign_op(OP) \
	template<typename T1, typename T2, size_t Maj, size_t Min, typename = decltype(std::declval<T1>() OP std::declval<T2>())> \
	constexpr mat<T1, Maj, Min>& operator OP(mat<T1, Maj, Min>& v1, const T2& v2) noexcept \
	{ \
		detail::apply_for_each(v1, [&](const auto& x) { x OP v2; }); \
		return v1; \
	}

assign_op(&=)
assign_op(|=)
assign_op(^=)
assign_op(+=)
assign_op(-=)
assign_op(*=)
assign_op(/=)
assign_op(%=)
assign_op(<<= )
assign_op(>>= )

#undef assign_op

#define unary_op(OP, Pre) \
	template<typename T1, size_t Maj, size_t Min, typename = decltype(OP std::declval<T1>())> \
	constexpr auto operator OP(Pre mat<T1, Maj, Min>& v1) noexcept \
	{ \
		return detail::apply_for_each(v1, [](Pre auto& x) { return OP x; }); \
	}

unary_op(*, const)
unary_op(!, const)
unary_op(-, const)
unary_op(+, const)
unary_op(~, const)

#undef unary_op

template<typename T1, size_t Maj, size_t Min, typename = decltype(++ std::declval<T1>())>
constexpr auto operator++(mat<T1, Maj, Min>& v1) noexcept
{
	detail::apply_for_each(v1, [](auto& x) { ++ x; });
	return v1;
}
template<typename T1, size_t Maj, size_t Min, typename = decltype(--std::declval<T1>())>
constexpr auto operator--(mat<T1, Maj, Min>& v1) noexcept
{
	detail::apply_for_each(v1, [](auto& x) { --x; });
	return v1;
}
template<typename T1, size_t Maj, size_t Min, typename = decltype(std::declval<T1>()++)>
constexpr auto operator++(mat<T1, Maj, Min>& v1, int) noexcept
{
	return detail::apply_for_each(v1, [](auto& x) { return x++; });
}
template<typename T1, size_t Maj, size_t Min, typename = decltype(std::declval<T1>()--)>
constexpr auto operator--(mat<T1, Maj, Min>& v1, int) noexcept
{
	return detail::apply_for_each(v1, [](auto& x) { return x--; });
}

}    // namespace gfx