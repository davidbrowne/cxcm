//          Copyright David Browne 2020-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// https://github.com/davidbrowne/cxcm

// opening include guard
#if !defined(CXCM_CXCM_HXX)
#define CXCM_CXCM_HXX

#include <limits>
#include <type_traits>
#include <concepts>
#include <cmath>
#include <bit>						// bit_cast
#include <cassert>

//
// cxcm_constexpr_assert() derived from https://gist.github.com/oliora/928424f7675d58fadf49c70fdba70d2f
//

#if defined(CXCM_DISABLE_ASSERTS)

#define cxcm_assertm(exp, msg) ((void)0)
#define cxcm_constexpr_assert(cond, msg) ((void)0)

#else

#define cxcm_assertm(exp, msg) assert(((void)msg, exp))

// this needs to be NOT constexpr, so attempted use of this function stops constexpr evaluation
template<class Assert>
inline void cxcm_constexpr_assert_failed(Assert &&a) noexcept
{
	std::forward<Assert>(a)();
}

// When evaluated at compile time emits a compilation error if condition is not true.
// Invokes the standard assert at run time.
#define cxcm_constexpr_assert(cond, msg) \
	((void)(!!(cond) ? 0 : (cxcm_constexpr_assert_failed([](){ assert(((void)msg, !static_cast<bool>(#cond)));}), 0)))

#endif

//
// ConstXpr CMath -- cxcm
//

// version info

constexpr inline int CXCM_MAJOR_VERSION = 1;
constexpr inline int CXCM_MINOR_VERSION = 0;
constexpr inline int CXCM_PATCH_VERSION = 0;

namespace cxcm
{
	namespace qdouble
	{
		// this is heavily modified to be as minimal as needed for constexpr sqrt() and rsqrt().
		// it will not be used with +/-infinity, NaNs, +/-0, or negative numbers.
		// https://github.com/janm31415/qdouble

		/*
		MIT License

		Copyright (c) 2022 Jan Maes

		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"), to deal
		in the Software without restriction, including without limitation the rights
		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		copies of the Software, and to permit persons to whom the Software is
		furnished to do so, subject to the following conditions:

		The above copyright notice and this permission notice shall be included in all
		copies or substantial portions of the Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
		SOFTWARE.
		*/

		/* The following code computes s = fl(a+b) and error(a + b), assuming |a| >= |b|. */
		constexpr double quick_two_sum(double a, double b, double &error) noexcept
		{
			double s = a + b;
			error = b - (s - a);
			return s;
		}

		/* The following code computes s = fl(a+b) and error(a + b). */
		constexpr double two_sum(double a, double b, double &error) noexcept
		{
			double s = a + b;
			double v = s - a;
			error = (a - (s - v)) + (b - v);
			return s;
		}

		/* The following code splits a 53-bit IEEE double precision floating number a into a high word and a low word, each with 26
		bits of significand, such that a is the sum of the high word with the low word. The high word will contain the first 26 bits,
		while the low word will contain the lower 26 bits.*/
		constexpr void split(double a, double &high, double &low) noexcept
		{
			double temp = 134217729.0 * a; // 134217729.0 = 2^27 + 1
			high = temp - (temp - a);
			low = a - high;
		}

		/* The following code computes fl(a x b) and error(a x b). */
		constexpr double two_prod(double a, double b, double &error) noexcept
		{
			double a_high, a_low, b_high, b_low;
			double p = a * b;
			split(a, a_high, a_low);
			split(b, b_high, b_low);
			error = ((a_high * b_high - p) + a_high * b_low + a_low * b_high) + a_low * b_low;
			return p;
		}

		constexpr void three_sum(double &a, double &b, double &c) noexcept
		{
			double t1, t2, t3;
			t1 = two_sum(a, b, t2);
			a = two_sum(c, t1, t3);
			b = two_sum(t2, t3, c);
		}

		constexpr void three_sum2(double &a, double &b, double &c) noexcept
		{
			double t1, t2, t3;
			t1 = two_sum(a, b, t2);
			a = two_sum(c, t1, t3);
			b = t2 + t3;
		}


		struct qdouble
		{
			double a[4];

			constexpr qdouble() noexcept
			{
				a[0] = 0.0;
				a[1] = 0.0;
				a[2] = 0.0;
				a[3] = 0.0;
			}

			constexpr qdouble(double a0, double a1, double a2, double a3) noexcept
			{
				a[0] = a0;
				a[1] = a1;
				a[2] = a2;
				a[3] = a3;
			}

			constexpr qdouble(double a0) noexcept
			{
				a[0] = a0;
				a[1] = 0.0;
				a[2] = 0.0;
				a[3] = 0.0;
			}

			constexpr qdouble(int i) noexcept
			{
				a[0] = static_cast<double>(i);
				a[1] = 0.0;
				a[2] = 0.0;
				a[3] = 0.0;
			}

			template <class TType>
			constexpr double operator[] (TType i) const noexcept
			{
				return a[i];
			}

			template <class TType>
			constexpr double &operator[] (TType i) noexcept
			{
				return a[i];
			}

			explicit constexpr operator double() noexcept
			{
				return a[0];
			}

		};

		constexpr bool operator == (const qdouble &a, const qdouble &b) noexcept
		{
			return (a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3]);
		}

		constexpr void renormalize(double &a0, double &a1, double &a2, double &a3) noexcept
		{
			double s0, s1, s2 = 0.0, s3 = 0.0;
			s0 = quick_two_sum(a2, a3, a3);
			s0 = quick_two_sum(a1, s0, a2);
			a0 = quick_two_sum(a0, s0, a1);

			s0 = a0;
			s1 = a1;
			if (s1 != 0.0)
			{
				s1 = quick_two_sum(s1, a2, s2);
				if (s2 != 0.0)
					s2 = quick_two_sum(s2, a3, s3);
				else
					s1 = quick_two_sum(s1, a3, s2);
			}
			else
			{
				s0 = quick_two_sum(s0, a2, s1);
				if (s1 != 0.0)
					s1 = quick_two_sum(s1, a3, s2);
				else
					s0 = quick_two_sum(s0, a3, s1);
			}

			a0 = s0;
			a1 = s1;
			a2 = s2;
			a3 = s3;
		}

		constexpr void renormalize(double &a0, double &a1, double &a2, double &a3, double &a4) noexcept
		{
			double s0, s1, s2 = 0.0, s3 = 0.0;

			s0 = quick_two_sum(a3, a4, a4);
			s0 = quick_two_sum(a2, s0, a3);
			s0 = quick_two_sum(a1, s0, a2);
			a0 = quick_two_sum(a0, s0, a1);

			s0 = a0;
			s1 = a1;

			s0 = quick_two_sum(a0, a1, s1);
			if (s1 != 0.0)
			{
				s1 = quick_two_sum(s1, a2, s2);
				if (s2 != 0.0)
				{
					s2 = quick_two_sum(s2, a3, s3);
					if (s3 != 0.0)
						s3 += a4;
					else
						s2 += a4;
				}
				else
				{
					s1 = quick_two_sum(s1, a3, s2);
					if (s2 != 0.0)
						s2 = quick_two_sum(s2, a4, s3);
					else
						s1 = quick_two_sum(s1, a4, s2);
				}
			}
			else
			{
				s0 = quick_two_sum(s0, a2, s1);
				if (s1 != 0.0)
				{
					s1 = quick_two_sum(s1, a3, s2);
					if (s2 != 0.0)
						s2 = quick_two_sum(s2, a4, s3);
					else
						s1 = quick_two_sum(s1, a4, s2);
				}
				else
				{
					s0 = quick_two_sum(s0, a3, s1);
					if (s1 != 0.0)
						s1 = quick_two_sum(s1, a4, s2);
					else
						s0 = quick_two_sum(s0, a4, s1);
				}
			}

			a0 = s0;
			a1 = s1;
			a2 = s2;
			a3 = s3;
		}

		/*
		s = double_accumulate(u,v,x) adds x to the double double pair (u,v).
		The output is the significant component s if the remaining components
		contain more than one double worth of significand. u and v are
		modified to represent the other two components in the sum.
		*/

		constexpr double double_accumulate(double &u, double &v, double x) noexcept
		{
			double s;
			bool zu, zv;

			s = two_sum(v, x, v);
			s = two_sum(u, s, u);

			zu = (u != 0.0);
			zv = (v != 0.0);

			if (zu && zv)
				return s;

			if (!zv)
			{
				v = u;
				u = s;
			}
			else
			{
				u = s;
			}

			return 0.0;
		}

		constexpr double abs(double value) noexcept
		{
			return (value < 0.0) ? -value : value;
		}

		constexpr qdouble operator + (const qdouble &a, const qdouble &b) noexcept
		{
			int i, j, k;
			double s, t;
			double u, v;   /* double-length accumulator */
			double x[4] = {0.0, 0.0, 0.0, 0.0};

			i = j = k = 0;
			if (abs(a[i]) > abs(b[j]))
				u = a[i++];
			else
				u = b[j++];
			if (abs(a[i]) > abs(b[j]))
				v = a[i++];
			else
				v = b[j++];

			u = quick_two_sum(u, v, v);

			while (k < 4)
			{
				if (i >= 4 && j >= 4)
				{
					x[k] = u;
					if (k < 3)
						x[++k] = v;
					break;
				}

				if (i >= 4)
					t = b[j++];
				else if (j >= 4)
					t = a[i++];
				else if (abs(a[i]) > abs(b[j]))
				{
					t = a[i++];
				}
				else
					t = b[j++];

				s = double_accumulate(u, v, t);

				if (s != 0.0)
				{
					x[k++] = s;
				}
			}

			/* add the rest. */
			for (k = i; k < 4; ++k)
				x[3] += a[k];
			for (k = j; k < 4; ++k)
				x[3] += b[k];

			renormalize(x[0], x[1], x[2], x[3]);
			return qdouble(x[0], x[1], x[2], x[3]);
		}

		constexpr qdouble operator - (const qdouble &a) noexcept
		{
			return qdouble(-a[0], -a[1], -a[2], -a[3]);
		}

		constexpr qdouble operator - (const qdouble &a, const qdouble &b) noexcept
		{
			return a + (-b);
		}

		constexpr qdouble &operator += (qdouble &a, const qdouble &b) noexcept
		{
			a = (a + b);
			return a;
		}

		constexpr qdouble &operator -= (qdouble &a, const qdouble &b) noexcept
		{
			a = (a - b);
			return a;
		}

		constexpr qdouble operator * (const qdouble &a, double b) noexcept
		{
			double p0, p1, p2, p3;
			double q0, q1, q2;
			double s0, s1, s2, s3, s4;
			p0 = two_prod(a[0], b, q0);
			p1 = two_prod(a[1], b, q1);
			p2 = two_prod(a[2], b, q2);
			p3 = a[3] * b;
			s0 = p0;
			s1 = two_sum(q0, p1, s2);
			three_sum(s2, q1, p2);
			three_sum2(q1, q2, p3);
			s3 = q1;
			s4 = q2 + p2;
			renormalize(s0, s1, s2, s3, s4);
			return qdouble(s0, s1, s2, s3);
		}

		constexpr qdouble operator * (const qdouble &a, const qdouble &b) noexcept
		{
			double p0, p1, p2, p3, p4, p5;
			double q0, q1, q2, q3, q4, q5;
			double p6, p7, p8, p9;
			double q6, q7, q8, q9;
			double r0, r1;
			double t0, t1;
			double s0, s1, s2;

			p0 = two_prod(a[0], b[0], q0);

			p1 = two_prod(a[0], b[1], q1);
			p2 = two_prod(a[1], b[0], q2);

			p3 = two_prod(a[0], b[2], q3);
			p4 = two_prod(a[1], b[1], q4);
			p5 = two_prod(a[2], b[0], q5);

			/* Start Accumulation */
			three_sum(p1, p2, q0);

			/* Six-Three Sum  of p2, q1, q2, p3, p4, p5. */
			three_sum(p2, q1, q2);
			three_sum(p3, p4, p5);
			/* compute (s0, s1, s2) = (p2, q1, q2) + (p3, p4, p5). */
			s0 = two_sum(p2, p3, t0);
			s1 = two_sum(q1, p4, t1);
			s2 = q2 + p5;
			s1 = two_sum(s1, t0, t0);
			s2 += (t0 + t1);

			/* O(eps^3) order terms */
			p6 = two_prod(a[0], b[3], q6);
			p7 = two_prod(a[1], b[2], q7);
			p8 = two_prod(a[2], b[1], q8);
			p9 = two_prod(a[3], b[0], q9);

			/* Nine-Two-Sum of q0, s1, q3, q4, q5, p6, p7, p8, p9. */
			q0 = two_sum(q0, q3, q3);
			q4 = two_sum(q4, q5, q5);
			p6 = two_sum(p6, p7, p7);
			p8 = two_sum(p8, p9, p9);
			/* Compute (t0, t1) = (q0, q3) + (q4, q5). */
			t0 = two_sum(q0, q4, t1);
			t1 += (q3 + q5);
			/* Compute (r0, r1) = (p6, p7) + (p8, p9). */
			r0 = two_sum(p6, p8, r1);
			r1 += (p7 + p9);
			/* Compute (q3, q4) = (t0, t1) + (r0, r1). */
			q3 = two_sum(t0, r0, q4);
			q4 += (t1 + r1);
			/* Compute (t0, t1) = (q3, q4) + s1. */
			t0 = two_sum(q3, s1, t1);
			t1 += q4;

			/* O(eps^4) terms -- Nine-One-Sum */
			t1 += a[1] * b[3] + a[2] * b[2] + a[3] * b[1] + q6 + q7 + q8 + q9 + s2;

			renormalize(p0, p1, s0, t0, t1);
			return qdouble(p0, p1, s0, t0);
		}

		constexpr qdouble operator / (const qdouble &a, const qdouble &b) noexcept
{
			double q0, q1, q2, q3;

			qdouble r;

			q0 = a[0] / b[0];
			r = a - (b * q0);

			q1 = r[0] / b[0];
			r -= (b * q1);

			q2 = r[0] / b[0];
			r -= (b * q2);

			q3 = r[0] / b[0];

			r -= (b * q3);
			double q4 = r[0] / b[0];

			renormalize(q0, q1, q2, q3, q4);

			return qdouble(q0, q1, q2, q3);
		}

	} // namespace qdouble

	namespace limits
	{
		namespace detail
		{
			// long doubles vary between compilers and platforms. Windows MSVC and clang on Windows both use
			// the same representation as double. For gcc and linux, etc., it is often represented by an extended
			// precision data structure with 80 bits (64 bits of significand). sizeof(long double) on gcc on Windows
			// (at least MSYS2) is 16, implying it is 128 bits, but std::numeric_limits<long double> returns values
			// consistent with an 80 bit representation.
			constexpr long double get_largest_fractional_long_double() noexcept
			{
				if constexpr (std::numeric_limits<long double>::digits == 64)
				{
					// if digits is 64, then long double is using extended precision, and we can
					// just barely get away with casting to a long long to remove the fractional
					// part and keep the rest of the bits, without overflow.
					return 0x1.fffffffffffffffep+62L;
				}
				else
				{
					// assuming that long double does the same thing as double (which is true for
					// MSVC and clang on windows).
					return 0x1.fffffffffffffp+51L;
				}
			}
		}

		//
		// largest_fractional_value
		//

		// the largest floating point value that has a fractional representation

		template <std::floating_point T>
		constexpr inline T largest_fractional_value = T();

		template <>
		constexpr inline long double largest_fractional_value<long double> = detail::get_largest_fractional_long_double();

		template <>
		constexpr inline double largest_fractional_value<double> = 0x1.fffffffffffffp+51;

		template <>
		constexpr inline float largest_fractional_value<float> = 0x1.fffffep+22f;
	}

	// don't worry about esoteric input.
	// much faster than strict or standard when non constant evaluated,
	// though standard library is a little better in debugger.
	namespace relaxed
	{

		//
		// abs(), fabs()
		//

		// absolute value

		template <std::floating_point T>
		constexpr T abs(T value) noexcept
		{
			return (value < T(0)) ? -value : value;
		}

		// undefined behavior if value is std::numeric_limits<T>::min()
		template <std::signed_integral T>
		constexpr double abs(T value) noexcept
		{
			return (value < T(0)) ? -value : value;
		}

		template <std::unsigned_integral T>
		constexpr double abs(T value) noexcept
		{
			return value;
		}

		template <std::floating_point T>
		constexpr T fabs(T value) noexcept
		{
			return abs(value);
		}

		template <std::integral T>
		constexpr double fabs(T value) noexcept
		{
			return abs(value);
		}

		//
		// trunc()
		//

		// this is the workhorse function for floor(), ceil(), and round().

		// rounds towards zero

		template <std::floating_point T>
		constexpr T trunc(T value) noexcept
		{
			return static_cast<T>(static_cast<long long>(value));
		}

		// float specialization
		template <>
		constexpr float trunc(float value) noexcept
		{
			return static_cast<float>(static_cast<int>(value));
		}

		//
		// floor()
		//

		// rounds towards negative infinity

		template <std::floating_point T>
		constexpr T floor(T value) noexcept
		{
			const T truncated_value = trunc(value);

			// truncation rounds to zero which is right direction for positive values,
			// but we need to go the other way for negative values.

			// negative non-integral value
			if (truncated_value > value)
				return (truncated_value - T(1.0f));

			// positive or integral value
			return truncated_value;
		}

		//
		// ceil()
		//

		// rounds towards positive infinity

		template <std::floating_point T>
		constexpr T ceil(T value) noexcept
		{
			const T truncated_value = trunc(value);

			// truncation rounds to zero which is right direction for negative values,
			// but we need to go the other way for positive values.

			// positive non-integral value
			if (truncated_value < value)
				return (truncated_value + T(1.0f));

			// negative or integral value
			return truncated_value;
		}

		//
		// round()
		//

		// rounds to nearest integral position, halfway cases away from zero

		template <std::floating_point T>
		constexpr T round(T value) noexcept
		{
			// zero could be handled either place, but here it is with the negative values.

			// positive value, taking care of halfway case.
			if (value > T(0))
				return trunc(value + T(0.5f));

			// negative or zero value, taking care of halfway case.
			return trunc(value - T(0.5f));
		}

		//
		// fract() - not in standard library
		//

		// the fractional part of a floating point number - always non-negative.

		template <std::floating_point T>
		constexpr T fract(T value) noexcept
		{
			return value - floor(value);
		}

		//
		// fmod()
		//

		// the floating point remainder of division

		template <std::floating_point T>
		constexpr T fmod(T x, T y) noexcept
		{
			return x - trunc(x / y) * y;
		}

		//
		// round_even() - not in standard library
		//

		// rounds to nearest integral position, halfway cases towards even

		template <std::floating_point T>
		constexpr T round_even(T value) noexcept
		{
			T trunc_value = trunc(value);
			bool is_even = (fmod(trunc_value, T(2)) == T(0));
			bool is_halfway = (fract(value) == T(0.5));

			// the special case
			if (is_halfway && is_even)
				return trunc_value;

			// zero could be handled either place, but here it is with the negative values.

			// positive value, taking care of halfway case.
			if (value > T(0))
				return trunc(value + T(0.5f));

			// negative or zero value, taking care of halfway case.
			return trunc(value - T(0.5f));
		}

		//
		// sqrt()
		//

		namespace detail
		{
			template <typename T>
			constexpr T converging_sqrt(T arg) noexcept
			{
				T current_value = arg;
				T previous_value = T(0);

				if constexpr (std::is_same_v<T, qdouble::qdouble>)
				{
					while ((current_value[0] * current_value[0] != arg[0]) && (current_value[0] != previous_value[0]))
					{
						previous_value = current_value;
						current_value = (T(0.5) * current_value) + (T(0.5) * (arg / current_value));
					}
				}
				else if constexpr (std::floating_point<T>)
				{
					while ((current_value * current_value != arg) && (current_value != previous_value))
					{
						previous_value = current_value;
						current_value = (T(0.5) * current_value) + (T(0.5) * (arg / current_value));
					}
				}

				return current_value;
			}

			template <typename T>
			constexpr T inverse_sqrt(T arg) noexcept
			{
				T current_value = T(1.0) / converging_sqrt(arg);

				// 3 refinements is best
				current_value += T(0.5) * current_value * (T(1.0) - arg * current_value * current_value);
				current_value += T(0.5) * current_value * (T(1.0) - arg * current_value * current_value);
				current_value += T(0.5) * current_value * (T(1.0) - arg * current_value * current_value);

				return current_value;
			}
		}

		template <std::floating_point T>
		constexpr T sqrt(T value) noexcept
		{
			return detail::converging_sqrt(value);
		}

		// float specialization - uses higher precision internally (double) which gives us 100% fidelity with std::sqrt()
		template <>
		constexpr float sqrt(float value) noexcept
		{
			double val = value;
			return static_cast<float>(detail::converging_sqrt(val));
		}

		// double specialization - uses higher precision internally (qdouble) which gives us 100% fidelity with std::sqrt()
		template <>
		constexpr double sqrt(double value) noexcept
		{
			qdouble::qdouble val = value;
			return static_cast<double>(detail::converging_sqrt(val));
		}

		// reciprocal of square root - our constexpr sqrt() has 100% fidelity with std::sqrt()
		template <std::floating_point T>
		constexpr T rsqrt(T value) noexcept
		{
			return T(1.0) / sqrt(value);
//			return T(1.0) / (value * detail::inverse_sqrt(value));
		}

	} // namespace relaxed

	//
	// isnan()
	//

	// make sure this isn't optimized away if used with fast-math

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(precise, on, push)
#endif

	template <std::floating_point T>
	#if defined(__GNUC__) && !defined(__clang__)
		__attribute__((optimize("-fno-fast-math")))
	#endif
	constexpr bool isnan(T value) noexcept
	{
		return (value != value);
	}

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(pop)
#endif

	//
	// isinf()
	//

	template <std::floating_point T>
	constexpr bool isinf(T value) noexcept
	{
		return (value == -std::numeric_limits<T>::infinity()) || (value == std::numeric_limits<T>::infinity());
	}

	//
	// fpclassify()
	//

	template <std::floating_point T>
	constexpr int fpclassify(T value) noexcept
	{
		if (isnan(value))
			return FP_NAN;
		else if (isinf(value))
			return FP_INFINITE;
		else if (value == 0)				// intentional use of the implicit cast of 0 to T.
			return FP_ZERO;
		else if (relaxed::abs(value) < std::numeric_limits<T>::min())
			return FP_SUBNORMAL;

		return FP_NORMAL;
	}

	//
	// isnormal()
	//

	template <std::floating_point T>
	constexpr bool isnormal(T value) noexcept
	{
		return (fpclassify(value) == FP_NORMAL);
	}

	//
	// isfinite()
	//

	template <std::floating_point T>
	constexpr bool isfinite(T value) noexcept
	{
		return !isnan(value) && !isinf(value);
	}

	//
	// signbit()
	//

	// +0 returns false and -0 returns true
	template <std::floating_point T>
	constexpr bool signbit(T value) noexcept
	{
		if constexpr (sizeof(T) == 4)
		{
			unsigned int bits = std::bit_cast<unsigned int>(value);
			return (bits & 0x80000000) != 0;
		}
		else if constexpr (sizeof(T) == 8)
		{
			unsigned long long bits = std::bit_cast<unsigned long long>(value);
			return (bits & 0x8000000000000000) != 0;
		}
	}

	//
	// copysign()
	//

	// +0 or -0 for sign is considered as *not* negative
	template <std::floating_point T>
	constexpr T copysign(T value, T sgn) noexcept
	{
		// +0 or -0 for sign is considered as *not* negative
		bool is_neg = signbit(sgn);

		if constexpr (sizeof(T) == 4)
		{
			unsigned int bits = std::bit_cast<unsigned int>(value);
			if (is_neg)
				bits |= 0x80000000;
			else
				bits &= 0x7FFFFFFF;

			return std::bit_cast<T>(bits);
		}
		else if constexpr (sizeof(T) == 8)
		{
			unsigned long long bits = std::bit_cast<unsigned long long>(value);
			if (is_neg)
				bits |= 0x8000000000000000;
			else
				bits &= 0x7FFFFFFFFFFFFFFF;

			return std::bit_cast<T>(bits);
		}
	}

	// try and match standard library requirements.
	// this namespace is pulled into parent namespace via inline.
	inline namespace strict
	{

		namespace detail
		{

			//
			// isnormal_or_subnormal()
			//

			// standard library screening requirement for these functions

			template <std::floating_point T>
			constexpr bool isnormal_or_subnormal(T value) noexcept
			{
				// intentional use of the implicit cast of 0 to T.
				return isfinite(value) && (value != 0);
			}

			//
			// fails_fractional_input_constraints()
			//

			// the fractional functions,e.g., trunc(), floor(), ceil(), round(), need the input to satisfy
			// certain constraints before it further processes the input. if this function returns true,
			// the constraints weren't met, and the fractional functions will do no further work and return
			// the value as is.

			template <std::floating_point T>
			constexpr bool fails_fractional_input_constraints(T value) noexcept
			{
				// if any of the following constraints are not met, return true:
				// no NaNs
				// no +/- infinity
				// no +/- 0
				// no value that can't even have a fractional part
				return !isnormal_or_subnormal(value) || (relaxed::abs(value) > limits::largest_fractional_value<T>);
			}

			//
			// constexpr_trunc()
			//

			// rounds towards zero

			template <std::floating_point T>
			constexpr T constexpr_trunc(T value) noexcept
			{
				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::trunc(value);
			}

			//
			// constexpr_floor()
			//

			// rounds towards negative infinity

			template <std::floating_point T>
			constexpr T constexpr_floor(T value) noexcept
			{
				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::floor(value);
			}

			//
			// constexpr_ceil()
			//

			// rounds towards positive infinity

			template <std::floating_point T>
			constexpr T constexpr_ceil(T value) noexcept
			{
				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::ceil(value);
			}

			//
			// constexpr_round()
			//

			// rounds to nearest integral position, halfway cases away from zero

			template <std::floating_point T>
			constexpr T constexpr_round(T value) noexcept
			{
				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				// halfway rounding can bump into max long long value for truncation
				// (for extended precision), so be more gentle at the end points.
				// this works because the largest_fractional_value remainder is T(0.5f).
				if (value == limits::largest_fractional_value<T>)
					return value + T(0.5f);
				else if (value == -limits::largest_fractional_value<T>)			// we technically don't have to do this for negative case (one more number in negative range)
					return value - T(0.5f);

				return relaxed::round(value);
			}

			//
			// constexpr_fract()
			//

			template <std::floating_point T>
			constexpr T constexpr_fract(T value) noexcept
			{
				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::fract(value);
			}

			//
			// constexpr_fmod()
			//

			template <std::floating_point T>
			constexpr T constexpr_fmod(T x, T y) noexcept
			{
				// screen out unnecessary input

				if (isnan(x) || isnan(y) || !isfinite(x))
					return std::numeric_limits<T>::quiet_NaN();

				if (isinf(y))
					return x;

				if (x == T(0) && y != T(0))
					return 0;

				if (y == 0)
					return std::numeric_limits<T>::quiet_NaN();

				return relaxed::fmod(x, y);
			}

			//
			// constexpr_round_even()
			//

			// rounds to nearest integral position, halfway cases away from zero

			template <std::floating_point T>
			constexpr T constexpr_round_even(T value) noexcept
			{
				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				// halfway rounding can bump into max long long value for truncation
				// (for extended precision), so be more gentle at the end points.
				// this works because the largest_fractional_value remainder is T(0.5f).
				if (value == limits::largest_fractional_value<T>)
					return value + T(0.5f);
				else if (value == -limits::largest_fractional_value<T>)			// we technically don't have to do this for negative case (one more number in negative range)
					return value - T(0.5f);

				return relaxed::round_even(value);
			}

			//
			// constexpr_sqrt()
			//

			template <std::floating_point T>
			constexpr T constexpr_sqrt(T value) noexcept
			{
				// screen out unnecessary input

				if (isnan(value))
				{
					if constexpr (sizeof(T) == 4)
					{
						unsigned int bits = std::bit_cast<unsigned int>(value);

						// set the is_quiet bit
						bits |= 0x00400000;

						return std::bit_cast<T>(bits);
					}
					else if constexpr (sizeof(T) == 8)
					{
						unsigned long long bits = std::bit_cast<unsigned long long>(value);

						// set the is_quiet bit
						bits |= 0x0008000000000000;

						return std::bit_cast<T>(bits);
					}
				}
				else if (value == std::numeric_limits<T>::infinity())
				{
					return value;
				}
				else if (value == -std::numeric_limits<T>::infinity())
				{
					return -std::numeric_limits<T>::quiet_NaN();
				}
				else if (value == T(0))
				{
					return value;
				}
				else if (value < T(0))
				{
					return -std::numeric_limits<T>::quiet_NaN();
				}

				return relaxed::sqrt(value);
			}

			//
			// constexpr_inverse_sqrt()
			//

			template <std::floating_point T>
			constexpr T constexpr_rsqrt(T value) noexcept
			{
				// screen out unnecessary input

				if (isnan(value))
				{
					if constexpr (sizeof(T) == 4)
					{
						unsigned int bits = std::bit_cast<unsigned int>(value);

						// set the is_quiet bit
						bits |= 0x00400000;

						return std::bit_cast<T>(bits);
					}
					else if constexpr (sizeof(T) == 8)
					{
						unsigned long long bits = std::bit_cast<unsigned long long>(value);

						// set the is_quiet bit
						bits |= 0x0008000000000000;

						return std::bit_cast<T>(bits);
					}
				}
				else if (value == std::numeric_limits<T>::infinity())
				{
					return T(0);
				}
				else if (value == -std::numeric_limits<T>::infinity())
				{
					return -std::numeric_limits<T>::quiet_NaN();
				}
				else if (value == T(0))
				{
					return std::numeric_limits<T>::infinity();
				}
				else if (value < T(0))
				{
					return -std::numeric_limits<T>::quiet_NaN();
				}

				return relaxed::rsqrt(value);
			}

		} // namespace detail

		//
		// abs(), fabs()
		//


		// absolute value

		template <std::floating_point T>
		constexpr T abs(T value) noexcept
		{
			if (!detail::isnormal_or_subnormal(value))
				return value;

			return relaxed::abs(value);
		}

		// don't know what to do if someone tries to negate the most negative number.
		// standard says behavior is undefined if you can't represent the result by return type.
		template <std::integral T>
		constexpr double abs(T value) noexcept
		{
			cxcm_constexpr_assert(value != std::numeric_limits<T>::min(), "undefined behavior in abs()");

			return relaxed::abs(value);
		}

		template <std::floating_point T>
		constexpr T fabs(T value) noexcept
		{
			if (!detail::isnormal_or_subnormal(value))
				return value;

			return relaxed::fabs(value);
		}

		template <std::integral T>
		constexpr double fabs(T value) noexcept
		{
			cxcm_constexpr_assert(value != std::numeric_limits<T>::min(), "undefined behavior in fabs()");

			return relaxed::fabs(value);
		}

		//
		// trunc()
		//

		// rounds towards zero

		template <std::floating_point T>
		constexpr T trunc(T value) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_trunc(value);
			}
			else
			{
				return std::trunc(value);
			}
		}

		template <std::integral T>
		constexpr double trunc(T value) noexcept
		{
			return value;
		}

		//
		// floor()
		//

		// rounds towards negative infinity

		template <std::floating_point T>
		constexpr T floor(T value) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_floor(value);
			}
			else
			{
				return std::floor(value);
			}
		}

		template <std::integral T>
		constexpr double floor(T value) noexcept
		{
			return value;
		}

		//
		// ceil()
		//

		// rounds towards positive infinity

		template <std::floating_point T>
		constexpr T ceil(T value) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_ceil(value);
			}
			else
			{
				return std::ceil(value);
			}
		}

		template <std::integral T>
		constexpr double ceil(T value) noexcept
		{
			return value;
		}

		//
		// round()
		//

		// rounds to nearest integral position, halfway cases away from zero

		template <std::floating_point T>
		constexpr T round(T value) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_round(value);
			}
			else
			{
				return std::round(value);
			}
		}

		template <std::integral T>
		constexpr double round(T value) noexcept
		{
			return value;
		}

		//
		// fract()
		//

		// there is no standard c++ version of this, so always call constexpr version

		// the fractional part of a floating point number - always non-negative.

		template <std::floating_point T>
		constexpr T fract(T value) noexcept
		{
			return detail::constexpr_fract(value);
		}

		template <std::integral T>
		constexpr double fract(T /* value */) noexcept
		{
			return 0.0;
		}

		//
		// fmod()
		//

		// the floating point remainder of division

		template <std::floating_point T>
		constexpr T fmod(T x, T y) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_fmod(x, y);
			}
			else
			{
				return std::fmod(x, y);
			}
		}

		//
		// round_even()
		//

		// there is no standard c++ version of this, so always call constexpr version

		// rounds to nearest integral position, halfway cases towards even

		template <std::floating_point T>
		constexpr T round_even(T value) noexcept
		{
			return detail::constexpr_round_even(value);
		}

		template <std::integral T>
		constexpr double round_even(T value) noexcept
		{
			return value;
		}

		//
		// sqrt()
		//

		template <std::floating_point T>
		constexpr T sqrt(T value) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_sqrt(value);
			}
			else
			{
				return std::sqrt(value);
			}
		}

		//
		// rsqrt() - inverse square root
		//

		template <std::floating_point T>
		constexpr T rsqrt(T value) noexcept
		{
			if (std::is_constant_evaluated())
			{
				return detail::constexpr_rsqrt(value);
			}
			else
			{
				return T(1.0) / std::sqrt(value);
			}
		}

	} // namespace strict

} // namespace cxcm

// closing include guard
#endif
