
#pragma once

namespace std {
template<typename T, size_t S>
constexpr auto real(const gfx::vec<T, S>& a) noexcept
{
    return a.real();
}
template<typename T, size_t S>
constexpr auto imag(const gfx::vec<T, S>& a) noexcept
{
    return a.imag();
}
template<typename T, size_t S>
constexpr auto abs(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::abs(val); });
}
template<typename T, size_t S>
constexpr auto arg(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::arg(val); });
}
template<typename T, size_t S>
constexpr auto norm(const gfx::vec<T, S>& a) noexcept
{
    return gfx::dot(a, a);
}
template<typename T, size_t S>
constexpr auto conj(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::conj(val); });
}
template<typename T, size_t S>
constexpr auto proj(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::proj(val); });
}
template<typename T, size_t S>
constexpr auto exp(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::exp(val); });
}
template<typename T, size_t S>
constexpr auto exp2(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::exp2(val); });
}
template<typename T, size_t S>
constexpr auto expm1(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::expm1(val); });
}
template<typename T, size_t S>
constexpr auto log(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log(val); });
}
template<typename T, size_t S>
constexpr auto log2(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log2(val); });
}
template<typename T, size_t S>
constexpr auto log10(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log10(val); });
}
template<typename T, size_t S>
constexpr auto log1p(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::log1p(val); });
}
template<typename T, size_t S>
constexpr auto pow(const gfx::vec<T, S>& a, const T& exponent) noexcept
{
    return gfx::detail::apply_for_each(a, [&](const auto& val) { return std::pow(val, exponent); });
}
template<typename T, size_t S>
constexpr auto pow(const gfx::vec<T, S>& a, const gfx::vec<T, S>& exponent) noexcept
{
    return gfx::detail::apply_for_each(a, exponent, [&](const auto& val, const auto& ex) { return std::pow(val, ex); });
}
template<typename T, size_t S>
constexpr auto modf(const gfx::vec<T, S>& a, const T& divisor) noexcept
{
	return gfx::detail::apply_for_each(a, [&](const auto& val) { return std::modf(val, divisor); });
}
template<typename T, size_t S>
constexpr auto modf(const gfx::vec<T, S>& a, const gfx::vec<T, S>& divisor) noexcept
{
	return gfx::detail::apply_for_each(a, divisor, [&](const auto& val, const auto& ex) { return std::modf(val, ex); });
}
template<typename T, size_t S>
constexpr auto sqrt(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::sqrt(val); });
}
template<typename T, size_t S>
constexpr auto cqrt(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::cqrt(val); });
}
template<typename T, size_t S>
constexpr auto hypot(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::hypot(va, vb); });
}
template<typename T, size_t S>
constexpr auto hypot(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b, const gfx::vec<T, S>& c) noexcept
{
    return gfx::detail::apply_for_each(a, b, c, [](const auto& va, const auto& vb, const auto& vc) { return std::hypot(va, vb, vc); });
}
template<typename T, size_t S>
constexpr auto sin(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::sin(val); });
}
template<typename T, size_t S>
constexpr auto cos(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::cos(val); });
}
template<typename T, size_t S>
constexpr auto tan(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::tan(val); });
}
template<typename T, size_t S>
constexpr auto asin(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::asin(val); });
}
template<typename T, size_t S>
constexpr auto acos(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::acos(val); });
}
template<typename T, size_t S>
constexpr auto atan(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::atan(val); });
}
template<typename T, size_t S>
constexpr auto sinh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::sinh(val); });
}
template<typename T, size_t S>
constexpr auto cosh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::cosh(val); });
}
template<typename T, size_t S>
constexpr auto tanh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::tanh(val); });
}
template<typename T, size_t S>
constexpr auto asinh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::asinh(val); });
}
template<typename T, size_t S>
constexpr auto acosh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::acosh(val); });
}
template<typename T, size_t S>
constexpr auto atanh(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::atanh(val); });
}
template<typename T, size_t S>
constexpr auto atan2(const gfx::vec<T, S>& a, const T& b) noexcept
{
    return gfx::detail::apply_for_each(a, [&](const auto& va) { return std::atan2(va, b); });
}
template<typename T, size_t S>
constexpr auto atan2(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::atan2(va, vb); });
}
template<typename T, size_t S>
constexpr auto ceil(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::ceil(val); });
}
template<typename T, size_t S>
constexpr auto floor(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::floor(val); });
}
template<typename T, size_t S>
constexpr auto round(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::round(val); });
}
template<typename T, size_t S>
constexpr auto isfinite(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isfinite(val); });
}
template<typename T, size_t S>
constexpr auto isinf(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isinf(val); });
}
template<typename T, size_t S>
constexpr auto isnan(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isnan(val); });
}
template<typename T, size_t S>
constexpr auto isnormal(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::isnormal(val); });
}
template<typename T, size_t S>
constexpr auto signbit(const gfx::vec<T, S>& a) noexcept
{
    return gfx::detail::apply_for_each(a, [](const auto& val) { return std::signbit(val); });
}
template<typename T, size_t S>
constexpr auto isgreater(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isgreater(va, vb); });
}
template<typename T, size_t S>
constexpr auto isgreaterequal(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isgreaterequal(va, vb); });
}
template<typename T, size_t S>
constexpr auto isless(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isless(va, vb); });
}
template<typename T, size_t S>
constexpr auto islessequal(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::islessequal(va, vb); });
}
template<typename T, size_t S>
constexpr auto islessgreater(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::islessgreater(va, vb); });
}
template<typename T, size_t S>
constexpr auto isunordered(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::isunordered(va, vb); });
}
template<typename T, size_t S>
constexpr auto max(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::max(va, vb); });
}
template<typename T, size_t S>
constexpr auto min(const gfx::vec<T, S>& a, const gfx::vec<T, S>& b) noexcept
{
    return gfx::detail::apply_for_each(a, b, [](const auto& va, const auto& vb) { return std::min(va, vb); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const gfx::vec<T, S>& min, const gfx::vec<T, S>& max) noexcept
{
    return gfx::detail::apply_for_each(a, min, max,
                                       [](const auto& va, const auto& vmin, const auto& vmax) { return std::clamp(va, vmin, vmax); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const T& min, const gfx::vec<T, S>& max) noexcept
{
    return gfx::detail::apply_for_each(a, max, [&](const auto& va, const auto& vmax) { return std::clamp(va, min, vmax); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const gfx::vec<T, S>& min, const T& max) noexcept
{
    return gfx::detail::apply_for_each(a, min, [&](const auto& va, const auto& vmin) { return std::clamp(va, vmin, max); });
}
template<typename T, size_t S>
constexpr auto clamp(const gfx::vec<T, S>& a, const T& min, const T& max) noexcept
{
    return gfx::detail::apply_for_each(a, [&](const auto& va) { return std::clamp(va, min, max); });
}
}    // namespace std
namespace gfx {
using std::abs;
using std::acos;
using std::acosh;
using std::arg;
using std::asin;
using std::asinh;
using std::atan;
using std::atanh;
using std::ceil;
using std::clamp;
using std::conj;
using std::cos;
using std::cosh;
using std::cqrt;
using std::exp;
using std::exp2;
using std::expm1;
using std::floor;
using std::hypot;
using std::imag;
using std::isfinite;
using std::isgreater;
using std::isgreaterequal;
using std::isinf;
using std::isless;
using std::islessequal;
using std::islessgreater;
using std::isnan;
using std::isnormal;
using std::isunordered;
using std::log;
using std::log10;
using std::log1p;
using std::log2;
using std::max;
using std::min;
using std::norm;
using std::pow;
using std::proj;
using std::real;
using std::round;
using std::signbit;
using std::sin;
using std::sinh;
using std::sqrt;
using std::tan;
using std::tanh;
}    // namespace gfx
