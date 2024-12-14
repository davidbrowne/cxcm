//          Copyright David Browne 2020-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// https://github.com/davidbrowne/cxcm

// opening include guard
#if !defined(CXCM_HXX)
#define CXCM_HXX

#include <limits>
#include <type_traits>
#include <concepts>
#include <cmath>
#include <bit>						// bit_cast
#include <stdexcept>

//
// ConstXpr CMath -- cxcm
//

namespace cxcm
{
    //          Copyright David Browne 2020-2024.
    // Distributed under the Boost Software License, Version 1.0.
    //    (See accompanying file LICENSE_1_0.txt or copy at
    //          https://www.boost.org/LICENSE_1_0.txt)
 
	// version info

	constexpr int CXCM_MAJOR_VERSION = 1;
	constexpr int CXCM_MINOR_VERSION = 1;
	constexpr int CXCM_PATCH_VERSION = 10;

	namespace dd_real
	{
		// https://www.davidhbailey.com/dhbsoftware/ - QD

		/*
			Modified BSD 3-Clause License

			This work was supported by the Director, Office of Science, Division
			of Mathematical, Information, and Computational Sciences of the
			U.S. Department of Energy under contract number DE-AC03-76SF00098.

			Copyright (c) 2000-2007

			1. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

				(1) Redistributions of source code must retain the copyright notice, this list of conditions and the following disclaimer.

				(2) Redistributions in binary form must reproduce the copyright notice, this list of conditions and the following disclaimer in the documentation
					and/or other materials provided with the distribution.

				(3) Neither the name of the University of California, Lawrence Berkeley National Laboratory, U.S. Dept. of Energy nor the names of its contributors
					may be used to endorse or promote products derived from this software without specific prior written permission.

			2. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
			   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
			   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
			   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
			   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
			   OF THE POSSIBILITY OF SUCH DAMAGE.

			3. You are under no obligation whatsoever to provide any bug fixes, patches, or upgrades to the features, functionality or performance of the
			   source code ("Enhancements") to anyone; however, if you choose to make your Enhancements available either publicly, or directly to Lawrence
			   Berkeley National Laboratory, without imposing a separate written license agreement for such Enhancements, then you hereby grant the following
			   license: a non-exclusive, royalty-free perpetual license to install, use, modify, prepare derivative works, incorporate into other computer
			   software, distribute, and sublicense such enhancements or derivative works thereof, in binary and source code form.
		*/

		//
		// heavily modified dd_real type and support
		//

		// The following code computes s = fl(a+b) and error(a + b), assuming |a| >= |b|.
		constexpr double quick_two_sum(double a, double b, double &error) noexcept
		{
			double s = a + b;
			error = b - (s - a);
			return s;
		}

		// The following code computes s = fl(a+b) and error(a + b).
		constexpr double two_sum(double a, double b, double &error) noexcept
		{
			double s = a + b;
			double v = s - a;
			error = (a - (s - v)) + (b - v);
			return s;
		}

		// The following code splits a 53-bit IEEE double precision floating number a into a high word and a low word, each with 26
		// bits of significand, such that a is the sum of the high word with the low word. The high word will contain the first 26 bits,
		// while the low word will contain the lower 26 bits.
		constexpr void split(double a, double &high, double &low) noexcept
		{
			double temp = 134217729.0 * a;				// 134217729.0 = 2^27 + 1
			high = temp - (temp - a);
			low = a - high;
		}

		// The following code computes fl(a x b) and error(a x b).
		constexpr double two_prod(double a, double b, double &error) noexcept
		{
			double a_high = 0.0;
			double a_low = 0.0;
			double b_high = 0.0;
			double b_low = 0.0;

			double p = a * b;
			split(a, a_high, a_low);
			split(b, b_high, b_low);
			error = ((a_high * b_high - p) + a_high * b_low + a_low * b_high) + a_low * b_low;
			return p;
		}

		// higher precision double-double
		struct dd_real
		{
			double x[2];

			constexpr dd_real() noexcept : x{}
			{
			}

			constexpr dd_real(double hi, double lo) noexcept : x{hi, lo}
			{
			}

			explicit constexpr dd_real(double h) noexcept : x{h, 0.}
			{
			}

			constexpr dd_real(const dd_real &) noexcept = default;
			constexpr dd_real(dd_real &&) noexcept = default;
			constexpr dd_real &operator =(const dd_real &) noexcept = default;
			constexpr dd_real &operator =(dd_real &&) noexcept = default;

			constexpr double operator [](unsigned int index) const noexcept
			{
				return x[index];
			}

			constexpr double &operator [](unsigned int index) noexcept
			{
				return x[index];
			}

			explicit constexpr operator double() const noexcept
			{
				return x[0];
			}

			explicit constexpr operator float() const noexcept
			{
				return static_cast<float>(x[0]);
			}

		};

		// double-double + double-double
		constexpr dd_real ieee_add(const dd_real &a, const dd_real &b) noexcept
		{
			// This one satisfies IEEE style error bound, due to K. Briggs and W. Kahan.
			double s1 = 0.0;
			double s2 = 0.0;
			double t1 = 0.0;
			double t2 = 0.0;

			s1 = two_sum(a.x[0], b.x[0], s2);
			t1 = two_sum(a.x[1], b.x[1], t2);
			s2 += t1;
			s1 = quick_two_sum(s1, s2, s2);
			s2 += t2;
			s1 = quick_two_sum(s1, s2, s2);
			return dd_real(s1, s2);
		}

		// double-double + double
		constexpr dd_real ieee_add(const dd_real &a, double b) noexcept
		{
			// This one satisfies IEEE style error bound, due to K. Briggs and W. Kahan.
			double s1 = 0.0;
			double s2 = 0.0;

			s1 = two_sum(a.x[0], b, s2);
			s1 = quick_two_sum(s1, s2 + a.x[1], s2);
			return dd_real(s1, s2);
		}

		// double-double - double-double
		constexpr dd_real ieee_subtract(const dd_real &a, const dd_real &b) noexcept
		{
			// This one satisfies IEEE style error bound, due to K. Briggs and W. Kahan.
			double s1 = 0.0;
			double s2 = 0.0;
			double t1 = 0.0;
			double t2 = 0.0;

			s1 = two_sum(a.x[0], -b.x[0], s2);
			t1 = two_sum(a.x[1], -b.x[1], t2);
			s2 += t1;
			s1 = quick_two_sum(s1, s2, s2);
			s2 += t2;
			s1 = quick_two_sum(s1, s2, s2);
			return dd_real(s1, s2);
		}

		// double - double-double
		constexpr dd_real ieee_subtract(double a, const dd_real &b) noexcept
		{
			// This one satisfies IEEE style error bound, due to K. Briggs and W. Kahan.
			double s1 = 0.0;
			double s2 = 0.0;

			s1 = two_sum(a, -b.x[0], s2);
			s1 = quick_two_sum(s1, s2 - b.x[1], s2);
			return dd_real(s1, s2);
		}

		// double-double + double-double
		constexpr dd_real operator +(const dd_real &a, const dd_real &b) noexcept
		{
			return ieee_add(a, b);
		}

		// double-double + double
		constexpr dd_real operator +(const dd_real &a, double b) noexcept
		{
			return ieee_add(a, b);
		}

		constexpr dd_real operator -(const dd_real &a, const dd_real &b) noexcept
		{
			return ieee_subtract(a, b);
		}

		constexpr dd_real operator -(double a, const dd_real &b) noexcept
		{
			return ieee_subtract(a, b);
		}

		constexpr dd_real &operator -=(dd_real &a, const dd_real &b) noexcept
		{
			a = (a - b);
			return a;
		}

		// double-double * double-double
		constexpr dd_real operator *(const dd_real &a, const dd_real &b) noexcept
		{
			double p1 = 0.0;
			double p2 = 0.0;

			p1 = two_prod(a.x[0], b.x[0], p2);
			p2 += (a.x[0] * b.x[1] + a.x[1] * b.x[0]);
			p1 = quick_two_sum(p1, p2, p2);
			return dd_real(p1, p2);
		}

		// double-double * double
		constexpr dd_real operator *(const dd_real &a, double b) noexcept
		{
			double p1 = 0.0;
			double p2 = 0.0;

			p1 = two_prod(a.x[0], b, p2);
			p1 = quick_two_sum(p1, p2 + (a.x[1] * b), p2);
			return dd_real(p1, p2);
		}

		// double * double-double
		constexpr dd_real operator *(double a, const dd_real &b) noexcept
		{
			return (b * a);
		}

		constexpr dd_real &operator *=(dd_real &a, const dd_real &b) noexcept
		{
			double p1 = 0.0;
			double p2 = 0.0;

			p1 = two_prod(a.x[0], b.x[0], p2);
			p2 += (a.x[0] * b.x[1] + a.x[1] * b.x[0]);
			a.x[0] = quick_two_sum(p1, p2, a.x[1]);
			return a;
		}

		constexpr dd_real accurate_div(const dd_real &a, const dd_real &b) noexcept
		{
			double q1 = 0.0;
			double q2 = 0.0;
			double q3 = 0.0;

			q1 = a.x[0] / b.x[0];						// approximate quotient

			dd_real r = a - q1 * b;

			q2 = r.x[0] / b.x[0];
			r -= (q2 * b);

			q3 = r.x[0] / b.x[0];

			q1 = quick_two_sum(q1, q2, q2);

			double s1 = 0.0;
			double s2 = 0.0;
			s1 = two_sum(q1, q3, s2);
			s1 = quick_two_sum(s1, s2 + q2, s2);

			return dd_real(s1, s2);
		}

		constexpr dd_real accurate_div(double a, const dd_real &b) noexcept
		{
			double q1 = 0.0;
			double q2 = 0.0;
			double q3 = 0.0;

			q1 = a / b.x[0];							// approximate quotient

			dd_real r = a - q1 * b;

			q2 = r.x[0] / b.x[0];
			r -= (q2 * b);

			q3 = r.x[0] / b.x[0];

			q1 = quick_two_sum(q1, q2, q2);

			double s1 = 0.0;
			double s2 = 0.0;
			s1 = two_sum(q1, q3, s2);
			s1 = quick_two_sum(s1, s2 + q2, s2);

			return dd_real(s1, s2);
		}

		// double / double-double
		constexpr dd_real operator /(double a, const dd_real &b) noexcept
		{
			return accurate_div(a, b);
		}

		// double-double / double-double
		constexpr dd_real operator /(const dd_real &a, const dd_real &b) noexcept
		{
			return accurate_div(a, b);
		}

	} // namespace dd_real

	namespace concepts
	{
		template <typename T>
		concept basic_floating_point = (std::is_same_v<float, T> || std::is_same_v<double, T>);
	}

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

		template <cxcm::concepts::basic_floating_point T>
		constexpr inline T largest_fractional_value = T();

		template <>
		constexpr inline double largest_fractional_value<double> = 0x1.fffffffffffffp+51;

		template <>
		constexpr inline float largest_fractional_value<float> = 0x1.fffffep+22f;
	}

	//
	// floating-point negative zero support
	//

	template <cxcm::concepts::basic_floating_point T>
	constexpr bool is_negative_zero(T) noexcept
	{
		return false;
	}

	template<>
	constexpr bool is_negative_zero(float val) noexcept
	{
		return (0x80000000 == std::bit_cast<unsigned int>(val));
	}

	template<>
	constexpr bool is_negative_zero(double val) noexcept
	{
		return (0x8000000000000000 == std::bit_cast<unsigned long long>(val));
	}

	template <cxcm::concepts::basic_floating_point T>
	constexpr inline T negative_zero = T(-0);

	template <>
	constexpr inline float negative_zero<float> = std::bit_cast<float>(0x80000000);

	template <>
	constexpr inline double negative_zero<double> = std::bit_cast<double>(0x8000000000000000);

	// don't worry about esoteric input.
	// much faster than strict or standard when non constant evaluated,
	// though standard library is a little better in debugger.
	namespace relaxed
	{

		//
		// abs(), fabs()
		//

		// absolute value

		template <cxcm::concepts::basic_floating_point T>
		constexpr T abs(T value) noexcept
		{
			return (value < T(0)) ? -value : value;
		}

		// undefined behavior if value is std::numeric_limits<T>::min()
		template <std::signed_integral T>
		constexpr T abs(T value) noexcept
		{
			return (value < T(0)) ? -value : value;
		}

		template <std::unsigned_integral T>
		constexpr T abs(T value) noexcept
		{
			return value;
		}

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
		constexpr T fract(T value) noexcept
		{
			return value - floor(value);
		}

		//
		// fmod()
		//

		// the floating point remainder of division

		template <cxcm::concepts::basic_floating_point T>
		constexpr T fmod(T x, T y) noexcept
		{
			return x - trunc(x / y) * y;
		}

		//
		// round_even() - not in standard library
		//

		// rounds to nearest integral position, halfway cases towards even

		template <cxcm::concepts::basic_floating_point T>
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

			// "Improving the Accuracy of the Fast Inverse Square Root by Modifying Newton-Raphson Corrections" 2021
			// https://www.mdpi.com/1099-4300/23/1/86
			//
			// in comparison to inverse_sqrt(double), this method gives pretty good results:
			//    0 ulps: ~68.58%
			//    1 ulps: ~31.00%
			//    2 ulps:  ~0.42%
			//
			// depending on compiler/platform, this may not be faster than rsqrt()
			constexpr double fast_rsqrt(double x) noexcept
			{
				double halfx = 0.5 * x;
				long long i = std::bit_cast<long long>(x);
				i = 0x5FE6ED2102DCBFDA - (i >> 1);
				double y = std::bit_cast<double>(i);
				y *= 1.50087895511633457 - halfx * y * y;
				y *= 1.50000057967625766 - halfx * y * y;
				y *= 1.5000000000002520 - halfx * y * y;
				y *= 1.5000000000000000 - halfx * y * y;
				return y;
			}

			// float uses double internally, double uses dd_real internally
			template <cxcm::concepts::basic_floating_point T>
			constexpr T converging_sqrt(T arg) noexcept
			{
				const double boosted_arg = arg;
				double init_value = boosted_arg * fast_rsqrt(boosted_arg);

				if constexpr (std::is_same_v<T, double>)
				{
					// boosted_arg doesn't need to be a dd_real for [T = double]

					auto current_value = dd_real::dd_real(init_value);
					auto previous_value = dd_real::dd_real(0.0);

					while ((current_value[0] != previous_value[0]) && (current_value[0] * current_value[0] != boosted_arg))
					{
						previous_value = current_value;
						current_value = 0.5 * (current_value + (boosted_arg / current_value));
					}

					return static_cast<double>(current_value);
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					double current_value = init_value;
					double previous_value = 0.0;

					while ((current_value != previous_value) && (current_value * current_value != boosted_arg))
					{
						previous_value = current_value;
						current_value = 0.5 * (current_value + (boosted_arg / current_value));
					}

					return static_cast<float>(current_value);
				}
			}

			// float uses double internally, double uses dd_real internally
			template <cxcm::concepts::basic_floating_point T>
			constexpr T inverse_sqrt(T arg) noexcept
			{
				// don't need this to be a dd_real
				const double boosted_arg = arg;

				if constexpr (std::is_same_v<T, double>)
				{
					// arg is already a double
					auto current_value = dd_real::dd_real(fast_rsqrt(arg));

					current_value *= (1.5 - ((0.5 * boosted_arg) * (current_value * current_value)));

					return static_cast<double>(current_value);
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					double current_value = fast_rsqrt(boosted_arg);

					current_value *= (1.5 - (0.5 * boosted_arg * current_value * current_value));

					// do a couple more refinements for floating point (this needs testing to see if necessary)
					current_value *= (1.5 - (0.5 * boosted_arg * current_value * current_value));
					current_value *= (1.5 - (0.5 * boosted_arg * current_value * current_value));

					return static_cast<float>(current_value);
				}
			}

		}

		// constexpr square root, uses higher precision behind the scenes
		template <cxcm::concepts::basic_floating_point T>
		constexpr T sqrt(T value) noexcept
		{
			return detail::converging_sqrt(value);
		}

		// reciprocal of square root, uses higher precision behind the scenes
		template <cxcm::concepts::basic_floating_point T>
		constexpr T rsqrt(T value) noexcept
		{
			return detail::inverse_sqrt(value);
		}

		// fast reciprocal of square root
		template <cxcm::concepts::basic_floating_point T>
		constexpr T fast_rsqrt(T value) noexcept
		{
			return static_cast<T>(detail::fast_rsqrt(static_cast<double>(value)));
		}

	} // namespace relaxed

	//
	// isnan()
	//

	// make sure this isn't optimized away if used with fast-math

#if defined(_MSC_VER) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER)
#pragma float_control(precise, on, push)
#endif

	template <cxcm::concepts::basic_floating_point T>
#if defined(__GNUC__) && !defined(__clang__)
	__attribute__((optimize("-fno-fast-math")))
#endif
	constexpr bool isnan(T value) noexcept
	{
		return (value != value);
	}

#if defined(_MSC_VER) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER)
#pragma float_control(pop)
#endif

	//
	// isinf()
	//

	// make sure this isn't optimized away if used with fast-math

#if defined(_MSC_VER) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER)
#pragma float_control(precise, on, push)
#endif

	template <cxcm::concepts::basic_floating_point T>
#if defined(__GNUC__) && !defined(__clang__)
	__attribute__((optimize("-fno-fast-math")))
#endif
	constexpr bool isinf(T value) noexcept
	{
		return (value == -std::numeric_limits<T>::infinity()) || (value == std::numeric_limits<T>::infinity());
	}

#if defined(_MSC_VER) || defined(__clang__) || defined(__INTEL_LLVM_COMPILER)
#pragma float_control(pop)
#endif

	//
	// fpclassify()
	//

	template <cxcm::concepts::basic_floating_point T>
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

	template <cxcm::concepts::basic_floating_point T>
	constexpr bool isnormal(T value) noexcept
	{
		return (fpclassify(value) == FP_NORMAL);
	}

	//
	// isfinite()
	//

	template <cxcm::concepts::basic_floating_point T>
	constexpr bool isfinite(T value) noexcept
	{
		return !isnan(value) && !isinf(value);
	}

	//
	// signbit()
	//

	// +0 returns false and -0 returns true
	template <cxcm::concepts::basic_floating_point T>
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
	template <cxcm::concepts::basic_floating_point T>
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
			// make_nan_quiet()
			//

			// make a NaN into a quiet NaN - if input is not a NaN, it is returned unchanged
			template <cxcm::concepts::basic_floating_point T>
			constexpr T convert_to_quiet_nan(T value) noexcept
			{
				if (cxcm::isnan(value))
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

				return value;
			}

			//
			// isnormal_or_subnormal()
			//

			// standard library screening requirement for these functions

			template <cxcm::concepts::basic_floating_point T>
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

			template <cxcm::concepts::basic_floating_point T>
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

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_trunc(T value) noexcept
			{
#if !defined(__GNUC__) || defined(__clang__)
				if (isnan(value))
					return convert_to_quiet_nan(value);
#endif

				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::trunc(value);
			}

			//
			// constexpr_floor()
			//

			// rounds towards negative infinity

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_floor(T value) noexcept
			{
#if !defined(__GNUC__) || defined(__clang__)
				if (isnan(value))
					return convert_to_quiet_nan(value);
#endif

				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::floor(value);
			}

			//
			// constexpr_ceil()
			//

			// rounds towards positive infinity

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_ceil(T value) noexcept
			{
#if !defined(__GNUC__) || defined(__clang__)
				if (isnan(value))
					return convert_to_quiet_nan(value);
#endif

				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::ceil(value);
			}

			//
			// constexpr_round()
			//

			// rounds to nearest integral position, halfway cases away from zero

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_round(T value) noexcept
			{
#if !defined(__GNUC__) || defined(__clang__)
				if (isnan(value))
					return convert_to_quiet_nan(value);
#endif

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

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_fract(T value) noexcept
			{
#if !defined(__GNUC__) || defined(__clang__)
				if (isnan(value))
					return convert_to_quiet_nan(value);
#endif

				// screen out unnecessary input
				if (fails_fractional_input_constraints(value))
					return value;

				return relaxed::fract(value);
			}

			//
			// constexpr_fmod()
			//

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_fmod(T x, T y) noexcept
			{
				// screen out unnecessary input

				if (isnan(x) || isnan(y) || !isfinite(x))
					return std::numeric_limits<T>::quiet_NaN();

				if (isinf(y))
					return x;

				if (x == T(0) && y != T(0))
					return x;

				if (y == 0)
					return std::numeric_limits<T>::quiet_NaN();

				return relaxed::fmod(x, y);
			}

			//
			// constexpr_round_even()
			//

			// rounds to nearest integral position, halfway cases away from zero

			template <cxcm::concepts::basic_floating_point T>
			constexpr T constexpr_round_even(T value) noexcept
			{
#if !defined(__GNUC__) || defined(__clang__)
				if (isnan(value))
					return convert_to_quiet_nan(value);
#endif

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

			// make sure this isn't optimized away if used with fast-math

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(precise, on, push)
#endif

			template <cxcm::concepts::basic_floating_point T>
#if defined(__GNUC__) && !defined(__clang__)
			__attribute__((optimize("-fno-fast-math")))
#endif
			constexpr T constexpr_sqrt(T value) noexcept
			{
				// screen out unnecessary input

				if (isnan(value))
				{
					return detail::convert_to_quiet_nan(value);
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

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(pop)
#endif

			//
			// constexpr_inverse_sqrt()
			//

			// make sure this isn't optimized away if used with fast-math

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(precise, on, push)
#endif

			template <cxcm::concepts::basic_floating_point T>
#if defined(__GNUC__) && !defined(__clang__)
			__attribute__((optimize("-fno-fast-math")))
#endif
			constexpr T constexpr_rsqrt(T value) noexcept
			{
				// screen out unnecessary input

				if (isnan(value))
				{
					return detail::convert_to_quiet_nan(value);
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

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(pop)
#endif

			// make sure this isn't optimized away if used with fast-math

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(precise, on, push)
#endif

			template <cxcm::concepts::basic_floating_point T>
#if defined(__GNUC__) && !defined(__clang__)
			__attribute__((optimize("-fno-fast-math")))
#endif
			constexpr T constexpr_fast_rsqrt(T value) noexcept
			{
				// screen out unnecessary input

				if (isnan(value))
				{
					return detail::convert_to_quiet_nan(value);
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

				return relaxed::fast_rsqrt(value);
			}

#if defined(_MSC_VER) || defined(__clang__)
#pragma float_control(pop)
#endif

		} // namespace detail

		//
		// abs(), fabs()
		//


		// absolute value

		template <cxcm::concepts::basic_floating_point T>
		constexpr T abs(T value) noexcept
		{
			auto new_value = cxcm::copysign(value, T(+1));

#if !defined(NDEBUG) && defined(_MSC_VER)
			if (isnan(new_value))
			{
				return detail::convert_to_quiet_nan(new_value);
			}
			else
			{
				return new_value;
			}
#else
			return new_value;
#endif
		}

		// don't know what to do if someone tries to negate the most negative number.
		// standard says behavior is undefined if you can't represent the result by return type.
		template <std::integral T>
		constexpr T abs(T value)
		{
			if (value == std::numeric_limits<T>::min())
			{
				throw std::domain_error("negation of min value is not a valid integral value");
			}

			return relaxed::abs(value);
		}

		template <cxcm::concepts::basic_floating_point T>
		constexpr T fabs(T value) noexcept
		{
			return cxcm::abs(value);
		}

		template <std::integral T>
		constexpr double fabs(T value)
		{
			if (value == std::numeric_limits<T>::min())
			{
				throw std::domain_error("negation of min value is not a valid integral value");
			}

			return relaxed::fabs(value);
		}

		//
		// trunc()
		//

		// rounds towards zero

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		template <cxcm::concepts::basic_floating_point T>
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

		// there is no standard c++ version of this, so always call constexpr version

		template <cxcm::concepts::basic_floating_point T>
		constexpr T rsqrt(T value) noexcept
		{
			return detail::constexpr_rsqrt(value);
		}

		//
		// fast_rsqrt() - fast good approximation to inverse square root
		//

		// there is no standard c++ version of this, so always call constexpr version

		template <cxcm::concepts::basic_floating_point T>
		constexpr T fast_rsqrt(T value) noexcept
		{
			return detail::constexpr_fast_rsqrt(value);
		}

	} // namespace strict

} // namespace cxcm

// closing include guard
#endif
