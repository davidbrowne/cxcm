#pragma once

//          Copyright David Browne 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <limits>
#include <type_traits>
#include <cmath>
#include <concepts>

namespace cxcm
{

	namespace limits
	{
		namespace detail
		{
			// long doubles vary between compilers and platforms. Windows MSVC and clang on Windows both use
			// the same representation as double. For gcc and linux, etc., it is often represented by an extended
			// precision data structure with 80 bits (64 bits of significand). sizeof(long double) on gcc on Windows
			// (at least MSYS2) is 16, implying it is 128 bits, but std::numeric_limits<long double> returns values
			// consistent with an 80 bit representation.
			constexpr long double get_largest_fractional_long_double()
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

		template <std::floating_point T>
		constexpr T fabs(T value) noexcept
		{
			return abs(value);
		}

		//
		// trunc()
		//

		// this is the workhorse function for floor(), ceil(), and round().

		// rounds towards zero

		// works for double and long double, as long as:
		// std::numeric_limits<long double>::digits <= 64

		template <std::floating_point T>
		constexpr T trunc(T value) noexcept requires (std::numeric_limits<T>::digits <= 64)
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

			// this attempt at branchless code actually is slower:
//			return (truncated_value - T(truncated_value > value));

			// using the ternary operator doesn't speed it up
//			return (truncated_value > value) ? (truncated_value - T(1.0f)) : truncated_value;

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

			// this attempt at branchless code actually is slower:
//			return (truncated_value + T(truncated_value < value));

			// using the ternary operator doesn't speed it up
//			return (truncated_value < value) ? (truncated_value + T(1.0f)) : truncated_value;

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

	} // namespace relaxed

	//
	// isnan()
	//

	// I've seen comments on the Microsoft/STL Github Issue that tracks where they are implementing
	// std::isnan for c++20, and said that on some compilers with various compiler switches, what
	// we are using here, (x != x) or !(x == x), can be optimized away, so this is not a good practice. So
	// microsoft is in the process of making this stuff constexpr, and they need to have a compiler
	// intrinsic to do it. https://github.com/microsoft/STL/issues/65#issuecomment-563886838

#if defined(_MSC_VER)
#pragma float_control(precise, on, push)
#endif

	template <std::floating_point T>
	constexpr bool isnan(T value) noexcept
	{
		return (value != value);
	}

#if defined(_MSC_VER)
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

		} // namespace detail

		//
		// abs(), fabs()
		//

#if 1

		// absolute value

		template <std::floating_point T>
		constexpr T abs(T value) noexcept
		{
			if (!detail::isnormal_or_subnormal(value))
				return value;

			return relaxed::abs(value);
		}

#else

		// absolute value
		// better optimized, but rejects philosophy of leaving the details to relaxed namespace

		template <std::floating_point T>
		constexpr T abs(T value) noexcept
		{
			if (!detail::isnormal_or_subnormal(value) || (value > T(0)))
				return value;

			return -value;
		}

#endif

		template <std::floating_point T>
		constexpr T fabs(T value) noexcept
		{
			return abs(value);
		}


// if this is defined, potentially use other functions for non constant evaluated contexts.
// comment out this definition if we want just the one constexpr version.
#define ALLOW_NON_CONST_EVAL_SPECIALIZATIONS

		//
		// trunc()
		//

#if defined(ALLOW_NON_CONST_EVAL_SPECIALIZATIONS) && (defined(_DEBUG) || defined(_M_IX86))

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

#else

		// rounds towards zero

		template <std::floating_point T>
		constexpr T trunc(T value) noexcept
		{
			return detail::constexpr_trunc(value);
		}

#endif

		//
		// floor()
		//

#if defined(ALLOW_NON_CONST_EVAL_SPECIALIZATIONS) && (defined(_DEBUG) || defined(_M_IX86))

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

#else

		// rounds towards negative infinity

		template <std::floating_point T>
		constexpr T floor(T value) noexcept
		{
			return detail::constexpr_floor(value);
		}

#endif

		//
		// ceil()
		//

#if defined(ALLOW_NON_CONST_EVAL_SPECIALIZATIONS) && (defined(_DEBUG) || defined(_M_IX86))

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

#else

		// rounds towards positive infinity

		template <std::floating_point T>
		constexpr T ceil(T value) noexcept
		{
			return detail::constexpr_ceil(value);
		}

#endif

		//
		// round()
		//

#if defined(ALLOW_NON_CONST_EVAL_SPECIALIZATIONS) && (defined(_DEBUG) || defined(_M_IX86))

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

#else

		// rounds to nearest integral position, halfway cases away from zero

		template <std::floating_point T>
		constexpr T round(T value) noexcept
		{
			return detail::constexpr_round(value);
		}

#endif

	} // namespace strict

} // namespace cxcm
