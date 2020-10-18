#pragma once

namespace gfx {
template<typename T, size_t S>
constexpr void transpose(mat<T, S, S>& m) noexcept
{
    detail::apply_for_each_half_indexed<T, S, S>([&](size_t col, size_t row) { std::swap(m[col][row], m[row][col]); });
}

template<typename T, size_t Maj, size_t Min>
constexpr mat<T, Min, Maj> transposed(const mat<T, Maj, Min>& in) noexcept
{
    const auto x = detail::apply_for_each_indexed<T, Min, Maj>([&](size_t col, size_t row) { return in[row][col]; });
    return x;
}
template<typename T, size_t S>
constexpr mat<T, S, S> transposed(const mat<T, S, S>& in) noexcept
{
    const auto x = detail::apply_for_each_indexed<T, S, S>([&](size_t col, size_t row) { return in[row][col]; });
    return x;
}

template<typename T, size_t S>
constexpr mat<T, S, S> inverse(const mat<T, S, S>& m) noexcept
{
	if constexpr (S == 1)
		return { static_cast<T>(1.0 / m[0][0]) };
	else if constexpr (S == 2)
	{
		const auto d = det(m);
		return mat<T, S, S>{
			vec<T, S>{m[1][1] / d, m[0][1] / d},
				vec<T, S>{m[1][0] / d, m[0][0] / d},
		};
	}
	else if constexpr (S == 3)
	{
		float a00 = m[0][0], a01 = m[1][0], a02 = m[2][0];
		float a10 = m[0][1], a11 = m[1][1], a12 = m[2][1];
		float a20 = m[0][2], a21 = m[1][2], a22 = m[2][2];

		float b01 = a22 * a11 - a12 * a21;
		float b11 = -a22 * a10 + a12 * a20;
		float b21 = a21 * a10 - a11 * a20;

		float d = a00 * b01 + a01 * b11 + a02 * b21;

		return vec<vec<T, S>, S>{vec<T, S>{b01 / d, b11 / d, b21 / d},
			vec<T, S>{(-a22 * a01 + a02 * a21) / d, (a22 * a00 - a02 * a20) / d, (-a21 * a00 + a01 * a20) / d},
			vec<T, S>{(a12 * a01 - a02 * a11) / d, (-a12 * a00 + a02 * a10) / d, (a11 * a00 - a01 * a10) / d}};
	}
	else if constexpr (S == 4)
	{
		float a00 = m[0][0], a01 = m[1][0], a02 = m[2][0], a03 = m[3][0], a10 = m[0][1], a11 = m[1][1], a12 = m[2][1], a13 = m[3][1],
			a20 = m[0][2], a21 = m[1][2], a22 = m[2][2], a23 = m[3][2], a30 = m[0][3], a31 = m[1][3], a32 = m[2][3], a33 = m[3][3],

			b00 = a00 * a11 - a01 * a10, b01 = a00 * a12 - a02 * a10, b02 = a00 * a13 - a03 * a10, b03 = a01 * a12 - a02 * a11,
			b04 = a01 * a13 - a03 * a11, b05 = a02 * a13 - a03 * a12, b06 = a20 * a31 - a21 * a30, b07 = a20 * a32 - a22 * a30,
			b08 = a20 * a33 - a23 * a30, b09 = a21 * a32 - a22 * a31, b10 = a21 * a33 - a23 * a31, b11 = a22 * a33 - a23 * a32,

			det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

		return vec<vec<T, S>, S>{vec<T, S>{(a11 * b11 - a12 * b10 + a13 * b09) / det, (a12 * b08 - a10 * b11 - a13 * b07) / det,
			(a10 * b10 - a11 * b08 + a13 * b06) / det, (a11 * b07 - a10 * b09 - a12 * b06) / det},
			vec<T, S>{(a02 * b10 - a01 * b11 - a03 * b09) / det, (a00 * b11 - a02 * b08 + a03 * b07) / det,
			(a01 * b08 - a00 * b10 - a03 * b06) / det, (a00 * b09 - a01 * b07 + a02 * b06) / det},
			vec<T, S>{(a31 * b05 - a32 * b04 + a33 * b03) / det, (a32 * b02 - a30 * b05 - a33 * b01) / det,
			(a30 * b04 - a31 * b02 + a33 * b00) / det, (a31 * b01 - a30 * b03 - a32 * b00) / det},
			vec<T, S>{(a22 * b04 - a21 * b05 - a23 * b03) / det, (a20 * b05 - a22 * b02 + a23 * b01) / det,
			(a21 * b02 - a20 * b04 - a23 * b00) / det, (a20 * b03 - a21 * b01 + a22 * b00) / det}};
	}
	else
	{
		gfx::mat<T, S> mine = m;
		gfx::mat<T, S> inve(gfx::identity);
		gfx::detail::apply_for_each_half_incl_indexed<T, S, S>([&](size_t col, size_t row)
		{
			if (row == col)
			{
				const auto inv_val = T(1.0 / mine[col][row]);
				gfx::detail::apply_for_each_indexed<T, S, 1>([&](size_t set_col, size_t)
				{
					inve[set_col][row] *= inv_val;
					mine[set_col][row] *= inv_val;
				});
			}
			else
			{
				if (const auto fac = mine[col][row])
				{
					gfx::detail::apply_for_each_indexed<T, S, 1>([&](size_t set_col, size_t)
					{
						inve[set_col][row] -= fac * inve[set_col][col];
						mine[set_col][row] -= fac * mine[set_col][col];
					});
				}
			}
		});

		gfx::detail::apply_for_each_half_bw_indexed<T, S, S>([&](size_t col, size_t row)
		{
			if (const auto fac = mine[col][row])
			{
				gfx::detail::apply_for_each_indexed<T, S, 1>([&](size_t set_col, size_t)
				{
					inve[set_col][row] -= fac * inve[set_col][col];
					mine[set_col][row] -= fac * mine[set_col][col];
				});
			}
		});
		return inve;
	}
}


template<typename T, size_t S>
constexpr mat<T, S, S> adjoint(const mat<T, S, S>& m) noexcept
{
	if constexpr (S == 1)
		return { static_cast<T>(1.0 / m[0][0]) };
	else if constexpr (S == 2)
	{
		return mat<T, S, S>{
			vec<T, S>{m[1][1], m[0][1]},
				vec<T, S>{m[1][0], m[0][0]},
		};
	}
	else if constexpr (S == 3)
	{
		float a00 = m[0][0], a01 = m[1][0], a02 = m[2][0];
		float a10 = m[0][1], a11 = m[1][1], a12 = m[2][1];
		float a20 = m[0][2], a21 = m[1][2], a22 = m[2][2];

		float b01 = a22 * a11 - a12 * a21;
		float b11 = -a22 * a10 + a12 * a20;
		float b21 = a21 * a10 - a11 * a20;

		return vec<vec<T, S>, S>{vec<T, S>{b01, b11, b21},
			vec<T, S>{(-a22 * a01 + a02 * a21), (a22 * a00 - a02 * a20), (-a21 * a00 + a01 * a20)},
			vec<T, S>{(a12 * a01 - a02 * a11), (-a12 * a00 + a02 * a10), (a11 * a00 - a01 * a10)}};
	}
	else if constexpr (S == 4)
	{
		float a00 = m[0][0], a01 = m[1][0], a02 = m[2][0], a03 = m[3][0], a10 = m[0][1], a11 = m[1][1], a12 = m[2][1], a13 = m[3][1],
			a20 = m[0][2], a21 = m[1][2], a22 = m[2][2], a23 = m[3][2], a30 = m[0][3], a31 = m[1][3], a32 = m[2][3], a33 = m[3][3],
			b00 = a00 * a11 - a01 * a10, b01 = a00 * a12 - a02 * a10, b02 = a00 * a13 - a03 * a10, b03 = a01 * a12 - a02 * a11,
			b04 = a01 * a13 - a03 * a11, b05 = a02 * a13 - a03 * a12, b06 = a20 * a31 - a21 * a30, b07 = a20 * a32 - a22 * a30,
			b08 = a20 * a33 - a23 * a30, b09 = a21 * a32 - a22 * a31, b10 = a21 * a33 - a23 * a31, b11 = a22 * a33 - a23 * a32;

		return vec<vec<T, S>, S>{vec<T, S>{(a11 * b11 - a12 * b10 + a13 * b09), (a12 * b08 - a10 * b11 - a13 * b07),
			(a10 * b10 - a11 * b08 + a13 * b06), (a11 * b07 - a10 * b09 - a12 * b06)},
			vec<T, S>{(a02 * b10 - a01 * b11 - a03 * b09), (a00 * b11 - a02 * b08 + a03 * b07),
			(a01 * b08 - a00 * b10 - a03 * b06), (a00 * b09 - a01 * b07 + a02 * b06)},
			vec<T, S>{(a31 * b05 - a32 * b04 + a33 * b03), (a32 * b02 - a30 * b05 - a33 * b01),
			(a30 * b04 - a31 * b02 + a33 * b00), (a31 * b01 - a30 * b03 - a32 * b00)},
			vec<T, S>{(a22 * b04 - a21 * b05 - a23 * b03), (a20 * b05 - a22 * b02 + a23 * b01),
			(a21 * b02 - a20 * b04 - a23 * b00), (a20 * b03 - a21 * b01 + a22 * b00)}};
	}
	else
	{
		const auto inv_det = T(1.0 / det(m));
		gfx::mat<T, S> mine = m;
		gfx::mat<T, S> inve(gfx::identity);
		gfx::detail::apply_for_each_half_incl_indexed<T, S, S>([&](size_t col, size_t row)
		{
			if (row == col)
			{
				const auto inv_val = T(1.0 / mine[col][row]);
				gfx::detail::apply_for_each_indexed<T, S, 1>([&](size_t set_col, size_t)
				{
					inve[set_col][row] *= inv_val;
					mine[set_col][row] *= inv_val;
				});
			}
			else
			{
				if (const auto fac = mine[col][row])
				{
					gfx::detail::apply_for_each_indexed<T, S, 1>([&](size_t set_col, size_t)
					{
						inve[set_col][row] -= fac * inve[set_col][col];
						mine[set_col][row] -= fac * mine[set_col][col];
					});
				}
			}
		});

		gfx::detail::apply_for_each_half_bw_indexed<T, S, S>([&](size_t col, size_t row)
		{
			if (const auto fac = mine[col][row])
			{
				gfx::detail::apply_for_each_indexed<T, S, 1>([&](size_t set_col, size_t)
				{
					inve[set_col][row] -= fac * inve[set_col][col];
					mine[set_col][row] -= fac * mine[set_col][col];
				});
			}
		});
        gfx::detail::apply_for_each_indexed([&](size_t col, size_t row)
        {
			inve[col][row] *= inv_det;
		});
		return inve;
	}
}
}    // namespace gfx