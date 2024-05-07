
//          Copyright David Browne 2020-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "cxcm.hxx"
#include <numbers>
#include <climits>


#if defined(__clang__)
// clang 10.0 does not like colors on windows (link problems with isatty and fileno)
#define DOCTEST_CONFIG_COLORS_NONE
#endif
//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_SUITE("constexpr_math for long double")
{
	TEST_CASE("floating-point negative zero")
	{
		CHECK(cxcm::is_negative_zero(cxcm::negative_zero<double>));
		CHECK(cxcm::is_negative_zero(cxcm::negative_zero<float>));
		CHECK_UNARY_FALSE(cxcm::is_negative_zero(0.0));
		CHECK_UNARY_FALSE(cxcm::is_negative_zero(0.0f));
	}

	constexpr bool extended_precision_long_double = std::numeric_limits<long double>::digits == 64;

	TEST_CASE("testing fidelity of cxcm::trunc() with std::trunc() with long double values")
	{
		// +/- 0
		CHECK(cxcm::trunc(0.0L) == std::trunc(0.0L));
		CHECK(cxcm::trunc(-0.0L) == std::trunc(-0.0L));

		// +/- infinity
		CHECK(cxcm::trunc(std::numeric_limits<long double>::infinity()) == std::trunc(std::numeric_limits<long double>::infinity()));
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::infinity()) == std::trunc(-std::numeric_limits<long double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::trunc(std::numeric_limits<long double>::quiet_NaN())) == std::isnan(std::trunc(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::trunc(std::numeric_limits<long double>::denorm_min()) == std::trunc(std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::denorm_min()) == std::trunc(-std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::trunc(std::numeric_limits<long double>::epsilon()) == std::trunc(std::numeric_limits<long double>::epsilon()));
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::epsilon()) == std::trunc(-std::numeric_limits<long double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::trunc(std::numeric_limits<long double>::min()) == std::trunc(std::numeric_limits<long double>::min()));
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::min()) == std::trunc(-std::numeric_limits<long double>::min()));
		CHECK(cxcm::trunc(std::numeric_limits<long double>::max()) == std::trunc(std::numeric_limits<long double>::max()));
		CHECK(cxcm::trunc(std::numeric_limits<long double>::lowest()) == std::trunc(std::numeric_limits<long double>::lowest()));

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::trunc(0x1.fffffffffffffffep+62L) == std::trunc(0x1.fffffffffffffffep+62L));
			CHECK(cxcm::trunc(-0x1.fffffffffffffffep+62L) == std::trunc(-0x1.fffffffffffffffep+62L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::trunc(0x1.0000000000000002p+63L) == std::trunc(0x1.0000000000000002p+63L));
			CHECK(cxcm::trunc(-0x1.0000000000000002p+63L) == std::trunc(-0x1.0000000000000002p+63L));
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::trunc(0x1.fffffffffffffp+51L) == std::trunc(0x1.fffffffffffffp+51L));
			CHECK(cxcm::trunc(-0x1.fffffffffffffp+51L) == std::trunc(-0x1.fffffffffffffp+51L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::trunc(0x1.0000000000001p+52L) == std::trunc(0x1.0000000000001p+52L));
			CHECK(cxcm::trunc(-0x1.0000000000001p+52L) == std::trunc(-0x1.0000000000001p+52L));
		}

		// long double value that overflows a long long
		CHECK(cxcm::trunc(0x1.0p+63L) == std::trunc(0x1.0p+63L));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::trunc(0.5L) == std::trunc(0.5L));
		CHECK(cxcm::trunc(-0.5L) == std::trunc(-0.5L));

		// other values near 0
		CHECK(cxcm::trunc(0.25L) == std::trunc(0.25L));
		CHECK(cxcm::trunc(-0.25L) == std::trunc(-0.25L));
		CHECK(cxcm::trunc(0.75L) == std::trunc(0.75L));
		CHECK(cxcm::trunc(-0.75L) == std::trunc(-0.75L));

		// simple values with no fractional part
		CHECK(cxcm::trunc(-4.0L) == std::trunc(-4.0L));
		CHECK(cxcm::trunc(125.0L) == std::trunc(125.0L));

		// simple values with fractional parts
		CHECK(cxcm::trunc(-1.825L) == std::trunc(-1.825L));
		CHECK(cxcm::trunc(13.75L) == std::trunc(13.75L));
	}

	TEST_CASE("testing cxcm::trunc() long double values")
	{
		// +/- 0
		CHECK(cxcm::trunc(0.0L) == 0.0L);
		CHECK(cxcm::trunc(-0.0L) == -0.0L);

		// +/- infinity
		CHECK(cxcm::trunc(std::numeric_limits<long double>::infinity()) == std::numeric_limits<long double>::infinity());
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::infinity()) == -std::numeric_limits<long double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::trunc(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::trunc(std::numeric_limits<long double>::denorm_min()) == 0.0L);
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::denorm_min()) == -0.0L);
		CHECK(cxcm::trunc(std::numeric_limits<long double>::epsilon()) == 0.0L);
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::epsilon()) == -0.0L);

		// +/- min and max normal values
		CHECK(cxcm::trunc(std::numeric_limits<long double>::min()) == 0.0L);
		CHECK(cxcm::trunc(-std::numeric_limits<long double>::min()) == -0.0L);
		CHECK(cxcm::trunc(std::numeric_limits<long double>::max()) == std::numeric_limits<long double>::max());
		CHECK(cxcm::trunc(std::numeric_limits<long double>::lowest()) == std::numeric_limits<long double>::lowest());

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::trunc(0x1.fffffffffffffffep+62L) == 0x1.fffffffffffffffcp+62L);
			CHECK(cxcm::trunc(-0x1.fffffffffffffffep+62L) == -0x1.fffffffffffffffcp+62L);
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::trunc(0x1.0000000000000002p+63L) == 0x1.0000000000000002p+63L);
			CHECK(cxcm::trunc(-0x1.0000000000000002p+63L) == -0x1.0000000000000002p+63L);
		}

		// +/- value where distance between adjacent long doubles is 1.0
		CHECK(cxcm::trunc(0x1.0000000000001p+52L) == 0x1.0000000000001p+52L);
		CHECK(cxcm::trunc(-0x1.0000000000001p+52L) == -0x1.0000000000001p+52L);

		// long double value that overflows a long long
		CHECK(cxcm::trunc(0x1.0p+63L) == 0x1.0p+63L);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::trunc(0.5L) == 0.0L);
		CHECK(cxcm::trunc(-0.5L) == -0.0L);

		// other values near 0
		CHECK(cxcm::trunc(0.25L) == 0.0L);
		CHECK(cxcm::trunc(-0.25L) == -0.0L);
		CHECK(cxcm::trunc(0.75L) == 0.0L);
		CHECK(cxcm::trunc(-0.75L) == -0.0L);

		// simple values with no fractional part
		CHECK(cxcm::trunc(-4.0L) == -4.0L);
		CHECK(cxcm::trunc(125.0L) == 125.0L);

		// simple values with fractional parts
		CHECK(cxcm::trunc(-1.825L) == -1.0L);
		CHECK(cxcm::trunc(13.75L) == 13.0L);
	}

	TEST_CASE("testing fidelity of cxcm::floor() with std::floor() with long double values")
	{
		// +/- 0
		CHECK(cxcm::floor(0.0L) == std::floor(0.0L));
		CHECK(cxcm::floor(-0.0L) == std::floor(-0.0L));

		// +/- infinity
		CHECK(cxcm::floor(std::numeric_limits<long double>::infinity()) == std::floor(std::numeric_limits<long double>::infinity()));
		CHECK(cxcm::floor(-std::numeric_limits<long double>::infinity()) == std::floor(-std::numeric_limits<long double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::floor(std::numeric_limits<long double>::quiet_NaN())) == std::isnan(std::floor(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::floor(std::numeric_limits<long double>::denorm_min()) == std::floor(std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::floor(-std::numeric_limits<long double>::denorm_min()) == std::floor(-std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::floor(std::numeric_limits<long double>::epsilon()) == std::floor(std::numeric_limits<long double>::epsilon()));
		CHECK(cxcm::floor(-std::numeric_limits<long double>::epsilon()) == std::floor(-std::numeric_limits<long double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::floor(std::numeric_limits<long double>::min()) == std::floor(std::numeric_limits<long double>::min()));
		CHECK(cxcm::floor(-std::numeric_limits<long double>::min()) == std::floor(-std::numeric_limits<long double>::min()));
		CHECK(cxcm::floor(std::numeric_limits<long double>::max()) == std::floor(std::numeric_limits<long double>::max()));
		CHECK(cxcm::floor(std::numeric_limits<long double>::lowest()) == std::floor(std::numeric_limits<long double>::lowest()));

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::floor(0x1.fffffffffffffffep+62L) == std::floor(0x1.fffffffffffffffep+62L));
			CHECK(cxcm::floor(-0x1.fffffffffffffffep+62L) == std::floor(-0x1.fffffffffffffffep+62L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::floor(0x1.0000000000000002p+63L) == std::floor(0x1.0000000000000002p+63L));
			CHECK(cxcm::floor(-0x1.0000000000000002p+63L) == std::floor(-0x1.0000000000000002p+63L));
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::floor(0x1.fffffffffffffp+51L) == std::floor(0x1.fffffffffffffp+51L));
			CHECK(cxcm::floor(-0x1.fffffffffffffp+51L) == std::floor(-0x1.fffffffffffffp+51L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::floor(0x1.0000000000001p+52L) == std::floor(0x1.0000000000001p+52L));
			CHECK(cxcm::floor(-0x1.0000000000001p+52L) == std::floor(-0x1.0000000000001p+52L));
		}

		// long double value that overflows a long long
		CHECK(cxcm::floor(0x1.0p+63L) == std::floor(0x1.0p+63L));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::floor(0.5L) == std::floor(0.5L));
		CHECK(cxcm::floor(-0.5L) == std::floor(-0.5L));

		// other values near 0
		CHECK(cxcm::floor(0.25L) == std::floor(0.25L));
		CHECK(cxcm::floor(-0.25L) == std::floor(-0.25L));
		CHECK(cxcm::floor(0.75L) == std::floor(0.75L));
		CHECK(cxcm::floor(-0.75L) == std::floor(-0.75L));

		// simple values with no fractional part
		CHECK(cxcm::floor(-4.0L) == std::floor(-4.0L));
		CHECK(cxcm::floor(125.0L) == std::floor(125.0L));

		// simple values with fractional parts
		CHECK(cxcm::floor(-1.825L) == std::floor(-1.825L));
		CHECK(cxcm::floor(13.75L) == std::floor(13.75L));
	}

	TEST_CASE("testing cxcm::floor() long double values")
	{
		// +/- 0
		CHECK(cxcm::floor(0.0L) == 0.0L);
		CHECK(cxcm::floor(-0.0L) == -0.0L);

		// +/- infinity
		CHECK(cxcm::floor(std::numeric_limits<long double>::infinity()) == std::numeric_limits<long double>::infinity());
		CHECK(cxcm::floor(-std::numeric_limits<long double>::infinity()) == -std::numeric_limits<long double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::floor(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::floor(std::numeric_limits<long double>::denorm_min()) == 0.0L);
		CHECK(cxcm::floor(-std::numeric_limits<long double>::denorm_min()) == -1.0L);
		CHECK(cxcm::floor(std::numeric_limits<long double>::epsilon()) == 0.0L);
		CHECK(cxcm::floor(-std::numeric_limits<long double>::epsilon()) == -1.0L);

		// +/- min and max normal values
		CHECK(cxcm::floor(std::numeric_limits<long double>::min()) == 0.0L);
		CHECK(cxcm::floor(-std::numeric_limits<long double>::min()) == -1.0L);
		CHECK(cxcm::floor(std::numeric_limits<long double>::max()) == std::numeric_limits<long double>::max());
		CHECK(cxcm::floor(std::numeric_limits<long double>::lowest()) == std::numeric_limits<long double>::lowest());

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::floor(0x1.fffffffffffffffep+62L) == 0x1.fffffffffffffffcp+62L);
			CHECK(cxcm::floor(-0x1.fffffffffffffffep+62L) == -0x1.0p+63L);

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::floor(0x1.0000000000000002p+63L) == 0x1.0000000000000002p+63L);
			CHECK(cxcm::floor(-0x1.0000000000000002p+63L) == -0x1.0000000000000002p+63L);
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::floor(0x1.fffffffffffffp+51L) == 0x1.ffffffffffffep+51L);
			CHECK(cxcm::floor(-0x1.fffffffffffffp+51L) == -0x1.0p+52L);

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::floor(0x1.0000000000001p+52L) == 0x1.0000000000001p+52L);
			CHECK(cxcm::floor(-0x1.0000000000001p+52L) == -0x1.0000000000001p+52L);
		}

		// long double value that overflows a long long
		CHECK(cxcm::floor(0x1.0p+63L) == 0x1.0p+63L);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::floor(0.5L) == 0.0L);
		CHECK(cxcm::floor(-0.5L) == -1.0L);

		// other values near 0
		CHECK(cxcm::floor(0.25L) == 0.0L);
		CHECK(cxcm::floor(-0.25L) == -1.0L);
		CHECK(cxcm::floor(0.75L) == 0.0L);
		CHECK(cxcm::floor(-0.75L) == -1.0L);

		// simple values with no fractional part
		CHECK(cxcm::floor(-4.0L) == -4.0L);
		CHECK(cxcm::floor(125.0L) == 125.0L);

		// simple values with fractional parts
		CHECK(cxcm::floor(-1.825L) == -2.0L);
		CHECK(cxcm::floor(13.75L) == 13.0L);
	}

	TEST_CASE("testing fidelity of cxcm::ceil() with std::ceil() with long double values")
	{
		// +/- 0
		CHECK(cxcm::ceil(0.0L) == std::ceil(0.0L));
		CHECK(cxcm::ceil(-0.0L) == std::ceil(-0.0L));

		// +/- infinity
		CHECK(cxcm::ceil(std::numeric_limits<long double>::infinity()) == std::ceil(std::numeric_limits<long double>::infinity()));
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::infinity()) == std::ceil(-std::numeric_limits<long double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::ceil(std::numeric_limits<long double>::quiet_NaN())) == std::isnan(std::ceil(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::ceil(std::numeric_limits<long double>::denorm_min()) == std::ceil(std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::denorm_min()) == std::ceil(-std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::ceil(std::numeric_limits<long double>::epsilon()) == std::ceil(std::numeric_limits<long double>::epsilon()));
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::epsilon()) == std::ceil(-std::numeric_limits<long double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::ceil(std::numeric_limits<long double>::min()) == std::ceil(std::numeric_limits<long double>::min()));
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::min()) == std::ceil(-std::numeric_limits<long double>::min()));
		CHECK(cxcm::ceil(std::numeric_limits<long double>::max()) == std::ceil(std::numeric_limits<long double>::max()));
		CHECK(cxcm::ceil(std::numeric_limits<long double>::lowest()) == std::ceil(std::numeric_limits<long double>::lowest()));

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::ceil(0x1.fffffffffffffffep+62L) == std::ceil(0x1.fffffffffffffffep+62L));
			CHECK(cxcm::ceil(-0x1.fffffffffffffffep+62L) == std::ceil(-0x1.fffffffffffffffep+62L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::ceil(0x1.0000000000000002p+63L) == std::ceil(0x1.0000000000000002p+63L));
			CHECK(cxcm::ceil(-0x1.0000000000000002p+63L) == std::ceil(-0x1.0000000000000002p+63L));
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::ceil(0x1.fffffffffffffp+51L) == std::ceil(0x1.fffffffffffffp+51L));
			CHECK(cxcm::ceil(-0x1.fffffffffffffp+51L) == std::ceil(-0x1.fffffffffffffp+51L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::ceil(0x1.0000000000001p+52L) == std::ceil(0x1.0000000000001p+52L));
			CHECK(cxcm::ceil(-0x1.0000000000001p+52L) == std::ceil(-0x1.0000000000001p+52L));
		}

		// long double value that overflows a long long
		CHECK(cxcm::ceil(0x1.0p+63L) == std::ceil(0x1.0p+63L));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::ceil(0.5L) == std::ceil(0.5L));
		CHECK(cxcm::ceil(-0.5L) == std::ceil(-0.5L));

		// other values near 0
		CHECK(cxcm::ceil(0.25L) == std::ceil(0.25L));
		CHECK(cxcm::ceil(-0.25L) == std::ceil(-0.25L));
		CHECK(cxcm::ceil(0.75L) == std::ceil(0.75L));
		CHECK(cxcm::ceil(-0.75L) == std::ceil(-0.75L));

		// simple values with no fractional part
		CHECK(cxcm::ceil(-4.0L) == std::ceil(-4.0L));
		CHECK(cxcm::ceil(125.0L) == std::ceil(125.0L));

		// simple values with fractional parts
		CHECK(cxcm::ceil(-1.825L) == std::ceil(-1.825L));
		CHECK(cxcm::ceil(13.75L) == std::ceil(13.75L));
	}

	TEST_CASE("testing cxcm::ceil() long double values")
	{
		// +/- 0
		CHECK(cxcm::ceil(0.0L) == 0.0L);
		CHECK(cxcm::ceil(-0.0L) == -0.0L);

		// +/- infinity
		CHECK(cxcm::ceil(std::numeric_limits<long double>::infinity()) == std::numeric_limits<long double>::infinity());
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::infinity()) == -std::numeric_limits<long double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::ceil(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::ceil(std::numeric_limits<long double>::denorm_min()) == 1.0L);
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::denorm_min()) == -0.0L);
		CHECK(cxcm::ceil(std::numeric_limits<long double>::epsilon()) == 1.0L);
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::epsilon()) == -0.0L);

		// +/- min and max normal values
		CHECK(cxcm::ceil(std::numeric_limits<long double>::min()) == 1.0L);
		CHECK(cxcm::ceil(-std::numeric_limits<long double>::min()) == -0.0L);
		CHECK(cxcm::ceil(std::numeric_limits<long double>::max()) == std::numeric_limits<long double>::max());
		CHECK(cxcm::ceil(std::numeric_limits<long double>::lowest()) == std::numeric_limits<long double>::lowest());

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::ceil(0x1.fffffffffffffffep+62L) == 0x1.0p+63L);
			CHECK(cxcm::ceil(-0x1.fffffffffffffffep+62L) == -0x1.fffffffffffffffcp+62L);

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::ceil(0x1.0000000000000002p+63L) == 0x1.0000000000000002p+63L);
			CHECK(cxcm::ceil(-0x1.0000000000000002p+63L) == -0x1.0000000000000002p+63L);
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::ceil(0x1.fffffffffffffp+51L) == 0x1.0p+52L);
			CHECK(cxcm::ceil(-0x1.fffffffffffffp+51L) == -0x1.ffffffffffffep+51L);

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::ceil(0x1.0000000000001p+52L) == 0x1.0000000000001p+52L);
			CHECK(cxcm::ceil(-0x1.0000000000001p+52L) == -0x1.0000000000001p+52L);
		}

		// long double value that overflows a long long
		CHECK(cxcm::ceil(0x1.0p+63L) == 0x1.0p+63L);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::ceil(0.5L) == 1.0L);
		CHECK(cxcm::ceil(-0.5L) == -0.0L);

		// other values near 0
		CHECK(cxcm::ceil(0.25L) == 1.0L);
		CHECK(cxcm::ceil(-0.25L) == -0.0L);
		CHECK(cxcm::ceil(0.75L) == 1.0L);
		CHECK(cxcm::ceil(-0.75L) == -0.0L);

		// simple values with no fractional part
		CHECK(cxcm::ceil(-4.0L) == -4.0L);
		CHECK(cxcm::ceil(125.0L) == 125.0L);

		// simple values with fractional parts
		CHECK(cxcm::ceil(-1.825L) == -1.0L);
		CHECK(cxcm::ceil(13.75L) == 14.0L);
	}

	TEST_CASE("testing fidelity of cxcm::round() with std::round() with long double values")
	{
		// +/- 0
		CHECK(cxcm::round(0.0L) == std::round(0.0L));
		CHECK(cxcm::round(-0.0L) == std::round(-0.0L));

		// +/- infinity
		CHECK(cxcm::round(std::numeric_limits<long double>::infinity()) == std::round(std::numeric_limits<long double>::infinity()));
		CHECK(cxcm::round(-std::numeric_limits<long double>::infinity()) == std::round(-std::numeric_limits<long double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::round(std::numeric_limits<long double>::quiet_NaN())) == std::isnan(std::round(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round(std::numeric_limits<long double>::denorm_min()) == std::round(std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::round(-std::numeric_limits<long double>::denorm_min()) == std::round(-std::numeric_limits<long double>::denorm_min()));
		CHECK(cxcm::round(std::numeric_limits<long double>::epsilon()) == std::round(std::numeric_limits<long double>::epsilon()));
		CHECK(cxcm::round(-std::numeric_limits<long double>::epsilon()) == std::round(-std::numeric_limits<long double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::round(std::numeric_limits<long double>::min()) == std::round(std::numeric_limits<long double>::min()));
		CHECK(cxcm::round(-std::numeric_limits<long double>::min()) == std::round(-std::numeric_limits<long double>::min()));
		CHECK(cxcm::round(std::numeric_limits<long double>::max()) == std::round(std::numeric_limits<long double>::max()));
		CHECK(cxcm::round(std::numeric_limits<long double>::lowest()) == std::round(std::numeric_limits<long double>::lowest()));

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::round(0x1.fffffffffffffffep+62L) == std::round(0x1.fffffffffffffffep+62L));
			CHECK(cxcm::round(-0x1.fffffffffffffffep+62L) == std::round(-0x1.fffffffffffffffep+62L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::round(0x1.0000000000000002p+63L) == std::round(0x1.0000000000000002p+63L));
			CHECK(cxcm::round(-0x1.0000000000000002p+63L) == std::round(-0x1.0000000000000002p+63L));
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::round(0x1.fffffffffffffp+51L) == std::round(0x1.fffffffffffffp+51L));
			CHECK(cxcm::round(-0x1.fffffffffffffp+51L) == std::round(-0x1.fffffffffffffp+51L));

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::round(0x1.0000000000001p+52L) == std::round(0x1.0000000000001p+52L));
			CHECK(cxcm::round(-0x1.0000000000001p+52L) == std::round(-0x1.0000000000001p+52L));
		}

		// long double value that overflows a long long
		CHECK(cxcm::round(0x1.0p+63L) == std::round(0x1.0p+63L));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::round(0.5L) == std::round(0.5L));
		CHECK(cxcm::round(-0.5L) == std::round(-0.5L));

		// other values near 0
		CHECK(cxcm::round(0.25L) == std::round(0.25L));
		CHECK(cxcm::round(-0.25L) == std::round(-0.25L));
		CHECK(cxcm::round(0.75L) == std::round(0.75L));
		CHECK(cxcm::round(-0.75L) == std::round(-0.75L));

		// simple values with no fractional part
		CHECK(cxcm::round(-4.0L) == std::round(-4.0L));
		CHECK(cxcm::round(125.0L) == std::round(125.0L));

		// simple values with fractional parts
		CHECK(cxcm::round(-1.825L) == std::round(-1.825L));
		CHECK(cxcm::round(13.75L) == std::round(13.75L));
	}

	TEST_CASE("testing cxcm::round() long double values")
	{
		// +/- 0
		CHECK(cxcm::round(0.0L) == 0.0L);
		CHECK(cxcm::round(-0.0L) == -0.0L);

		// +/- infinity
		CHECK(cxcm::round(std::numeric_limits<long double>::infinity()) == std::numeric_limits<long double>::infinity());
		CHECK(cxcm::round(-std::numeric_limits<long double>::infinity()) == -std::numeric_limits<long double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::round(std::numeric_limits<long double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round(std::numeric_limits<long double>::denorm_min()) == 0.0L);
		CHECK(cxcm::round(-std::numeric_limits<long double>::denorm_min()) == -0.0L);
		CHECK(cxcm::round(std::numeric_limits<long double>::epsilon()) == 0.0L);
		CHECK(cxcm::round(-std::numeric_limits<long double>::epsilon()) == -0.0L);

		// +/- min and max normal values
		CHECK(cxcm::round(std::numeric_limits<long double>::min()) == 0.0L);
		CHECK(cxcm::round(-std::numeric_limits<long double>::min()) == -0.0L);
		CHECK(cxcm::round(std::numeric_limits<long double>::max()) == std::numeric_limits<long double>::max());
		CHECK(cxcm::round(std::numeric_limits<long double>::lowest()) == std::numeric_limits<long double>::lowest());

		if (extended_precision_long_double)
		{
			// +/- largest fractional long double
			CHECK(cxcm::round(0x1.fffffffffffffffep+62L) == 0x1.0p+63L);
			CHECK(cxcm::round(-0x1.fffffffffffffffep+62L) == -0x1.0p+63L);

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::round(0x1.0000000000000002p+63L) == 0x1.0000000000000002p+63L);
			CHECK(cxcm::round(-0x1.0000000000000002p+63L) == -0x1.0000000000000002p+63L);
		}
		else
		{
			// +/- largest fractional long double
			CHECK(cxcm::round(0x1.fffffffffffffp+51L) == 0x1.0p+52L);
			CHECK(cxcm::round(-0x1.fffffffffffffp+51L) == -0x1.0p+52L);

			// +/- value where distance between adjacent long doubles is 1.0
			CHECK(cxcm::round(0x1.0000000000001p+52L) == 0x1.0000000000001p+52L);
			CHECK(cxcm::round(-0x1.0000000000001p+52L) == -0x1.0000000000001p+52L);
		}

		// long double value that overflows a long long
		CHECK(cxcm::round(0x1.0p+63L) == 0x1.0p+63L);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::round(0.5L) == 1.0L);
		CHECK(cxcm::round(-0.5L) == -1.0L);

		// other values near 0
		CHECK(cxcm::round(0.25L) == 0.0L);
		CHECK(cxcm::round(-0.25L) == -0.0L);
		CHECK(cxcm::round(0.75L) == 1.0L);
		CHECK(cxcm::round(-0.75L) == -1.0L);

		// simple values with no fractional part
		CHECK(cxcm::round(-4.0L) == -4.0L);
		CHECK(cxcm::round(125.0L) == 125.0L);

		// simple values with fractional parts
		CHECK(cxcm::round(-1.825L) == -2.0L);
		CHECK(cxcm::round(13.75L) == 14.0L);
	}

}

TEST_SUITE("constexpr_math for double")
{

	TEST_CASE("testing fidelity of cxcm::signbit() with std::signbit() for double values")
	{
		auto pos_nan = std::numeric_limits<double>::quiet_NaN();
		auto neg_nan = -std::numeric_limits<double>::quiet_NaN();
		auto pos_inf = std::numeric_limits<double>::infinity();
		auto neg_inf = -std::numeric_limits<double>::infinity();
		double pos_zero = 0;
		double neg_zero = std::copysign(0.0, -1.0);
		double pos_four = 4;
		double neg_four = -4;

		// +/- 0
		CHECK(cxcm::signbit(pos_zero) == std::signbit(pos_zero));
		CHECK(cxcm::signbit(neg_zero) == std::signbit(neg_zero));

		// +/- infinity
		CHECK(cxcm::signbit(pos_inf) == std::signbit(pos_inf));
		CHECK(cxcm::signbit(neg_inf) == std::signbit(neg_inf));

		// NaN
		CHECK(cxcm::signbit(pos_nan) == std::signbit(pos_nan));
		CHECK(cxcm::signbit(neg_nan) == std::signbit(neg_nan));

		// +/- 4
		CHECK(cxcm::signbit(pos_four) == std::signbit(pos_four));
		CHECK(cxcm::signbit(neg_four) == std::signbit(neg_four));
	}

	TEST_CASE("testing fidelity of cxcm::copysign() with std::copysign() for double values")
	{
		auto pos_nan = std::numeric_limits<double>::quiet_NaN();
		auto neg_nan = -std::numeric_limits<double>::quiet_NaN();
		auto pos_inf = std::numeric_limits<double>::infinity();
		auto neg_inf = -std::numeric_limits<double>::infinity();
		double pos_zero = 0;
		double neg_zero = std::copysign(0.0, -1.0);
		double pos_four = 4;
		double neg_four = -4;

		// +/- 0
		CHECK(cxcm::copysign(pos_four, pos_zero) == std::copysign(pos_four, pos_zero));
		CHECK(cxcm::copysign(neg_four, pos_zero) == std::copysign(neg_four, pos_zero));
		CHECK(cxcm::copysign(pos_four, neg_zero) == std::copysign(pos_four, neg_zero));
		CHECK(cxcm::copysign(neg_four, neg_zero) == std::copysign(neg_four, neg_zero));

		// +/- infinity
		CHECK(cxcm::copysign(pos_four, pos_inf) == std::copysign(pos_four, pos_inf));
		CHECK(cxcm::copysign(neg_four, pos_inf) == std::copysign(neg_four, pos_inf));
		CHECK(cxcm::copysign(pos_four, neg_inf) == std::copysign(pos_four, neg_inf));
		CHECK(cxcm::copysign(neg_four, neg_inf) == std::copysign(neg_four, neg_inf));

		CHECK(cxcm::copysign(pos_inf, pos_four) == std::copysign(pos_inf, pos_four));
		CHECK(cxcm::copysign(pos_inf, neg_four) == std::copysign(pos_inf, neg_four));
		CHECK(cxcm::copysign(neg_inf, pos_four) == std::copysign(neg_inf, pos_four));
		CHECK(cxcm::copysign(neg_inf, neg_four) == std::copysign(neg_inf, neg_four));

		// NaN
		CHECK(cxcm::copysign(pos_four, pos_nan) == std::copysign(pos_four, pos_nan));
		CHECK(cxcm::copysign(neg_four, pos_nan) == std::copysign(neg_four, pos_nan));
		CHECK(cxcm::copysign(pos_four, neg_nan) == std::copysign(pos_four, neg_nan));
		CHECK(cxcm::copysign(neg_four, neg_nan) == std::copysign(neg_four, neg_nan));

		// +/- 4
		CHECK(cxcm::copysign(pos_four, pos_four) == std::copysign(pos_four, pos_four));
		CHECK(cxcm::copysign(neg_four, pos_four) == std::copysign(neg_four, pos_four));
		CHECK(cxcm::copysign(pos_four, neg_four) == std::copysign(pos_four, neg_four));
		CHECK(cxcm::copysign(neg_four, neg_four) == std::copysign(neg_four, neg_four));
	}

	TEST_CASE("testing fidelity of cxcm::trunc() with std::trunc() with double values")
	{
		// +/- 0
		CHECK(cxcm::trunc(0.0) == std::trunc(0.0));
		CHECK(cxcm::trunc(-0.0) == std::trunc(-0.0));

		// +/- infinity
		CHECK(cxcm::trunc(std::numeric_limits<double>::infinity()) == std::trunc(std::numeric_limits<double>::infinity()));
		CHECK(cxcm::trunc(-std::numeric_limits<double>::infinity()) == std::trunc(-std::numeric_limits<double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::trunc(std::numeric_limits<double>::quiet_NaN())) == std::isnan(std::trunc(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::trunc(std::numeric_limits<double>::denorm_min()) == std::trunc(std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::trunc(-std::numeric_limits<double>::denorm_min()) == std::trunc(-std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::trunc(std::numeric_limits<double>::epsilon()) == std::trunc(std::numeric_limits<double>::epsilon()));
		CHECK(cxcm::trunc(-std::numeric_limits<double>::epsilon()) == std::trunc(-std::numeric_limits<double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::trunc(std::numeric_limits<double>::min()) == std::trunc(std::numeric_limits<double>::min()));
		CHECK(cxcm::trunc(-std::numeric_limits<double>::min()) == std::trunc(-std::numeric_limits<double>::min()));
		CHECK(cxcm::trunc(std::numeric_limits<double>::max()) == std::trunc(std::numeric_limits<double>::max()));
		CHECK(cxcm::trunc(std::numeric_limits<double>::lowest()) == std::trunc(std::numeric_limits<double>::lowest()));

		// +/- largest fractional double
		CHECK(cxcm::trunc(0x1.fffffffffffffp+51) == std::trunc(0x1.fffffffffffffp+51));
		CHECK(cxcm::trunc(-0x1.fffffffffffffp+51) == std::trunc(-0x1.fffffffffffffp+51));

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::trunc(0x1.0000000000001p+52) == std::trunc(0x1.0000000000001p+52));
		CHECK(cxcm::trunc(-0x1.0000000000001p+52) == std::trunc(-0x1.0000000000001p+52));

		// double value that overflows a long long
		CHECK(cxcm::trunc(0x1.0p+63) == std::trunc(0x1.0p+63));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::trunc(0.5) == std::trunc(0.5));
		CHECK(cxcm::trunc(-0.5) == std::trunc(-0.5));

		// other values near 0
		CHECK(cxcm::trunc(0.25) == std::trunc(0.25));
		CHECK(cxcm::trunc(-0.25) == std::trunc(-0.25));
		CHECK(cxcm::trunc(0.75) == std::trunc(0.75));
		CHECK(cxcm::trunc(-0.75) == std::trunc(-0.75));

		// simple values with no fractional part
		CHECK(cxcm::trunc(-4.0) == std::trunc(-4.0));
		CHECK(cxcm::trunc(125.0) == std::trunc(125.0));

		// simple values with fractional parts
		CHECK(cxcm::trunc(-1.825) == std::trunc(-1.825));
		CHECK(cxcm::trunc(13.75) == std::trunc(13.75));
	}

	TEST_CASE("testing cxcm::trunc() double values")
	{
		// +/- 0
		CHECK(cxcm::trunc(0.0) == 0.0);
		CHECK(cxcm::trunc(-0.0) == -0.0);

		// +/- infinity
		CHECK(cxcm::trunc(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		CHECK(cxcm::trunc(-std::numeric_limits<double>::infinity()) == -std::numeric_limits<double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::trunc(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::trunc(std::numeric_limits<double>::denorm_min()) == 0.0);
		CHECK(cxcm::trunc(-std::numeric_limits<double>::denorm_min()) == -0.0);
		CHECK(cxcm::trunc(std::numeric_limits<double>::epsilon()) == 0.0);
		CHECK(cxcm::trunc(-std::numeric_limits<double>::epsilon()) == -0.0);

		// +/- min and max normal values
		CHECK(cxcm::trunc(std::numeric_limits<double>::min()) == 0.0);
		CHECK(cxcm::trunc(-std::numeric_limits<double>::min()) == -0.0);
		CHECK(cxcm::trunc(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		CHECK(cxcm::trunc(std::numeric_limits<double>::lowest()) == std::numeric_limits<double>::lowest());

		// +/- largest fractional double
		CHECK(cxcm::trunc(0x1.fffffffffffffp+51) == 0x1.ffffffffffffep+51);
		CHECK(cxcm::trunc(-0x1.fffffffffffffp+51) == -0x1.ffffffffffffep+51);

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::trunc(0x1.0000000000001p+52) == 0x1.0000000000001p+52);
		CHECK(cxcm::trunc(-0x1.0000000000001p+52) == -0x1.0000000000001p+52);

		// double value that overflows a long long
		CHECK(cxcm::trunc(0x1.0p+63) == 0x1.0p+63);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::trunc(0.5) == 0.0);
		CHECK(cxcm::trunc(-0.5) == -0.0);

		// other values near 0
		CHECK(cxcm::trunc(0.25) == 0.0);
		CHECK(cxcm::trunc(-0.25) == -0.0);
		CHECK(cxcm::trunc(0.75) == 0.0);
		CHECK(cxcm::trunc(-0.75) == -0.0);

		// simple values with no fractional part
		CHECK(cxcm::trunc(-4.0) == -4.0);
		CHECK(cxcm::trunc(125.0) == 125.0);

		// simple values with fractional parts
		CHECK(cxcm::trunc(-1.825) == -1.0);
		CHECK(cxcm::trunc(13.75) == 13.0);
	}

	TEST_CASE("testing fidelity of cxcm::floor() with std::floor() with double values")
	{
		// +/- 0
		CHECK(cxcm::floor(0.0) == std::floor(0.0));
		CHECK(cxcm::floor(-0.0) == std::floor(-0.0));

		// +/- infinity
		CHECK(cxcm::floor(std::numeric_limits<double>::infinity()) == std::floor(std::numeric_limits<double>::infinity()));
		CHECK(cxcm::floor(-std::numeric_limits<double>::infinity()) == std::floor(-std::numeric_limits<double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::floor(std::numeric_limits<double>::quiet_NaN())) == std::isnan(std::floor(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::floor(std::numeric_limits<double>::denorm_min()) == std::floor(std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::floor(-std::numeric_limits<double>::denorm_min()) == std::floor(-std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::floor(std::numeric_limits<double>::epsilon()) == std::floor(std::numeric_limits<double>::epsilon()));
		CHECK(cxcm::floor(-std::numeric_limits<double>::epsilon()) == std::floor(-std::numeric_limits<double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::floor(std::numeric_limits<double>::min()) == std::floor(std::numeric_limits<double>::min()));
		CHECK(cxcm::floor(-std::numeric_limits<double>::min()) == std::floor(-std::numeric_limits<double>::min()));
		CHECK(cxcm::floor(std::numeric_limits<double>::max()) == std::floor(std::numeric_limits<double>::max()));
		CHECK(cxcm::floor(std::numeric_limits<double>::lowest()) == std::floor(std::numeric_limits<double>::lowest()));

		// +/- largest fractional double
		CHECK(cxcm::floor(0x1.fffffffffffffp+51) == std::floor(0x1.fffffffffffffp+51));
		CHECK(cxcm::floor(-0x1.fffffffffffffp+51) == std::floor(-0x1.fffffffffffffp+51));

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::floor(0x1.0000000000001p+52) == std::floor(0x1.0000000000001p+52));
		CHECK(cxcm::floor(-0x1.0000000000001p+52) == std::floor(-0x1.0000000000001p+52));

		// double value that overflows a long long
		CHECK(cxcm::floor(0x1.0p+63) == std::floor(0x1.0p+63));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::floor(0.5) == std::floor(0.5));
		CHECK(cxcm::floor(-0.5) == std::floor(-0.5));

		// other values near 0
		CHECK(cxcm::floor(0.25) == std::floor(0.25));
		CHECK(cxcm::floor(-0.25) == std::floor(-0.25));
		CHECK(cxcm::floor(0.75) == std::floor(0.75));
		CHECK(cxcm::floor(-0.75) == std::floor(-0.75));

		// simple values with no fractional part
		CHECK(cxcm::floor(-4.0) == std::floor(-4.0));
		CHECK(cxcm::floor(125.0) == std::floor(125.0));

		// simple values with fractional parts
		CHECK(cxcm::floor(-1.825) == std::floor(-1.825));
		CHECK(cxcm::floor(13.75) == std::floor(13.75));
	}

	TEST_CASE("testing cxcm::floor() double values")
	{
		// +/- 0
		CHECK(cxcm::floor(0.0) == 0.0);
		CHECK(cxcm::floor(-0.0) == -0.0);

		// +/- infinity
		CHECK(cxcm::floor(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		CHECK(cxcm::floor(-std::numeric_limits<double>::infinity()) == -std::numeric_limits<double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::floor(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::floor(std::numeric_limits<double>::denorm_min()) == 0.0);
		CHECK(cxcm::floor(-std::numeric_limits<double>::denorm_min()) == -1.0);
		CHECK(cxcm::floor(std::numeric_limits<double>::epsilon()) == 0.0);
		CHECK(cxcm::floor(-std::numeric_limits<double>::epsilon()) == -1.0);

		// +/- min and max normal values
		CHECK(cxcm::floor(std::numeric_limits<double>::min()) == 0.0);
		CHECK(cxcm::floor(-std::numeric_limits<double>::min()) == -1.0);
		CHECK(cxcm::floor(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		CHECK(cxcm::floor(std::numeric_limits<double>::lowest()) == std::numeric_limits<double>::lowest());

		// +/- largest fractional double
		CHECK(cxcm::floor(0x1.fffffffffffffp+51) == 0x1.ffffffffffffep+51);
		CHECK(cxcm::floor(-0x1.fffffffffffffp+51) == -0x1.0p+52);

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::floor(0x1.0000000000001p+52) == 0x1.0000000000001p+52);
		CHECK(cxcm::floor(-0x1.0000000000001p+52) == -0x1.0000000000001p+52);

		// double value that overflows a long long
		CHECK(cxcm::floor(0x1.0p+63) == 0x1.0p+63);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::floor(0.5) == 0.0);
		CHECK(cxcm::floor(-0.5) == -1.0);

		// other values near 0
		CHECK(cxcm::floor(0.25) == 0.0);
		CHECK(cxcm::floor(-0.25) == -1.0);
		CHECK(cxcm::floor(0.75) == 0.0);
		CHECK(cxcm::floor(-0.75) == -1.0);

		// simple values with no fractional part
		CHECK(cxcm::floor(-4.0) == -4.0);
		CHECK(cxcm::floor(125.0) == 125.0);

		// simple values with fractional parts
		CHECK(cxcm::floor(-1.825) == -2.0);
		CHECK(cxcm::floor(13.75) == 13.0);
	}

	TEST_CASE("testing fidelity of cxcm::ceil() with std::ceil() with double values")
	{
		// +/- 0
		CHECK(cxcm::ceil(0.0) == std::ceil(0.0));
		CHECK(cxcm::ceil(-0.0) == std::ceil(-0.0));

		// +/- infinity
		CHECK(cxcm::ceil(std::numeric_limits<double>::infinity()) == std::ceil(std::numeric_limits<double>::infinity()));
		CHECK(cxcm::ceil(-std::numeric_limits<double>::infinity()) == std::ceil(-std::numeric_limits<double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::ceil(std::numeric_limits<double>::quiet_NaN())) == std::isnan(std::ceil(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::ceil(std::numeric_limits<double>::denorm_min()) == std::ceil(std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::ceil(-std::numeric_limits<double>::denorm_min()) == std::ceil(-std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::ceil(std::numeric_limits<double>::epsilon()) == std::ceil(std::numeric_limits<double>::epsilon()));
		CHECK(cxcm::ceil(-std::numeric_limits<double>::epsilon()) == std::ceil(-std::numeric_limits<double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::ceil(std::numeric_limits<double>::min()) == std::ceil(std::numeric_limits<double>::min()));
		CHECK(cxcm::ceil(-std::numeric_limits<double>::min()) == std::ceil(-std::numeric_limits<double>::min()));
		CHECK(cxcm::ceil(std::numeric_limits<double>::max()) == std::ceil(std::numeric_limits<double>::max()));
		CHECK(cxcm::ceil(std::numeric_limits<double>::lowest()) == std::ceil(std::numeric_limits<double>::lowest()));

		// +/- largest fractional double
		CHECK(cxcm::ceil(0x1.fffffffffffffp+51) == std::ceil(0x1.fffffffffffffp+51));
		CHECK(cxcm::ceil(-0x1.fffffffffffffp+51) == std::ceil(-0x1.fffffffffffffp+51));

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::ceil(0x1.0000000000001p+52) == std::ceil(0x1.0000000000001p+52));
		CHECK(cxcm::ceil(-0x1.0000000000001p+52) == std::ceil(-0x1.0000000000001p+52));

		// double value that overflows a long long
		CHECK(cxcm::ceil(0x1.0p+63) == std::ceil(0x1.0p+63));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::ceil(0.5) == std::ceil(0.5));
		CHECK(cxcm::ceil(-0.5) == std::ceil(-0.5));

		// other values near 0
		CHECK(cxcm::ceil(0.25) == std::ceil(0.25));
		CHECK(cxcm::ceil(-0.25) == std::ceil(-0.25));
		CHECK(cxcm::ceil(0.75) == std::ceil(0.75));
		CHECK(cxcm::ceil(-0.75) == std::ceil(-0.75));

		// simple values with no fractional part
		CHECK(cxcm::ceil(-4.0) == std::ceil(-4.0));
		CHECK(cxcm::ceil(125.0) == std::ceil(125.0));

		// simple values with fractional parts
		CHECK(cxcm::ceil(-1.825) == std::ceil(-1.825));
		CHECK(cxcm::ceil(13.75) == std::ceil(13.75));
	}

	TEST_CASE("testing cxcm::ceil() double values")
	{
		// +/- 0
		CHECK(cxcm::ceil(0.0) == 0.0);
		CHECK(cxcm::ceil(-0.0) == -0.0);

		// +/- infinity
		CHECK(cxcm::ceil(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		CHECK(cxcm::ceil(-std::numeric_limits<double>::infinity()) == -std::numeric_limits<double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::ceil(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::ceil(std::numeric_limits<double>::denorm_min()) == 1.0);
		CHECK(cxcm::ceil(-std::numeric_limits<double>::denorm_min()) == -0.0);
		CHECK(cxcm::ceil(std::numeric_limits<double>::epsilon()) == 1.0);
		CHECK(cxcm::ceil(-std::numeric_limits<double>::epsilon()) == -0.0);

		// +/- min and max normal values
		CHECK(cxcm::ceil(std::numeric_limits<double>::min()) == 1.0);
		CHECK(cxcm::ceil(-std::numeric_limits<double>::min()) == -0.0);
		CHECK(cxcm::ceil(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		CHECK(cxcm::ceil(std::numeric_limits<double>::lowest()) == std::numeric_limits<double>::lowest());

		// +/- largest fractional double
		CHECK(cxcm::ceil(0x1.fffffffffffffp+51) == 0x1.0p+52);
		CHECK(cxcm::ceil(-0x1.fffffffffffffp+51) == -0x1.ffffffffffffep+51);

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::ceil(0x1.0000000000001p+52) == 0x1.0000000000001p+52);
		CHECK(cxcm::ceil(-0x1.0000000000001p+52) == -0x1.0000000000001p+52);

		// double value that overflows a long long
		CHECK(cxcm::ceil(0x1.0p+63) == 0x1.0p+63);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::ceil(0.5) == 1.0);
		CHECK(cxcm::ceil(-0.5) == -0.0);

		// other values near 0
		CHECK(cxcm::ceil(0.25) == 1.0);
		CHECK(cxcm::ceil(-0.25) == -0.0);
		CHECK(cxcm::ceil(0.75) == 1.0);
		CHECK(cxcm::ceil(-0.75) == -0.0);

		// simple values with no fractional part
		CHECK(cxcm::ceil(-4.0) == -4.0);
		CHECK(cxcm::ceil(125.0) == 125.0);

		// simple values with fractional parts
		CHECK(cxcm::ceil(-1.825) == -1.0);
		CHECK(cxcm::ceil(13.75) == 14.0);
	}

	TEST_CASE("testing fidelity of cxcm::round() with std::round() with double values")
	{
		// +/- 0
		CHECK(cxcm::round(0.0) == std::round(0.0));
		CHECK(cxcm::round(-0.0) == std::round(-0.0));

		// +/- infinity
		CHECK(cxcm::round(std::numeric_limits<double>::infinity()) == std::round(std::numeric_limits<double>::infinity()));
		CHECK(cxcm::round(-std::numeric_limits<double>::infinity()) == std::round(-std::numeric_limits<double>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::round(std::numeric_limits<double>::quiet_NaN())) == std::isnan(std::round(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round(std::numeric_limits<double>::denorm_min()) == std::round(std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::round(-std::numeric_limits<double>::denorm_min()) == std::round(-std::numeric_limits<double>::denorm_min()));
		CHECK(cxcm::round(std::numeric_limits<double>::epsilon()) == std::round(std::numeric_limits<double>::epsilon()));
		CHECK(cxcm::round(-std::numeric_limits<double>::epsilon()) == std::round(-std::numeric_limits<double>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::round(std::numeric_limits<double>::min()) == std::round(std::numeric_limits<double>::min()));
		CHECK(cxcm::round(-std::numeric_limits<double>::min()) == std::round(-std::numeric_limits<double>::min()));
		CHECK(cxcm::round(std::numeric_limits<double>::max()) == std::round(std::numeric_limits<double>::max()));
		CHECK(cxcm::round(std::numeric_limits<double>::lowest()) == std::round(std::numeric_limits<double>::lowest()));

		// +/- largest fractional double
		CHECK(cxcm::round(0x1.fffffffffffffp+51) == std::round(0x1.fffffffffffffp+51));
		CHECK(cxcm::round(-0x1.fffffffffffffp+51) == std::round(-0x1.fffffffffffffp+51));

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::round(0x1.0000000000001p+52) == std::round(0x1.0000000000001p+52));
		CHECK(cxcm::round(-0x1.0000000000001p+52) == std::round(-0x1.0000000000001p+52));

		// double value that overflows a long long
		CHECK(cxcm::round(0x1.0p+63) == std::round(0x1.0p+63));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::round(0.5) == std::round(0.5));
		CHECK(cxcm::round(-0.5) == std::round(-0.5));

		// other values near 0
		CHECK(cxcm::round(0.25) == std::round(0.25));
		CHECK(cxcm::round(-0.25) == std::round(-0.25));
		CHECK(cxcm::round(0.75) == std::round(0.75));
		CHECK(cxcm::round(-0.75) == std::round(-0.75));

		// simple values with no fractional part
		CHECK(cxcm::round(-4.0) == std::round(-4.0));
		CHECK(cxcm::round(125.0) == std::round(125.0));

		// simple values with fractional parts
		CHECK(cxcm::round(-1.825) == std::round(-1.825));
		CHECK(cxcm::round(13.75) == std::round(13.75));
	}

	TEST_CASE("testing cxcm::round() double values")
	{
		// +/- 0
		CHECK(cxcm::round(0.0) == 0.0);
		CHECK(cxcm::round(-0.0) == -0.0);

		// +/- infinity
		CHECK(cxcm::round(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		CHECK(cxcm::round(-std::numeric_limits<double>::infinity()) == -std::numeric_limits<double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::round(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round(std::numeric_limits<double>::denorm_min()) == 0.0);
		CHECK(cxcm::round(-std::numeric_limits<double>::denorm_min()) == -0.0);
		CHECK(cxcm::round(std::numeric_limits<double>::epsilon()) == 0.0);
		CHECK(cxcm::round(-std::numeric_limits<double>::epsilon()) == -0.0);

		// +/- min and max normal values
		CHECK(cxcm::round(std::numeric_limits<double>::min()) == 0.0);
		CHECK(cxcm::round(-std::numeric_limits<double>::min()) == -0.0);
		CHECK(cxcm::round(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		CHECK(cxcm::round(std::numeric_limits<double>::lowest()) == std::numeric_limits<double>::lowest());

		// +/- largest fractional double
		CHECK(cxcm::round(0x1.fffffffffffffp+51) == 0x1.0p+52);
		CHECK(cxcm::round(-0x1.fffffffffffffp+51) == -0x1.0p+52);

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::round(0x1.0000000000001p+52) == 0x1.0000000000001p+52);
		CHECK(cxcm::round(-0x1.0000000000001p+52) == -0x1.0000000000001p+52);

		// double value that overflows a long long
		CHECK(cxcm::round(0x1.0p+63) == 0x1.0p+63);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::round(0.5) == 1.0);
		CHECK(cxcm::round(-0.5) == -1.0);

		// other values near 0
		CHECK(cxcm::round(0.25) == 0.0);
		CHECK(cxcm::round(-0.25) == -0.0);
		CHECK(cxcm::round(0.75) == 1.0);
		CHECK(cxcm::round(-0.75) == -1.0);

		// simple values with no fractional part
		CHECK(cxcm::round(-4.0) == -4.0);
		CHECK(cxcm::round(125.0) == 125.0);

		// simple values with fractional parts
		CHECK(cxcm::round(-1.825) == -2.0);
		CHECK(cxcm::round(13.75) == 14.0);
	}

	TEST_CASE("testing cxcm::round_even() double values")
	{
		// +/- 0
		CHECK(cxcm::round_even(0.0) == 0.0);
		CHECK(cxcm::round_even(-0.0) == -0.0);

		// +/- infinity
		CHECK(cxcm::round_even(std::numeric_limits<double>::infinity()) == std::numeric_limits<double>::infinity());
		CHECK(cxcm::round_even(-std::numeric_limits<double>::infinity()) == -std::numeric_limits<double>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::round_even(std::numeric_limits<double>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round_even(std::numeric_limits<double>::denorm_min()) == 0.0);
		CHECK(cxcm::round_even(-std::numeric_limits<double>::denorm_min()) == -0.0);
		CHECK(cxcm::round_even(std::numeric_limits<double>::epsilon()) == 0.0);
		CHECK(cxcm::round_even(-std::numeric_limits<double>::epsilon()) == -0.0);

		// +/- min and max normal values
		CHECK(cxcm::round_even(std::numeric_limits<double>::min()) == 0.0);
		CHECK(cxcm::round_even(-std::numeric_limits<double>::min()) == -0.0);
		CHECK(cxcm::round_even(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		CHECK(cxcm::round_even(std::numeric_limits<double>::lowest()) == std::numeric_limits<double>::lowest());

		// +/- largest fractional double
		CHECK(cxcm::round_even(0x1.fffffffffffffp+51) == 0x1.0p+52);
		CHECK(cxcm::round_even(-0x1.fffffffffffffp+51) == -0x1.0p+52);

		// +/- value where distance between adjacent doubles is 1.0
		CHECK(cxcm::round_even(0x1.0000000000001p+52) == 0x1.0000000000001p+52);
		CHECK(cxcm::round_even(-0x1.0000000000001p+52) == -0x1.0000000000001p+52);

		// double value that overflows a long long
		CHECK(cxcm::round_even(0x1.0p+63) == 0x1.0p+63);

		// other values near 0
		CHECK(cxcm::round_even(0.25) == 0.0);
		CHECK(cxcm::round_even(-0.25) == -0.0);
		CHECK(cxcm::round_even(0.75) == 1.0);
		CHECK(cxcm::round_even(-0.75) == -1.0);

		// simple values with no fractional part
		CHECK(cxcm::round_even(-4.0) == -4.0);
		CHECK(cxcm::round_even(125.0) == 125.0);

		// simple values with fractional parts
		CHECK(cxcm::round_even(-1.825) == -2.0);
		CHECK(cxcm::round_even(13.75) == 14.0);


		// the round even specialty of this function
		CHECK(cxcm::round_even(-3.5) == -4);
		CHECK(cxcm::round_even(-2.5) == -2);
		CHECK(cxcm::round_even(-1.5) == -2);
		CHECK(cxcm::round_even(-0.5) == -0);
		CHECK(cxcm::round_even(0.5) == 0);
		CHECK(cxcm::round_even(1.5) == 2);
		CHECK(cxcm::round_even(2.5) == 2);
		CHECK(cxcm::round_even(3.5) == 4);
	}

	TEST_CASE("constexpr sqrt() and rsqrt() for double")
	{
		CHECK_EQ(std::numbers::sqrt2_v<double>, cxcm::sqrt(2.0));
		CHECK_EQ(std::numbers::sqrt3_v<double>, cxcm::sqrt(3.0));
		CHECK_EQ(std::numbers::phi_v<double>, (1.0 + cxcm::sqrt(5.0)) / 2.0);

		CHECK_EQ(std::numbers::inv_sqrt3_v<double>, cxcm::rsqrt(3.0));
		CHECK_EQ(std::numbers::inv_sqrtpi_v<double>, cxcm::rsqrt(std::numbers::pi_v<double>));
	}
}

TEST_SUITE("constexpr_math for float")
{

	TEST_CASE("testing fidelity of cxcm::signbit() with std::signbit() for float values")
	{
		auto pos_nan = std::numeric_limits<float>::quiet_NaN();
		auto neg_nan = -std::numeric_limits<float>::quiet_NaN();
		auto pos_inf = std::numeric_limits<float>::infinity();
		auto neg_inf = -std::numeric_limits<float>::infinity();
		float pos_zero = 0;
		float neg_zero = std::copysign(0.0f, -1.0f);
		float pos_four = 4;
		float neg_four = -4;

		// +/- 0
		CHECK(cxcm::signbit(pos_zero) == std::signbit(pos_zero));
		CHECK(cxcm::signbit(neg_zero) == std::signbit(neg_zero));

		// +/- infinity
		CHECK(cxcm::signbit(pos_inf) == std::signbit(pos_inf));
		CHECK(cxcm::signbit(neg_inf) == std::signbit(neg_inf));

		// NaN
		CHECK(cxcm::signbit(pos_nan) == std::signbit(pos_nan));
		CHECK(cxcm::signbit(neg_nan) == std::signbit(neg_nan));

		// +/- 4
		CHECK(cxcm::signbit(pos_four) == std::signbit(pos_four));
		CHECK(cxcm::signbit(neg_four) == std::signbit(neg_four));
	}

	TEST_CASE("testing fidelity of cxcm::copysign() with std::copysign() for float values")
	{
		auto pos_nan = std::numeric_limits<float>::quiet_NaN();
		auto neg_nan = -std::numeric_limits<float>::quiet_NaN();
		auto pos_inf = std::numeric_limits<float>::infinity();
		auto neg_inf = -std::numeric_limits<float>::infinity();
		float pos_zero = 0;
		float neg_zero = std::copysign(0.0f, -1.0f);
		float pos_four = 4;
		float neg_four = -4;

		// +/- 0
		CHECK(cxcm::copysign(pos_four, pos_zero) == std::copysign(pos_four, pos_zero));
		CHECK(cxcm::copysign(neg_four, pos_zero) == std::copysign(neg_four, pos_zero));
		CHECK(cxcm::copysign(pos_four, neg_zero) == std::copysign(pos_four, neg_zero));
		CHECK(cxcm::copysign(neg_four, neg_zero) == std::copysign(neg_four, neg_zero));

		// +/- infinity
		CHECK(cxcm::copysign(pos_four, pos_inf) == std::copysign(pos_four, pos_inf));
		CHECK(cxcm::copysign(neg_four, pos_inf) == std::copysign(neg_four, pos_inf));
		CHECK(cxcm::copysign(pos_four, neg_inf) == std::copysign(pos_four, neg_inf));
		CHECK(cxcm::copysign(neg_four, neg_inf) == std::copysign(neg_four, neg_inf));

		CHECK(cxcm::copysign(pos_inf, pos_four) == std::copysign(pos_inf, pos_four));
		CHECK(cxcm::copysign(pos_inf, neg_four) == std::copysign(pos_inf, neg_four));
		CHECK(cxcm::copysign(neg_inf, pos_four) == std::copysign(neg_inf, pos_four));
		CHECK(cxcm::copysign(neg_inf, neg_four) == std::copysign(neg_inf, neg_four));

		// NaN
		CHECK(cxcm::copysign(pos_four, pos_nan) == std::copysign(pos_four, pos_nan));
		CHECK(cxcm::copysign(neg_four, pos_nan) == std::copysign(neg_four, pos_nan));
		CHECK(cxcm::copysign(pos_four, neg_nan) == std::copysign(pos_four, neg_nan));
		CHECK(cxcm::copysign(neg_four, neg_nan) == std::copysign(neg_four, neg_nan));

		// +/- 4
		CHECK(cxcm::copysign(pos_four, pos_four) == std::copysign(pos_four, pos_four));
		CHECK(cxcm::copysign(neg_four, pos_four) == std::copysign(neg_four, pos_four));
		CHECK(cxcm::copysign(pos_four, neg_four) == std::copysign(pos_four, neg_four));
		CHECK(cxcm::copysign(neg_four, neg_four) == std::copysign(neg_four, neg_four));
	}

	TEST_CASE("testing fidelity of cxcm::trunc() with std::trunc() with float values")
	{
		// +/- 0
		CHECK(cxcm::trunc(0.0f) == std::trunc(0.0f));
		CHECK(cxcm::trunc(-0.0f) == std::trunc(-0.0f));

		// +/- infinity
		CHECK(cxcm::trunc(std::numeric_limits<float>::infinity()) == std::trunc(std::numeric_limits<float>::infinity()));
		CHECK(cxcm::trunc(-std::numeric_limits<float>::infinity()) == std::trunc(-std::numeric_limits<float>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::trunc(std::numeric_limits<float>::quiet_NaN())) == std::isnan(std::trunc(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::trunc(std::numeric_limits<float>::denorm_min()) == std::trunc(std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::trunc(-std::numeric_limits<float>::denorm_min()) == std::trunc(-std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::trunc(std::numeric_limits<float>::epsilon()) == std::trunc(std::numeric_limits<float>::epsilon()));
		CHECK(cxcm::trunc(-std::numeric_limits<float>::epsilon()) == std::trunc(-std::numeric_limits<float>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::trunc(std::numeric_limits<float>::min()) == std::trunc(std::numeric_limits<float>::min()));
		CHECK(cxcm::trunc(-std::numeric_limits<float>::min()) == std::trunc(-std::numeric_limits<float>::min()));
		CHECK(cxcm::trunc(std::numeric_limits<float>::max()) == std::trunc(std::numeric_limits<float>::max()));
		CHECK(cxcm::trunc(std::numeric_limits<float>::lowest()) == std::trunc(std::numeric_limits<float>::lowest()));

		// +/- largest fractional float
		CHECK(cxcm::trunc(0x1.fffffep+22f) == std::trunc(0x1.fffffep+22f));
		CHECK(cxcm::trunc(-0x1.fffffep+22f) == std::trunc(-0x1.fffffep+22f));

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::trunc(0x1.000002p+23f) == std::trunc(0x1.000002p+23f));
		CHECK(cxcm::trunc(-0x1.000002p+23f) == std::trunc(-0x1.000002p+23f));

		// float value that overflows an int
		CHECK(cxcm::trunc(0x1.0p+31f) == std::trunc(0x1.0p+31f));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::trunc(0.5f) == std::trunc(0.5f));
		CHECK(cxcm::trunc(-0.5f) == std::trunc(-0.5f));

		// other values near 0
		CHECK(cxcm::trunc(0.25f) == std::trunc(0.25f));
		CHECK(cxcm::trunc(-0.25f) == std::trunc(-0.25f));
		CHECK(cxcm::trunc(0.75f) == std::trunc(0.75f));
		CHECK(cxcm::trunc(-0.75f) == std::trunc(-0.75f));

		// simple values with no fractional part
		CHECK(cxcm::trunc(-4.0f) == std::trunc(-4.0f));
		CHECK(cxcm::trunc(125.0f) == std::trunc(125.0f));

		// simple values with fractional parts
		CHECK(cxcm::trunc(-1.825f) == std::trunc(-1.825f));
		CHECK(cxcm::trunc(13.75f) == std::trunc(13.75f));
	}

	TEST_CASE("testing cxcm::trunc() float values")
	{
		// +/- 0
		CHECK(cxcm::trunc(0.0f) == 0.0f);
		CHECK(cxcm::trunc(-0.0f) == -0.0f);

		// +/- infinity
		CHECK(cxcm::trunc(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
		CHECK(cxcm::trunc(-std::numeric_limits<float>::infinity()) == -std::numeric_limits<float>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::trunc(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::trunc(std::numeric_limits<float>::denorm_min()) == 0.0f);
		CHECK(cxcm::trunc(-std::numeric_limits<float>::denorm_min()) == -0.0f);
		CHECK(cxcm::trunc(std::numeric_limits<float>::epsilon()) == 0.0f);
		CHECK(cxcm::trunc(-std::numeric_limits<float>::epsilon()) == -0.0f);

		// +/- min and max normal values
		CHECK(cxcm::trunc(std::numeric_limits<float>::min()) == 0.0f);
		CHECK(cxcm::trunc(-std::numeric_limits<float>::min()) == -0.0f);
		CHECK(cxcm::trunc(std::numeric_limits<float>::max()) == std::numeric_limits<float>::max());
		CHECK(cxcm::trunc(std::numeric_limits<float>::lowest()) == std::numeric_limits<float>::lowest());

		// +/- largest fractional float
		CHECK(cxcm::trunc(0x1.fffffep+22f) == 0x1.fffffcp+22f);
		CHECK(cxcm::trunc(-0x1.fffffep+22f) == -0x1.fffffcp+22f);

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::trunc(0x1.000002p+23f) == 0x1.000002p+23f);
		CHECK(cxcm::trunc(-0x1.000002p+23f) == -0x1.000002p+23f);

		// float value that overflows an int
		CHECK(cxcm::trunc(0x1.0p+31f) == 0x1.0p+31f);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::trunc(0.5f) == 0.0f);
		CHECK(cxcm::trunc(-0.5f) == -0.0f);

		// other values near 0
		CHECK(cxcm::trunc(0.25f) == 0.0f);
		CHECK(cxcm::trunc(-0.25f) == -0.0f);
		CHECK(cxcm::trunc(0.75f) == 0.0f);
		CHECK(cxcm::trunc(-0.75f) == -0.0f);

		// simple values with no fractional part
		CHECK(cxcm::trunc(-4.0f) == -4.0f);
		CHECK(cxcm::trunc(125.0f) == 125.0f);

		// simple values with fractional parts
		CHECK(cxcm::trunc(-1.825f) == -1.0f);
		CHECK(cxcm::trunc(13.75f) == 13.0f);
	}

	TEST_CASE("testing fidelity of cxcm::floor() with std::floor() with float values")
	{
		// +/- 0
		CHECK(cxcm::floor(0.0f) == std::floor(0.0f));
		CHECK(cxcm::floor(-0.0f) == std::floor(-0.0f));

		// +/- infinity
		CHECK(cxcm::floor(std::numeric_limits<float>::infinity()) == std::floor(std::numeric_limits<float>::infinity()));
		CHECK(cxcm::floor(-std::numeric_limits<float>::infinity()) == std::floor(-std::numeric_limits<float>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::floor(std::numeric_limits<float>::quiet_NaN())) == std::isnan(std::floor(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::floor(std::numeric_limits<float>::denorm_min()) == std::floor(std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::floor(-std::numeric_limits<float>::denorm_min()) == std::floor(-std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::floor(std::numeric_limits<float>::epsilon()) == std::floor(std::numeric_limits<float>::epsilon()));
		CHECK(cxcm::floor(-std::numeric_limits<float>::epsilon()) == std::floor(-std::numeric_limits<float>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::floor(std::numeric_limits<float>::min()) == std::floor(std::numeric_limits<float>::min()));
		CHECK(cxcm::floor(-std::numeric_limits<float>::min()) == std::floor(-std::numeric_limits<float>::min()));
		CHECK(cxcm::floor(std::numeric_limits<float>::max()) == std::floor(std::numeric_limits<float>::max()));
		CHECK(cxcm::floor(std::numeric_limits<float>::lowest()) == std::floor(std::numeric_limits<float>::lowest()));

		// +/- largest fractional float
		CHECK(cxcm::floor(0x1.fffffep+22f) == std::floor(0x1.fffffep+22f));
		CHECK(cxcm::floor(-0x1.fffffep+22f) == std::floor(-0x1.fffffep+22f));

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::floor(0x1.000002p+23f) == std::floor(0x1.000002p+23f));
		CHECK(cxcm::floor(-0x1.000002p+23f) == std::floor(-0x1.000002p+23f));

		// float value that overflows an int
		CHECK(cxcm::floor(0x1.0p+31f) == std::floor(0x1.0p+31f));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::floor(0.5f) == std::floor(0.5f));
		CHECK(cxcm::floor(-0.5f) == std::floor(-0.5f));

		// other values near 0
		CHECK(cxcm::floor(0.25f) == std::floor(0.25f));
		CHECK(cxcm::floor(-0.25f) == std::floor(-0.25f));
		CHECK(cxcm::floor(0.75f) == std::floor(0.75f));
		CHECK(cxcm::floor(-0.75f) == std::floor(-0.75f));

		// simple values with no fractional part
		CHECK(cxcm::floor(-4.0f) == std::floor(-4.0f));
		CHECK(cxcm::floor(125.0f) == std::floor(125.0f));

		// simple values with fractional parts
		CHECK(cxcm::floor(-1.825f) == std::floor(-1.825f));
		CHECK(cxcm::floor(13.75f) == std::floor(13.75f));
	}

	TEST_CASE("testing cxcm::floor() float values")
	{
		// +/- 0
		CHECK(cxcm::floor(0.0f) == 0.0f);
		CHECK(cxcm::floor(-0.0f) == -0.0f);

		// +/- infinity
		CHECK(cxcm::floor(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
		CHECK(cxcm::floor(-std::numeric_limits<float>::infinity()) == -std::numeric_limits<float>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::floor(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::floor(std::numeric_limits<float>::denorm_min()) == 0.0f);
		CHECK(cxcm::floor(-std::numeric_limits<float>::denorm_min()) == -1.0f);
		CHECK(cxcm::floor(std::numeric_limits<float>::epsilon()) == 0.0f);
		CHECK(cxcm::floor(-std::numeric_limits<float>::epsilon()) == -1.0f);

		// +/- min and max normal values
		CHECK(cxcm::floor(std::numeric_limits<float>::min()) == 0.0f);
		CHECK(cxcm::floor(-std::numeric_limits<float>::min()) == -1.0f);
		CHECK(cxcm::floor(std::numeric_limits<float>::max()) == std::numeric_limits<float>::max());
		CHECK(cxcm::floor(std::numeric_limits<float>::lowest()) == std::numeric_limits<float>::lowest());

		// +/- largest fractional float
		CHECK(cxcm::floor(0x1.fffffep+22f) == 0x1.fffffcp+22f);
		CHECK(cxcm::floor(-0x1.fffffep+22f) == -0x1.0p+23f);

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::floor(0x1.000002p+23f) == 0x1.000002p+23f);
		CHECK(cxcm::floor(-0x1.000002p+23f) == -0x1.000002p+23f);

		// float value that overflows an int
		CHECK(cxcm::floor(0x1.0p+31f) == 0x1.0p+31f);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::floor(0.5f) == 0.0f);
		CHECK(cxcm::floor(-0.5f) == -1.0f);

		// other values near 0
		CHECK(cxcm::floor(0.25f) == 0.0f);
		CHECK(cxcm::floor(-0.25f) == -1.0f);
		CHECK(cxcm::floor(0.75f) == 0.0f);
		CHECK(cxcm::floor(-0.75f) == -1.0f);

		// simple values with no fractional part
		CHECK(cxcm::floor(-4.0f) == -4.0f);
		CHECK(cxcm::floor(125.0f) == 125.0f);

		// simple values with fractional parts
		CHECK(cxcm::floor(-1.825f) == -2.0f);
		CHECK(cxcm::floor(13.75f) == 13.0f);
	}

	TEST_CASE("testing fidelity of cxcm::ceil() with std::ceil() with float values")
	{
		// +/- 0
		CHECK(cxcm::ceil(0.0f) == std::ceil(0.0f));
		CHECK(cxcm::ceil(-0.0f) == std::ceil(-0.0f));

		// +/- infinity
		CHECK(cxcm::ceil(std::numeric_limits<float>::infinity()) == std::ceil(std::numeric_limits<float>::infinity()));
		CHECK(cxcm::ceil(-std::numeric_limits<float>::infinity()) == std::ceil(-std::numeric_limits<float>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::ceil(std::numeric_limits<float>::quiet_NaN())) == std::isnan(std::ceil(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::ceil(std::numeric_limits<float>::denorm_min()) == std::ceil(std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::ceil(-std::numeric_limits<float>::denorm_min()) == std::ceil(-std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::ceil(std::numeric_limits<float>::epsilon()) == std::ceil(std::numeric_limits<float>::epsilon()));
		CHECK(cxcm::ceil(-std::numeric_limits<float>::epsilon()) == std::ceil(-std::numeric_limits<float>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::ceil(std::numeric_limits<float>::min()) == std::ceil(std::numeric_limits<float>::min()));
		CHECK(cxcm::ceil(-std::numeric_limits<float>::min()) == std::ceil(-std::numeric_limits<float>::min()));
		CHECK(cxcm::ceil(std::numeric_limits<float>::max()) == std::ceil(std::numeric_limits<float>::max()));
		CHECK(cxcm::ceil(std::numeric_limits<float>::lowest()) == std::ceil(std::numeric_limits<float>::lowest()));

		// +/- largest fractional float
		CHECK(cxcm::ceil(0x1.fffffep+22f) == std::ceil(0x1.fffffep+22f));
		CHECK(cxcm::ceil(-0x1.fffffep+22f) == std::ceil(-0x1.fffffep+22f));

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::ceil(0x1.000002p+23f) == std::ceil(0x1.000002p+23f));
		CHECK(cxcm::ceil(-0x1.000002p+23f) == std::ceil(-0x1.000002p+23f));

		// float value that overflows an int
		CHECK(cxcm::ceil(0x1.0p+31f) == std::ceil(0x1.0p+31f));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::ceil(0.5f) == std::ceil(0.5f));
		CHECK(cxcm::ceil(-0.5f) == std::ceil(-0.5f));

		// other values near 0
		CHECK(cxcm::ceil(0.25f) == std::ceil(0.25f));
		CHECK(cxcm::ceil(-0.25f) == std::ceil(-0.25f));
		CHECK(cxcm::ceil(0.75f) == std::ceil(0.75f));
		CHECK(cxcm::ceil(-0.75f) == std::ceil(-0.75f));

		// simple values with no fractional part
		CHECK(cxcm::ceil(-4.0f) == std::ceil(-4.0f));
		CHECK(cxcm::ceil(125.0f) == std::ceil(125.0f));

		// simple values with fractional parts
		CHECK(cxcm::ceil(-1.825f) == std::ceil(-1.825f));
		CHECK(cxcm::ceil(13.75f) == std::ceil(13.75f));
	}

	TEST_CASE("testing cxcm::ceil() float values")
	{
		// +/- 0
		CHECK(cxcm::ceil(0.0f) == 0.0f);
		CHECK(cxcm::ceil(-0.0f) == -0.0f);

		// +/- infinity
		CHECK(cxcm::ceil(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
		CHECK(cxcm::ceil(-std::numeric_limits<float>::infinity()) == -std::numeric_limits<float>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::ceil(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::ceil(std::numeric_limits<float>::denorm_min()) == 1.0f);
		CHECK(cxcm::ceil(-std::numeric_limits<float>::denorm_min()) == -0.0f);
		CHECK(cxcm::ceil(std::numeric_limits<float>::epsilon()) == 1.0f);
		CHECK(cxcm::ceil(-std::numeric_limits<float>::epsilon()) == -0.0f);

		// +/- min and max normal values
		CHECK(cxcm::ceil(std::numeric_limits<float>::min()) == 1.0f);
		CHECK(cxcm::ceil(-std::numeric_limits<float>::min()) == -0.0f);
		CHECK(cxcm::ceil(std::numeric_limits<float>::max()) == std::numeric_limits<float>::max());
		CHECK(cxcm::ceil(std::numeric_limits<float>::lowest()) == std::numeric_limits<float>::lowest());

		// +/- largest fractional float
		CHECK(cxcm::ceil(0x1.fffffep+22f) == 0x1.0p+23f);
		CHECK(cxcm::ceil(-0x1.fffffep+22f) == -0x1.fffffcp+22f);

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::ceil(0x1.000002p+23f) == 0x1.000002p+23f);
		CHECK(cxcm::ceil(-0x1.000002p+23f) == -0x1.000002p+23f);

		// float value that overflows an int
		CHECK(cxcm::ceil(0x1.0p+31f) == 0x1.0p+31f);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::ceil(0.5f) == 1.0f);
		CHECK(cxcm::ceil(-0.5f) == -0.0f);

		// other values near 0
		CHECK(cxcm::ceil(0.25f) == 1.0f);
		CHECK(cxcm::ceil(-0.25f) == -0.0f);
		CHECK(cxcm::ceil(0.75f) == 1.0f);
		CHECK(cxcm::ceil(-0.75f) == -0.0f);

		// simple values with no fractional part
		CHECK(cxcm::ceil(-4.0f) == -4.0f);
		CHECK(cxcm::ceil(125.0f) == 125.0f);

		// simple values with fractional parts
		CHECK(cxcm::ceil(-1.825f) == -1.0f);
		CHECK(cxcm::ceil(13.75f) == 14.0f);
	}

	TEST_CASE("testing fidelity of cxcm::round() with std::round() with float values")
	{
		// +/- 0
		CHECK(cxcm::round(0.0f) == std::round(0.0f));
		CHECK(cxcm::round(-0.0f) == std::round(-0.0f));

		// +/- infinity
		CHECK(cxcm::round(std::numeric_limits<float>::infinity()) == std::round(std::numeric_limits<float>::infinity()));
		CHECK(cxcm::round(-std::numeric_limits<float>::infinity()) == std::round(-std::numeric_limits<float>::infinity()));

		// NaN
		CHECK(cxcm::isnan(cxcm::round(std::numeric_limits<float>::quiet_NaN())) == std::isnan(std::round(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round(std::numeric_limits<float>::denorm_min()) == std::round(std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::round(-std::numeric_limits<float>::denorm_min()) == std::round(-std::numeric_limits<float>::denorm_min()));
		CHECK(cxcm::round(std::numeric_limits<float>::epsilon()) == std::round(std::numeric_limits<float>::epsilon()));
		CHECK(cxcm::round(-std::numeric_limits<float>::epsilon()) == std::round(-std::numeric_limits<float>::epsilon()));

		// +/- min and max normal values
		CHECK(cxcm::round(std::numeric_limits<float>::min()) == std::round(std::numeric_limits<float>::min()));
		CHECK(cxcm::round(-std::numeric_limits<float>::min()) == std::round(-std::numeric_limits<float>::min()));
		CHECK(cxcm::round(std::numeric_limits<float>::max()) == std::round(std::numeric_limits<float>::max()));
		CHECK(cxcm::round(std::numeric_limits<float>::lowest()) == std::round(std::numeric_limits<float>::lowest()));

		// +/- largest fractional float
		CHECK(cxcm::round(0x1.fffffep+22f) == std::round(0x1.fffffep+22f));
		CHECK(cxcm::round(-0x1.fffffep+22f) == std::round(-0x1.fffffep+22f));

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::round(0x1.000002p+23f) == std::round(0x1.000002p+23f));
		CHECK(cxcm::round(-0x1.000002p+23f) == std::round(-0x1.000002p+23f));

		// float value that overflows an int
		CHECK(cxcm::round(0x1.0p+31f) == std::round(0x1.0p+31f));

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::round(0.5f) == std::round(0.5f));
		CHECK(cxcm::round(-0.5f) == std::round(-0.5f));

		// other values near 0
		CHECK(cxcm::round(0.25f) == std::round(0.25f));
		CHECK(cxcm::round(-0.25f) == std::round(-0.25f));
		CHECK(cxcm::round(0.75f) == std::round(0.75f));
		CHECK(cxcm::round(-0.75f) == std::round(-0.75f));

		// simple values with no fractional part
		CHECK(cxcm::round(-4.0f) == std::round(-4.0f));
		CHECK(cxcm::round(125.0f) == std::round(125.0f));

		// simple values with fractional parts
		CHECK(cxcm::round(-1.825f) == std::round(-1.825f));
		CHECK(cxcm::round(13.75f) == std::round(13.75f));
	}

	TEST_CASE("testing cxcm::round() float values")
	{
		// +/- 0
		CHECK(cxcm::round(0.0f) == 0.0f);
		CHECK(cxcm::round(-0.0f) == -0.0f);

		// +/- infinity
		CHECK(cxcm::round(std::numeric_limits<float>::infinity()) == std::numeric_limits<float>::infinity());
		CHECK(cxcm::round(-std::numeric_limits<float>::infinity()) == -std::numeric_limits<float>::infinity());

		// NaN
		CHECK(cxcm::isnan(cxcm::round(std::numeric_limits<float>::quiet_NaN())));

		// +/- machine epsilon, +/- denorm_min
		CHECK(cxcm::round(std::numeric_limits<float>::denorm_min()) == 0.0f);
		CHECK(cxcm::round(-std::numeric_limits<float>::denorm_min()) == -0.0f);
		CHECK(cxcm::round(std::numeric_limits<float>::epsilon()) == 0.0f);
		CHECK(cxcm::round(-std::numeric_limits<float>::epsilon()) == -0.0f);

		// +/- min and max normal values
		CHECK(cxcm::round(std::numeric_limits<float>::min()) == 0.0f);
		CHECK(cxcm::round(-std::numeric_limits<float>::min()) == -0.0f);
		CHECK(cxcm::round(std::numeric_limits<float>::max()) == std::numeric_limits<float>::max());
		CHECK(cxcm::round(std::numeric_limits<float>::lowest()) == std::numeric_limits<float>::lowest());

		// +/- largest fractional float
		CHECK(cxcm::round(0x1.fffffep+22f) == 0x1.0p+23f);
		CHECK(cxcm::round(-0x1.fffffep+22f) == -0x1.0p+23f);

		// +/- value where distance between adjacent floats is 1.0
		CHECK(cxcm::round(0x1.000002p+23f) == 0x1.000002p+23f);
		CHECK(cxcm::round(-0x1.000002p+23f) == -0x1.000002p+23f);

		// float value that overflows an int
		CHECK(cxcm::round(0x1.0p+31f) == 0x1.0p+31f);

		// values near 0 that are halfway between 0 and +/- 1
		CHECK(cxcm::round(0.5f) == 1.0f);
		CHECK(cxcm::round(-0.5f) == -1.0f);

		// other values near 0
		CHECK(cxcm::round(0.25f) == 0.0f);
		CHECK(cxcm::round(-0.25f) == -0.0f);
		CHECK(cxcm::round(0.75f) == 1.0f);
		CHECK(cxcm::round(-0.75f) == -1.0f);

		// simple values with no fractional part
		CHECK(cxcm::round(-4.0f) == -4.0f);
		CHECK(cxcm::round(125.0f) == 125.0f);

		// simple values with fractional parts
		CHECK(cxcm::round(-1.825f) == -2.0f);
		CHECK(cxcm::round(13.75f) == 14.0f);
	}

	TEST_CASE("constexpr sqrt() and rsqrt() for float")
	{
		CHECK_EQ(std::numbers::sqrt2_v<float>, cxcm::sqrt(2.0f));
		CHECK_EQ(std::numbers::sqrt3_v<float>, cxcm::sqrt(3.0f));
		CHECK_EQ(std::numbers::phi_v<float>, (1.0f + cxcm::sqrt(5.0f)) / 2.0f);

		CHECK_EQ(std::numbers::inv_sqrt3_v<float>, cxcm::rsqrt(3.0f));

		// off by 1 ulp
//		CHECK_EQ(std::numbers::inv_sqrtpi_v<float>, cxcm::rsqrt(std::numbers::pi_v<float>));
	}
}

TEST_SUITE("constexpr_math for integral")
{
	TEST_CASE("testing cxcm::abs() integral values")
	{
		CHECK(cxcm::abs(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::abs(1) == 1.0);
		CHECK(cxcm::abs(0) == 0.0);
		CHECK(cxcm::abs(-1) == 1.0);
		CHECK(cxcm::abs(-INT_MAX) == static_cast<double>(INT_MAX));
	}

	TEST_CASE("testing cxcm::fabs() integral values")
	{
		CHECK(cxcm::fabs(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::fabs(1) == 1.0);
		CHECK(cxcm::fabs(0) == 0.0);
		CHECK(cxcm::fabs(-1) == 1.0);
		CHECK(cxcm::fabs(-INT_MAX) == static_cast<double>(INT_MAX));
	}

	TEST_CASE("testing cxcm::floor() integral values")
	{
		CHECK(cxcm::floor(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::floor(1) == 1.0);
		CHECK(cxcm::floor(0) == 0.0);
		CHECK(cxcm::floor(-1) == -1.0);
		CHECK(cxcm::floor(-INT_MAX) == static_cast<double>(-INT_MAX));
	}

	TEST_CASE("testing cxcm::ceil() integral values")
	{
		CHECK(cxcm::ceil(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::ceil(1) == 1.0);
		CHECK(cxcm::ceil(0) == 0.0);
		CHECK(cxcm::ceil(-1) == -1.0);
		CHECK(cxcm::ceil(-INT_MAX) == static_cast<double>(-INT_MAX));
	}

	TEST_CASE("testing cxcm::trunc() integral values")
	{
		CHECK(cxcm::trunc(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::trunc(1) == 1.0);
		CHECK(cxcm::trunc(0) == 0.0);
		CHECK(cxcm::trunc(-1) == -1.0);
		CHECK(cxcm::trunc(-INT_MAX) == static_cast<double>(-INT_MAX));
	}

	TEST_CASE("testing cxcm::fract() integral values")
	{
		CHECK(cxcm::fract(INT_MAX) == 0.0);
		CHECK(cxcm::fract(1) == 0.0);
		CHECK(cxcm::fract(0) == 0.0);
		CHECK(cxcm::fract(-1) == 0.0);
		CHECK(cxcm::fract(-INT_MAX) == 0.0);
	}

	TEST_CASE("testing cxcm::round() integral values")
	{
		CHECK(cxcm::round(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::round(1) == 1.0);
		CHECK(cxcm::round(0) == 0.0);
		CHECK(cxcm::round(-1) == -1.0);
		CHECK(cxcm::round(-INT_MAX) == static_cast<double>(-INT_MAX));
	}

	TEST_CASE("testing cxcm::round_even() integral values")
	{
		CHECK(cxcm::round_even(INT_MAX) == static_cast<double>(INT_MAX));
		CHECK(cxcm::round_even(1) == 1.0);
		CHECK(cxcm::round_even(0) == 0.0);
		CHECK(cxcm::round_even(-1) == -1.0);
		CHECK(cxcm::round_even(-INT_MAX) == static_cast<double>(-INT_MAX));
	}
}
