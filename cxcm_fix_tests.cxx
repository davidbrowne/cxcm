// regression tests for the cxcm fixes.
//
//   g++ -std=c++20 -O2 cxcm_fix_tests.cpp -o cxcm_fix_tests && ./cxcm_fix_tests
//
// part 1 is compile-time only (static_assert), part 2 evaluates in a constant expression and compares against the
// standard library at runtime, part 3 fuzzes the constexpr implementations against <cmath> at runtime.

#include "cxcm.hxx"

#include <cfloat>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <random>
#include <utility>

// ------------------------------------------------------------------------------------------------
// part 1: compile-time checks
// ------------------------------------------------------------------------------------------------

// integer abs/fabs
static_assert(cxcm::abs(0u) == 0u);
static_assert(cxcm::abs(7u) == 7u);
static_assert(cxcm::abs(-5) == 5);
static_assert(cxcm::abs(0) == 0);
static_assert(cxcm::abs(false) == false);
static_assert(cxcm::fabs(0u) == 0.0);
static_assert(cxcm::fabs(INT_MIN) == 2147483648.0);
static_assert(cxcm::fabs(LLONG_MIN) == 9223372036854775808.0);
static_assert(cxcm::fabs(-3) == 3.0);

// round: the largest value below 0.5 rounds to zero, and the sign of zero is kept
static_assert(cxcm::round(0.49999999999999994) == 0.0 && !cxcm::signbit(cxcm::round(0.49999999999999994)));
static_assert(cxcm::round(-0.49999999999999994) == 0.0 && cxcm::signbit(cxcm::round(-0.49999999999999994)));
static_assert(cxcm::round(0.49999997f) == 0.0f);
static_assert(cxcm::round(0.5) == 1.0 && cxcm::round(-0.5) == -1.0);
static_assert(cxcm::round(2.5) == 3.0 && cxcm::round(-2.5) == -3.0);
static_assert(cxcm::round(4503599627370495.5) == 4503599627370496.0);
static_assert(cxcm::round(-4503599627370495.5) == -4503599627370496.0);
static_assert(cxcm::signbit(cxcm::round(-0.3)));

// round_even
static_assert(cxcm::round_even(0.49999999999999994) == 0.0);
static_assert(cxcm::round_even(0.5) == 0.0 && cxcm::round_even(1.5) == 2.0 && cxcm::round_even(2.5) == 2.0);
static_assert(cxcm::round_even(-1.5) == -2.0 && cxcm::round_even(-2.5) == -2.0 && cxcm::round_even(-3.5) == -4.0);
static_assert(cxcm::signbit(cxcm::round_even(-0.5)));
static_assert(cxcm::round_even(4503599627370495.5) == 4503599627370496.0);		// halfway, odd neighbor below
static_assert(cxcm::round_even(4503599627370494.5) == 4503599627370494.0);		// halfway, even neighbor below
static_assert(cxcm::round_even(-4503599627370495.5) == -4503599627370496.0);
static_assert(cxcm::round_even(8388607.5f) == 8388608.0f);

// trunc/ceil keep the sign of zero
static_assert(cxcm::signbit(cxcm::trunc(-0.3)));
static_assert(cxcm::signbit(cxcm::ceil(-0.3)));
static_assert(!cxcm::signbit(cxcm::trunc(0.3)));
static_assert(cxcm::ceil(0.3) == 1.0 && cxcm::floor(-0.3) == -1.0);

// fract
static_assert(cxcm::fract(2.75) == 0.75 && cxcm::fract(-2.25) == 0.75);
static_assert(cxcm::fract(1e300) == 0.0);
static_assert(cxcm::fract(-1e300) == 0.0);
static_assert(cxcm::fract(4503599627370496.0) == 0.0);
static_assert(cxcm::fract(16777216.0f) == 0.0f);
static_assert(cxcm::isnan(cxcm::fract(std::numeric_limits<double>::infinity())));
static_assert(cxcm::fract(0.0) == 0.0);

// fmod
static_assert(cxcm::fmod(7.5, 2.0) == 1.5 && cxcm::fmod(-7.5, 2.0) == -1.5 && cxcm::fmod(7.5, -2.0) == 1.5);
static_assert(cxcm::signbit(cxcm::fmod(-6.0, 3.0)) && !cxcm::signbit(cxcm::fmod(6.0, -3.0)));
static_assert(cxcm::fmod(DBL_MAX, 5e-324) == 0.0);
static_assert(cxcm::fmod(5e-324, 3.0) == 5e-324);
static_assert(cxcm::fmod(1e300, 1e300) == 0.0);
static_assert(cxcm::isnan(cxcm::fmod(1.0, 0.0)));
static_assert(cxcm::isnan(cxcm::fmod(std::numeric_limits<double>::infinity(), 2.0)));
static_assert(cxcm::fmod(3.0, std::numeric_limits<double>::infinity()) == 3.0);

// sqrt / rsqrt at the ends of the range
static_assert(cxcm::sqrt(4.0) == 2.0);
static_assert(cxcm::sqrt(0.99999999999999989) == 0.99999999999999989);			// the largest double below 1. 2^-109 from a midpoint
static_assert(cxcm::sqrt(0x1.fffffffffffffp+107) == 18014398509481982.0);		// (2^54-1)^2 - 1: just below a midpoint
static_assert(cxcm::sqrt(5e-324) > 2.2e-162 && cxcm::sqrt(5e-324) < 2.3e-162);
static_assert(cxcm::sqrt(1e-315) > 3.16e-158 && cxcm::sqrt(1e-315) < 3.17e-158);
static_assert(cxcm::sqrt(DBL_MAX) > 1.34e154 && cxcm::sqrt(DBL_MAX) < 1.35e154);
static_assert(cxcm::rsqrt(0x1.a64f7609d6340p+922) == 0x1.8ea23d020eb61p-462);		// close to a rounding midpoint of 1/sqrt(x)
static_assert(!cxcm::isnan(cxcm::rsqrt(DBL_MAX)) && !cxcm::isnan(cxcm::rsqrt(5e-324)));
static_assert(cxcm::rsqrt(4.0) == 0.5);

// ------------------------------------------------------------------------------------------------
// part 2 + 3: runtime comparison with the standard library
// ------------------------------------------------------------------------------------------------

static int failures = 0;

#define CHECK(cond, ...)                                                    \
	do                                                                      \
	{                                                                       \
		if (!(cond))                                                        \
		{                                                                   \
			if (++failures <= 25)                                           \
			{                                                               \
				std::printf("FAIL line %d: " #cond "\n    ", __LINE__);     \
				std::printf(__VA_ARGS__);                                   \
				std::printf("\n");                                          \
			}                                                               \
		}                                                                   \
	} while (0)

template <typename T>
static bool same_bits(T a, T b)
{
	if (std::isnan(a) && std::isnan(b))
		return true;
	return std::memcmp(&a, &b, sizeof(T)) == 0;
}

template <typename T>
static T from_bits(unsigned long long bits)
{
	if constexpr (sizeof(T) == 4)
	{
		unsigned int b = static_cast<unsigned int>(bits);
		T v;
		std::memcpy(&v, &b, 4);
		return v;
	}
	else
	{
		T v;
		std::memcpy(&v, &bits, 8);
		return v;
	}
}

// reference for 1/sqrt(x) that doesn't need an extended precision type. for float, double has plenty of extra
// room, so this is exact. for double, 1.0 / std::sqrt(x) is itself off by up to 1 ulp (it's two roundings), so
// on its own it can't confirm correct rounding: see test_rsqrt_near_midpoints() for that.
template <typename T>
static bool rsqrt_close(T got, T x)
{
	if constexpr (sizeof(T) == 4)
	{
		const T ref = static_cast<T>(1.0 / std::sqrt(static_cast<double>(x)));
		const T ulp = std::nextafter(ref, std::numeric_limits<T>::infinity()) - ref;
		return std::fabs(got - ref) <= ulp;
	}
	else
	{
		const T ref = T(1) / std::sqrt(x);
		const T ulp = std::nextafter(ref, std::numeric_limits<T>::infinity()) - ref;
		return std::fabs(got - ref) <= ulp * T(2);
	}
}

// evaluate in a real constant expression, then compare against the standard library at runtime
#define CONSTEVAL_CHECK_FMOD(T, X, Y)                                                                  \
	do                                                                                                 \
	{                                                                                                  \
		constexpr T c = cxcm::fmod(T(X), T(Y));                                                        \
		volatile T vx = T(X), vy = T(Y);                                                               \
		const T s = std::fmod(T(vx), T(vy));                                                           \
		CHECK(same_bits(c, s), "consteval fmod(%s, %s) = %.17g, std = %.17g", #X, #Y, (double)c, (double)s); \
	} while (0)

static void part2_consteval_vs_std()
{
	CONSTEVAL_CHECK_FMOD(double, 5.3, 0.1);
	CONSTEVAL_CHECK_FMOD(double, 1e15, 0.3);
	CONSTEVAL_CHECK_FMOD(double, 1e300, 3.0);
	CONSTEVAL_CHECK_FMOD(double, DBL_MAX, 3.0);
	CONSTEVAL_CHECK_FMOD(double, DBL_MAX, 1e-300);
	CONSTEVAL_CHECK_FMOD(double, DBL_MAX, 5e-324);
	CONSTEVAL_CHECK_FMOD(double, 1e-310, 3e-312);
	CONSTEVAL_CHECK_FMOD(double, 2.5e-308, 1.1e-309);
	CONSTEVAL_CHECK_FMOD(double, -6.0, 3.0);
	CONSTEVAL_CHECK_FMOD(double, 6.0, -3.0);
	CONSTEVAL_CHECK_FMOD(double, -7.25, 2.0);
	CONSTEVAL_CHECK_FMOD(float, 1e10f, 0.3f);
	CONSTEVAL_CHECK_FMOD(float, FLT_MAX, 3.0f);
	CONSTEVAL_CHECK_FMOD(float, FLT_MAX, 1e-38f);
	CONSTEVAL_CHECK_FMOD(float, 1e-40f, 3e-42f);
	CONSTEVAL_CHECK_FMOD(float, -5.5f, 2.0f);

	// sqrt must be correctly rounded everywhere, including the extremes
	{
		constexpr double a = cxcm::sqrt(5e-324), b = cxcm::sqrt(1e-315), c = cxcm::sqrt(1e-310), d = cxcm::sqrt(DBL_MIN);
		constexpr double e = cxcm::sqrt(DBL_MAX), f = cxcm::sqrt(2.0), g = cxcm::sqrt(1e301), h = cxcm::sqrt(1e300);
		volatile double v1 = 5e-324, v2 = 1e-315, v3 = 1e-310, v4 = DBL_MIN, v5 = DBL_MAX, v6 = 2.0, v7 = 1e301, v8 = 1e300;
		CHECK(a == std::sqrt(double(v1)), "sqrt(5e-324) %.17g", a);
		CHECK(b == std::sqrt(double(v2)), "sqrt(1e-315) %.17g", b);
		CHECK(c == std::sqrt(double(v3)), "sqrt(1e-310) %.17g", c);
		CHECK(d == std::sqrt(double(v4)), "sqrt(DBL_MIN) %.17g", d);
		CHECK(e == std::sqrt(double(v5)), "sqrt(DBL_MAX) %.17g", e);
		CHECK(f == std::sqrt(double(v6)), "sqrt(2) %.17g", f);
		CHECK(g == std::sqrt(double(v7)), "sqrt(1e301) %.17g", g);
		CHECK(h == std::sqrt(double(v8)), "sqrt(1e300) %.17g", h);
	}

	// for x = 2^k * (1 - 2^-53) the exact root is within ~2^-109 of a rounding midpoint, closer than double-double's
	// ~106 bits can resolve, so the final rounding needs the exact residual check. e.g. sqrt(0.9999999999999999)
	// used to give 1 instead of 0.99999999999999989. every one of these must match std::sqrt exactly, anywhere in the
	// range (including subnormal results and the top end, which used to give garbage/NaN).
	{
		int mismatches = 0;
		for (int k = -1073; k <= 1023; ++k)
		{
			volatile double v = std::nextafter(std::ldexp(1.0, k), 0.0);
			if (!same_bits(cxcm::impl::constexpr_sqrt(double(v)), std::sqrt(double(v))))
				++mismatches;
		}
		CHECK(mismatches == 0, "%d inputs of the form 2^k(1-2^-53) don't match std::sqrt", mismatches);
	}

	// rsqrt: close to the reference (see rsqrt_close)
	{
		constexpr double a = cxcm::rsqrt(5e-324), b = cxcm::rsqrt(1e-315), c = cxcm::rsqrt(DBL_MAX), d = cxcm::rsqrt(1e301);
		volatile double v1 = 5e-324, v2 = 1e-315, v3 = DBL_MAX, v4 = 1e301;
		auto ok = [](double got, double in) { return rsqrt_close(got, in); };
		CHECK(ok(a, v1), "rsqrt(5e-324) %.17g", a);
		CHECK(ok(b, v2), "rsqrt(1e-315) %.17g", b);
		CHECK(ok(c, v3), "rsqrt(DBL_MAX) %.17g", c);
		CHECK(ok(d, v4), "rsqrt(1e301) %.17g", d);
	}

	// fast_rsqrt is an approximation, but a subnormal input must be as good as a normal one
	{
		constexpr double a = cxcm::fast_rsqrt(5e-324), b = cxcm::fast_rsqrt(1e-315), c = cxcm::fast_rsqrt(1e-300);
		volatile double v1 = 5e-324, v2 = 1e-315, v3 = 1e-300;
		auto rel = [](double got, double in) { return std::fabs(got * std::sqrt(in) - 1.0); };
		CHECK(rel(a, v1) < 1e-6, "fast_rsqrt(5e-324) %.17g rel %g", a, rel(a, v1));
		CHECK(rel(b, v2) < 1e-6, "fast_rsqrt(1e-315) %.17g rel %g", b, rel(b, v2));
		CHECK(rel(c, v3) < 1e-6, "fast_rsqrt(1e-300) %.17g rel %g", c, rel(c, v3));
	}
}

template <typename T>
static void fuzz_rounding(std::mt19937_64& rng, long count)
{
	using namespace cxcm::impl;

	auto test_value = [](T x) {
		volatile T v = x;
		T xv = v;
		CHECK(same_bits(constexpr_trunc(xv), std::trunc(xv)), "trunc(%.17g): %.17g vs %.17g", (double)xv, (double)constexpr_trunc(xv), (double)std::trunc(xv));
		CHECK(same_bits(constexpr_floor(xv), std::floor(xv)), "floor(%.17g)", (double)xv);
		CHECK(same_bits(constexpr_ceil(xv), std::ceil(xv)), "ceil(%.17g): %.17g vs %.17g", (double)xv, (double)constexpr_ceil(xv), (double)std::ceil(xv));
		CHECK(same_bits(constexpr_round(xv), std::round(xv)), "round(%.17g): %.17g vs %.17g", (double)xv, (double)constexpr_round(xv), (double)std::round(xv));
		CHECK(same_bits(constexpr_round_even(xv), std::nearbyint(xv)), "round_even(%.17g): %.17g vs %.17g", (double)xv, (double)constexpr_round_even(xv), (double)std::nearbyint(xv));

		// fract: x - floor(x) in the range where that's meaningful, 0 above it, NaN for infinity
		const T fr = constexpr_fract(xv);
		if (std::isnan(xv))
			CHECK(std::isnan(fr), "fract(nan)");
		else if (std::isinf(xv))
			CHECK(std::isnan(fr), "fract(inf)");
		else if (xv == T(0))
			CHECK(fr == T(0), "fract(0)");
		else if (std::fabs(xv) >= std::ldexp(T(1), std::numeric_limits<T>::digits - 1))
			CHECK(fr == T(0) && !std::signbit(fr), "fract(%.17g) = %.17g, expected 0", (double)xv, (double)fr);
		else
			CHECK(fr == xv - std::floor(xv), "fract(%.17g) = %.17g", (double)xv, (double)fr);
	};

	constexpr int digits = std::numeric_limits<T>::digits;
	const T big = std::ldexp(T(1), digits - 1);		// 2^52 or 2^23

	// special values and boundaries
	const T specials[] = {T(0), -T(0), T(0.5), -T(0.5), T(1.5), T(2.5), T(-2.5), T(0.3), T(-0.3), T(1e-30), T(-1e-30),
		std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), std::numeric_limits<T>::quiet_NaN(),
		std::numeric_limits<T>::denorm_min(), -std::numeric_limits<T>::denorm_min(), std::numeric_limits<T>::min(),
		std::numeric_limits<T>::max(), -std::numeric_limits<T>::max(), std::nextafter(T(0.5), T(0)), std::nextafter(T(-0.5), T(0)),
		std::nextafter(T(0.5), T(1)), std::nextafter(T(1.5), T(0)), std::nextafter(T(2.5), T(3))};
	for (T s : specials) test_value(s);

	for (T base : {big, T(big / 2), T(big * 2), T(1), T(1024)})
	{
		T x = base;
		for (int i = 0; i < 6; ++i) x = std::nextafter(x, T(0));
		for (int i = 0; i < 12; ++i)
		{
			test_value(x);
			test_value(-x);
			test_value(x + T(0.5));
			test_value(x - T(0.5));
			x = std::nextafter(x, std::numeric_limits<T>::infinity());
		}
	}

	std::uniform_real_distribution<T> unit(T(-1), T(1));
	for (long i = 0; i < count; ++i)
	{
		T x = unit(rng) * std::pow(T(10), T(int(rng() % 16) - 6));		// wide magnitude range
		test_value(x);
		test_value(std::round(x * 4) / 4);								// lots of exact halves and quarters
		test_value(std::nextafter(std::round(x) + T(0.5), T(0)));		// just below halfway
		test_value(std::nextafter(std::round(x) + T(0.5), T(1e30)));	// just above halfway
		test_value(from_bits<T>(rng()));								// any bit pattern
	}
}

template <typename T>
static void fuzz_fmod(std::mt19937_64& rng, long count)
{
	using namespace cxcm::impl;

	constexpr int exp_bits = (sizeof(T) == 4) ? 8 : 11;

	for (long i = 0; i < count; ++i)
	{
		T x = from_bits<T>(rng());
		T y = from_bits<T>(rng());

		// bias half the tests toward similar magnitudes (small quotients), where exactness matters most
		if (i % 2 == 0)
			y = std::copysign(std::ldexp(std::fabs(x), -int(rng() % 40)) * T(0.5 + (rng() % 1000) / 1000.0), y);

		volatile T vx = x, vy = y;
		const T c = constexpr_fmod(T(vx), T(vy));
		const T s = std::fmod(T(vx), T(vy));
		CHECK(same_bits(c, s), "fmod(%.17g, %.17g): %.17g vs %.17g", (double)x, (double)y, (double)c, (double)s);
	}

	(void)exp_bits;
}

template <typename T>
static void fuzz_sqrt(std::mt19937_64& rng, long count)
{
	using namespace cxcm::impl;

	for (long i = 0; i < count; ++i)
	{
		T x = std::fabs(from_bits<T>(rng()));		// every exponent, including subnormals
		if (std::isnan(x) || std::isinf(x)) continue;

		volatile T vx = x;
		const T c = constexpr_sqrt(T(vx));
		CHECK(same_bits(c, std::sqrt(T(vx))), "sqrt(%.17g): %.17g vs %.17g", (double)x, (double)c, (double)std::sqrt(T(vx)));

		// rsqrt close to the reference (see rsqrt_close)
		const T r = constexpr_rsqrt(T(vx));
		if (x != T(0))
			CHECK(rsqrt_close(r, x), "rsqrt(%.17g): %.17g vs %.17g", (double)x, (double)r, (double)(T(1) / std::sqrt(x)));

		// fast_rsqrt: an approximation, but never worse than ~1e-5 relative (much better for most inputs)
		if (x != T(0))
		{
			const T f = constexpr_fast_rsqrt(T(vx));
			const double rel = std::fabs(static_cast<double>(f) * std::sqrt(static_cast<double>(x)) - 1.0);
			CHECK(rel < 1e-5, "fast_rsqrt(%.17g): %.17g rel %g", (double)x, (double)f, rel);
		}
	}
}

// inputs whose square root is extremely close to a rounding midpoint, from both sides. a midpoint is a 54-bit odd integer
// M (between two adjacent doubles in [2^53, 2^54)), and x = M^2 - c is a double when M^2 == c (mod 2^55). the root is then
// only c * 2^-108 (relative) from the midpoint: 2^-109 for the closest, about 2^-100 for the farthest. found by Hensel
// lifting with exact integer math, for c = 1, 9, 17, ... (below the midpoint) and c = -7, -15, -23, ... (above it).
static const double near_midpoint_inputs[] = {
	0x1.fffffffffffffp+107, 0x1.ffffffffffffdp+107, 0x1.5b95344972fe2p+107, 0x1.ffffffffffffbp+107,
	0x1.4eb5f85e783a9p+107, 0x1.77483d37ce205p+107, 0x1.ffffffffffff9p+107, 0x1.2b035c1197f48p+106,
	0x1.256565cadcbf4p+106, 0x1.8444088dbdcb5p+107, 0x1.ba44c2a0737a2p+106, 0x1.e4054716ef6a0p+107,
	0x1.ffffffffffff7p+107, 0x1.92e58e3597929p+107, 0x1.69ca8fa07c526p+107, 0x1.e63a86a12a838p+107,
	0x1.1ac9955fd90d2p+107, 0x1.f974b2f9b9fd3p+107, 0x1.ffffffffffff5p+107, 0x1.270ac7cec9d2ap+106,
	0x1.283741a1bef08p+106, 0x1.732b9a374ddcep+106, 0x1.d2221598a924ap+107, 0x1.ffffffffffff3p+107,
	0x1.0d4cef742baf7p+107, 0x1.a903217634cb4p+107, 0x1.b9031abd6ad9cp+106, 0x1.3ac226a62efabp+107,
	0x1.ed8815e50aeb2p+106, 0x1.c4d4dd56d8601p+107, 0x1.8cc6767d8db65p+107, 0x1.1dad9cfe3d12ap+106,
	0x1.906249f1ac465p+107, 0x1.004c704e61a6bp+107, 0x1.202a5c6165a5ep+107, 0x1.54b1521487230p+107,
	0x1.673ef2a3f7190p+106, 0x1.0f4057e306261p+107, 0x1.14aace83292f6p+106, 0x1.2e22e3ec9f181p+107,
	0x1.5b05ee452229ep+106, 0x1.7e835e386e8acp+106, 0x1.8b0e0d64fdc72p+106, 0x1.0d6162a2e302ap+106,
	0x1.77d560b29421ep+106, 0x1.2d98b6194283ep+106, 0x1.2a3c89ed8ed8cp+106, 0x1.2f23454fece4ep+106,
};

// 430 (x, correctly-rounded 1/sqrt(x)) pairs, computed with 60-digit decimal arithmetic.
// x values were found by sampling many exponents and refining toward wherever 1/sqrt(x) landed
// closest to a rounding midpoint, then scaled by 4^n (exact) to repeat the same mantissa structure
// across many binades. the closest is about 2^-73 relative to its midpoint.
static const std::pair<double, double> rsqrt_near_midpoint_cases[] = {
	{0x1.52d31e07d2817p-701, 0x1.3ab178e97faf7p+350}, {0x1.52d31e07d2817p-601, 0x1.3ab178e97faf7p+300},
	{0x1.52d31e07d2817p-501, 0x1.3ab178e97faf7p+250}, {0x1.52d31e07d2817p-401, 0x1.3ab178e97faf7p+200},
	{0x1.52d31e07d2817p-301, 0x1.3ab178e97faf7p+150}, {0x1.52d31e07d2817p-201, 0x1.3ab178e97faf7p+100},
	{0x1.52d31e07d2817p-101, 0x1.3ab178e97faf7p+50}, {0x1.52d31e07d2817p-1, 0x1.3ab178e97faf7p+0},
	{0x1.52d31e07d2817p+99, 0x1.3ab178e97faf7p-50}, {0x1.52d31e07d2817p+199, 0x1.3ab178e97faf7p-100},
	{0x1.52d31e07d2817p+299, 0x1.3ab178e97faf7p-150}, {0x1.52d31e07d2817p+399, 0x1.3ab178e97faf7p-200},
	{0x1.52d31e07d2817p+499, 0x1.3ab178e97faf7p-250}, {0x1.52d31e07d2817p+599, 0x1.3ab178e97faf7p-300},
	{0x1.52d31e07d2817p+699, 0x1.3ab178e97faf7p-350}, {0x1.52d31e07d2817p+799, 0x1.3ab178e97faf7p-400},
	{0x1.52d31e07d2817p+899, 0x1.3ab178e97faf7p-450}, {0x0.0000000000630p-1022, 0x1.9ba9da6c73588p+531},
	{0x1.8be119453cffcp-964, 0x1.9bb9eb2e22357p+481}, {0x1.8be119453cffcp-864, 0x1.9bb9eb2e22357p+431},
	{0x1.8be119453cffcp-764, 0x1.9bb9eb2e22357p+381}, {0x1.8be119453cffcp-664, 0x1.9bb9eb2e22357p+331},
	{0x1.8be119453cffcp-564, 0x1.9bb9eb2e22357p+281}, {0x1.8be119453cffcp-464, 0x1.9bb9eb2e22357p+231},
	{0x1.8be119453cffcp-364, 0x1.9bb9eb2e22357p+181}, {0x1.8be119453cffcp-264, 0x1.9bb9eb2e22357p+131},
	{0x1.8be119453cffcp-164, 0x1.9bb9eb2e22357p+81}, {0x1.8be119453cffcp-64, 0x1.9bb9eb2e22357p+31},
	{0x1.8be119453cffcp+36, 0x1.9bb9eb2e22357p-19}, {0x1.8be119453cffcp+136, 0x1.9bb9eb2e22357p-69},
	{0x1.b15e27ff69c3ap+66, 0x1.8983ead9e0301p-34}, {0x1.b15e27ff69c3ap+166, 0x1.8983ead9e0301p-84},
	{0x1.b15e27ff69c3ap+266, 0x1.8983ead9e0301p-134}, {0x1.b15e27ff69c3ap+366, 0x1.8983ead9e0301p-184},
	{0x1.b15e27ff69c3ap+466, 0x1.8983ead9e0301p-234}, {0x1.b15e27ff69c3ap+566, 0x1.8983ead9e0301p-284},
	{0x1.b15e27ff69c3ap+666, 0x1.8983ead9e0301p-334}, {0x1.b15e27ff69c3ap+766, 0x1.8983ead9e0301p-384},
	{0x1.b15e27ff69c3ap+866, 0x1.8983ead9e0301p-434}, {0x1.b15e27ff69c3ap+966, 0x1.8983ead9e0301p-484},
	{0x1.b38412695cd56p-994, 0x1.888b2b4565671p+496}, {0x1.b38412695cd56p-894, 0x1.888b2b4565671p+446},
	{0x1.b38412695cd56p-794, 0x1.888b2b4565671p+396}, {0x1.b38412695cd56p-694, 0x1.888b2b4565671p+346},
	{0x1.b38412695cd56p-594, 0x1.888b2b4565671p+296}, {0x1.b38412695cd56p-494, 0x1.888b2b4565671p+246},
	{0x1.b38412695cd56p-394, 0x1.888b2b4565671p+196}, {0x1.b38412695cd56p-294, 0x1.888b2b4565671p+146},
	{0x1.b38412695cd56p-194, 0x1.888b2b4565671p+96}, {0x1.b38412695cd56p-94, 0x1.888b2b4565671p+46},
	{0x1.b38412695cd56p+6, 0x1.888b2b4565671p-4}, {0x1.b38412695cd56p+106, 0x1.888b2b4565671p-54},
	{0x1.d36c8d618bdf1p-806, 0x1.7ae897db91772p+402}, {0x1.d36c8d618bdf1p-706, 0x1.7ae897db91772p+352},
	{0x1.d36c8d618bdf1p-606, 0x1.7ae897db91772p+302}, {0x1.d36c8d618bdf1p-506, 0x1.7ae897db91772p+252},
	{0x1.d36c8d618bdf1p-406, 0x1.7ae897db91772p+202}, {0x1.d36c8d618bdf1p-306, 0x1.7ae897db91772p+152},
	{0x1.d36c8d618bdf1p-206, 0x1.7ae897db91772p+102}, {0x1.d36c8d618bdf1p-106, 0x1.7ae897db91772p+52},
	{0x1.d36c8d618bdf1p-6, 0x1.7ae897db91772p+2}, {0x1.d36c8d618bdf1p+94, 0x1.7ae897db91772p-48},
	{0x1.d36c8d618bdf1p+194, 0x1.7ae897db91772p-98}, {0x1.d36c8d618bdf1p+294, 0x1.7ae897db91772p-148},
	{0x1.d36c8d618bdf1p+394, 0x1.7ae897db91772p-198}, {0x1.d36c8d618bdf1p+494, 0x1.7ae897db91772p-248},
	{0x1.d36c8d618bdf1p+594, 0x1.7ae897db91772p-298}, {0x1.d36c8d618bdf1p+694, 0x1.7ae897db91772p-348},
	{0x1.d36c8d618bdf1p+794, 0x1.7ae897db91772p-398}, {0x1.824799f42893bp-996, 0x1.a0cf933212f9fp+497},
	{0x1.824799f42893bp-896, 0x1.a0cf933212f9fp+447}, {0x1.824799f42893bp-796, 0x1.a0cf933212f9fp+397},
	{0x1.824799f42893bp-696, 0x1.a0cf933212f9fp+347}, {0x1.824799f42893bp-596, 0x1.a0cf933212f9fp+297},
	{0x1.824799f42893bp-496, 0x1.a0cf933212f9fp+247}, {0x1.824799f42893bp-396, 0x1.a0cf933212f9fp+197},
	{0x1.824799f42893bp-296, 0x1.a0cf933212f9fp+147}, {0x1.824799f42893bp-196, 0x1.a0cf933212f9fp+97},
	{0x1.824799f42893bp-96, 0x1.a0cf933212f9fp+47}, {0x1.824799f42893bp+4, 0x1.a0cf933212f9fp-3},
	{0x1.824799f42893bp+104, 0x1.a0cf933212f9fp-53}, {0x1.824799f42893bp+204, 0x1.a0cf933212f9fp-103},
	{0x1.824799f42893bp+304, 0x1.a0cf933212f9fp-153}, {0x1.824799f42893bp+404, 0x1.a0cf933212f9fp-203},
	{0x1.824799f42893bp+504, 0x1.a0cf933212f9fp-253}, {0x0.00000000046adp-1022, 0x1.e7382f7b312afp+529},
	{0x1.1ab3b20f5bab2p-960, 0x1.e73872a4c0929p+479}, {0x1.1ab3b20f5bab2p-860, 0x1.e73872a4c0929p+429},
	{0x1.1ab3b20f5bab2p-760, 0x1.e73872a4c0929p+379}, {0x1.1ab3b20f5bab2p-660, 0x1.e73872a4c0929p+329},
	{0x1.1ab3b20f5bab2p-560, 0x1.e73872a4c0929p+279}, {0x1.1ab3b20f5bab2p-460, 0x1.e73872a4c0929p+229},
	{0x1.1ab3b20f5bab2p-360, 0x1.e73872a4c0929p+179}, {0x1.1ab3b20f5bab2p-260, 0x1.e73872a4c0929p+129},
	{0x1.1ab3b20f5bab2p-160, 0x1.e73872a4c0929p+79}, {0x1.1ab3b20f5bab2p-60, 0x1.e73872a4c0929p+29},
	{0x1.1ab3b20f5bab2p+40, 0x1.e73872a4c0929p-21}, {0x1.1ab3b20f5bab2p+140, 0x1.e73872a4c0929p-71},
	{0x1.1ab3b20f5bab2p+240, 0x1.e73872a4c0929p-121}, {0x1.1ab3b20f5bab2p+340, 0x1.e73872a4c0929p-171},
	{0x1.1ab3b20f5bab2p+440, 0x1.e73872a4c0929p-221}, {0x1.1ab3b20f5bab2p+540, 0x1.e73872a4c0929p-271},
	{0x1.39741149b63bfp-998, 0x1.ceb447a85b259p+498}, {0x1.39741149b63bfp-898, 0x1.ceb447a85b259p+448},
	{0x1.39741149b63bfp-798, 0x1.ceb447a85b259p+398}, {0x1.39741149b63bfp-698, 0x1.ceb447a85b259p+348},
	{0x1.39741149b63bfp-598, 0x1.ceb447a85b259p+298}, {0x1.39741149b63bfp-498, 0x1.ceb447a85b259p+248},
	{0x1.39741149b63bfp-398, 0x1.ceb447a85b259p+198}, {0x1.39741149b63bfp-298, 0x1.ceb447a85b259p+148},
	{0x1.39741149b63bfp-198, 0x1.ceb447a85b259p+98}, {0x1.39741149b63bfp-98, 0x1.ceb447a85b259p+48},
	{0x1.39741149b63bfp+2, 0x1.ceb447a85b259p-2}, {0x1.325ba5e7bc4c6p-1018, 0x1.d40821d742752p+508},
	{0x1.325ba5e7bc4c6p-918, 0x1.d40821d742752p+458}, {0x1.325ba5e7bc4c6p-818, 0x1.d40821d742752p+408},
	{0x1.325ba5e7bc4c6p-718, 0x1.d40821d742752p+358}, {0x1.325ba5e7bc4c6p-618, 0x1.d40821d742752p+308},
	{0x1.325ba5e7bc4c6p-518, 0x1.d40821d742752p+258}, {0x1.325ba5e7bc4c6p-418, 0x1.d40821d742752p+208},
	{0x1.325ba5e7bc4c6p-318, 0x1.d40821d742752p+158}, {0x1.325ba5e7bc4c6p-218, 0x1.d40821d742752p+108},
	{0x1.325ba5e7bc4c6p-118, 0x1.d40821d742752p+58}, {0x1.325ba5e7bc4c6p-18, 0x1.d40821d742752p+8},
	{0x1.2861b68c2dceap-934, 0x1.dbd7f7f3a887bp+466}, {0x1.2861b68c2dceap-834, 0x1.dbd7f7f3a887bp+416},
	{0x1.2861b68c2dceap-734, 0x1.dbd7f7f3a887bp+366}, {0x1.2861b68c2dceap-634, 0x1.dbd7f7f3a887bp+316},
	{0x1.2861b68c2dceap-534, 0x1.dbd7f7f3a887bp+266}, {0x1.2861b68c2dceap-434, 0x1.dbd7f7f3a887bp+216},
	{0x1.2861b68c2dceap-334, 0x1.dbd7f7f3a887bp+166}, {0x1.2861b68c2dceap-234, 0x1.dbd7f7f3a887bp+116},
	{0x1.2861b68c2dceap-134, 0x1.dbd7f7f3a887bp+66}, {0x1.2861b68c2dceap-34, 0x1.dbd7f7f3a887bp+16},
	{0x1.2861b68c2dceap+66, 0x1.dbd7f7f3a887bp-34}, {0x1.2861b68c2dceap+166, 0x1.dbd7f7f3a887bp-84},
	{0x1.2861b68c2dceap+266, 0x1.dbd7f7f3a887bp-134}, {0x1.2861b68c2dceap+366, 0x1.dbd7f7f3a887bp-184},
	{0x1.2861b68c2dceap+466, 0x1.dbd7f7f3a887bp-234}, {0x1.2861b68c2dceap+566, 0x1.dbd7f7f3a887bp-284},
	{0x1.2861b68c2dceap+666, 0x1.dbd7f7f3a887bp-334}, {0x1.be342cfa9097fp-915, 0x1.1239cddbe08d3p+457},
	{0x1.be342cfa9097fp-815, 0x1.1239cddbe08d3p+407}, {0x1.be342cfa9097fp-715, 0x1.1239cddbe08d3p+357},
	{0x1.be342cfa9097fp-615, 0x1.1239cddbe08d3p+307}, {0x1.be342cfa9097fp-515, 0x1.1239cddbe08d3p+257},
	{0x1.be342cfa9097fp-415, 0x1.1239cddbe08d3p+207}, {0x1.be342cfa9097fp-315, 0x1.1239cddbe08d3p+157},
	{0x1.be342cfa9097fp-215, 0x1.1239cddbe08d3p+107}, {0x1.be342cfa9097fp-115, 0x1.1239cddbe08d3p+57},
	{0x1.be342cfa9097fp-15, 0x1.1239cddbe08d3p+7}, {0x1.be342cfa9097fp+85, 0x1.1239cddbe08d3p-43},
	{0x1.be342cfa9097fp+185, 0x1.1239cddbe08d3p-93}, {0x1.be342cfa9097fp+285, 0x1.1239cddbe08d3p-143},
	{0x1.be342cfa9097fp+385, 0x1.1239cddbe08d3p-193}, {0x1.be342cfa9097fp+485, 0x1.1239cddbe08d3p-243},
	{0x1.be342cfa9097fp+585, 0x1.1239cddbe08d3p-293}, {0x1.be342cfa9097fp+685, 0x1.1239cddbe08d3p-343},
	{0x1.ee6fecbd3db3dp-987, 0x1.0481c9d2a77ffp+493}, {0x1.ee6fecbd3db3dp-887, 0x1.0481c9d2a77ffp+443},
	{0x1.ee6fecbd3db3dp-787, 0x1.0481c9d2a77ffp+393}, {0x1.ee6fecbd3db3dp-687, 0x1.0481c9d2a77ffp+343},
	{0x1.ee6fecbd3db3dp-587, 0x1.0481c9d2a77ffp+293}, {0x1.ee6fecbd3db3dp-487, 0x1.0481c9d2a77ffp+243},
	{0x1.ee6fecbd3db3dp-387, 0x1.0481c9d2a77ffp+193}, {0x1.ee6fecbd3db3dp-287, 0x1.0481c9d2a77ffp+143},
	{0x1.ee6fecbd3db3dp-187, 0x1.0481c9d2a77ffp+93}, {0x1.ee6fecbd3db3dp-87, 0x1.0481c9d2a77ffp+43},
	{0x1.ee6fecbd3db3dp+13, 0x1.0481c9d2a77ffp-7}, {0x1.ee6fecbd3db3dp+113, 0x1.0481c9d2a77ffp-57},
	{0x1.ee6fecbd3db3dp+213, 0x1.0481c9d2a77ffp-107}, {0x1.ee6fecbd3db3dp+313, 0x1.0481c9d2a77ffp-157},
	{0x1.ee6fecbd3db3dp+413, 0x1.0481c9d2a77ffp-207}, {0x1.ee6fecbd3db3dp+513, 0x1.0481c9d2a77ffp-257},
	{0x1.ee6fecbd3db3dp+613, 0x1.0481c9d2a77ffp-307}, {0x0.045e669f1720fp-1022, 0x1.e9ea5f1c6262cp+513},
	{0x1.1799a7c5c83d4p-928, 0x1.e9ea5f1c6261ap+463}, {0x1.1799a7c5c83d4p-828, 0x1.e9ea5f1c6261ap+413},
	{0x1.1799a7c5c83d4p-728, 0x1.e9ea5f1c6261ap+363}, {0x1.1799a7c5c83d4p-628, 0x1.e9ea5f1c6261ap+313},
	{0x1.1799a7c5c83d4p-528, 0x1.e9ea5f1c6261ap+263}, {0x1.1799a7c5c83d4p-428, 0x1.e9ea5f1c6261ap+213},
	{0x1.1799a7c5c83d4p-328, 0x1.e9ea5f1c6261ap+163}, {0x1.1799a7c5c83d4p-228, 0x1.e9ea5f1c6261ap+113},
	{0x1.1799a7c5c83d4p-128, 0x1.e9ea5f1c6261ap+63}, {0x1.1799a7c5c83d4p-28, 0x1.e9ea5f1c6261ap+13},
	{0x1.1799a7c5c83d4p+72, 0x1.e9ea5f1c6261ap-37}, {0x1.1799a7c5c83d4p+172, 0x1.e9ea5f1c6261ap-87},
	{0x1.56ab08b1c0d28p+97, 0x1.38ec6712c5f45p-49}, {0x1.56ab08b1c0d28p+197, 0x1.38ec6712c5f45p-99},
	{0x1.56ab08b1c0d28p+297, 0x1.38ec6712c5f45p-149}, {0x1.56ab08b1c0d28p+397, 0x1.38ec6712c5f45p-199},
	{0x1.56ab08b1c0d28p+497, 0x1.38ec6712c5f45p-249}, {0x1.56ab08b1c0d28p+597, 0x1.38ec6712c5f45p-299},
	{0x1.56ab08b1c0d28p+697, 0x1.38ec6712c5f45p-349}, {0x1.56ab08b1c0d28p+797, 0x1.38ec6712c5f45p-399},
	{0x1.56ab08b1c0d28p+897, 0x1.38ec6712c5f45p-449}, {0x1.56ab08b1c0d28p+997, 0x1.38ec6712c5f45p-499},
	{0x1.429a706cffaa9p-485, 0x1.42820f7d5fee0p+242}, {0x1.429a706cffaa9p-385, 0x1.42820f7d5fee0p+192},
	{0x1.429a706cffaa9p-285, 0x1.42820f7d5fee0p+142}, {0x1.429a706cffaa9p-185, 0x1.42820f7d5fee0p+92},
	{0x1.429a706cffaa9p-85, 0x1.42820f7d5fee0p+42}, {0x1.429a706cffaa9p+15, 0x1.42820f7d5fee0p-8},
	{0x1.429a706cffaa9p+115, 0x1.42820f7d5fee0p-58}, {0x1.429a706cffaa9p+215, 0x1.42820f7d5fee0p-108},
	{0x1.429a706cffaa9p+315, 0x1.42820f7d5fee0p-158}, {0x1.429a706cffaa9p+415, 0x1.42820f7d5fee0p-208},
	{0x1.429a706cffaa9p+515, 0x1.42820f7d5fee0p-258}, {0x1.429a706cffaa9p+615, 0x1.42820f7d5fee0p-308},
	{0x1.429a706cffaa9p+715, 0x1.42820f7d5fee0p-358}, {0x1.429a706cffaa9p+815, 0x1.42820f7d5fee0p-408},
	{0x1.429a706cffaa9p+915, 0x1.42820f7d5fee0p-458}, {0x1.429a706cffaa9p+1015, 0x1.42820f7d5fee0p-508},
	{0x1.98b83e1a92bb9p-984, 0x1.95350b6a9532dp+491}, {0x1.98b83e1a92bb9p-884, 0x1.95350b6a9532dp+441},
	{0x1.98b83e1a92bb9p-784, 0x1.95350b6a9532dp+391}, {0x1.98b83e1a92bb9p-684, 0x1.95350b6a9532dp+341},
	{0x1.98b83e1a92bb9p-584, 0x1.95350b6a9532dp+291}, {0x1.98b83e1a92bb9p-484, 0x1.95350b6a9532dp+241},
	{0x1.98b83e1a92bb9p-384, 0x1.95350b6a9532dp+191}, {0x1.98b83e1a92bb9p-284, 0x1.95350b6a9532dp+141},
	{0x1.98b83e1a92bb9p-184, 0x1.95350b6a9532dp+91}, {0x1.98b83e1a92bb9p-84, 0x1.95350b6a9532dp+41},
	{0x1.89d6c97fe99f0p-1009, 0x1.23e336d27badbp+504}, {0x1.89d6c97fe99f0p-909, 0x1.23e336d27badbp+454},
	{0x1.89d6c97fe99f0p-809, 0x1.23e336d27badbp+404}, {0x1.89d6c97fe99f0p-709, 0x1.23e336d27badbp+354},
	{0x1.89d6c97fe99f0p-609, 0x1.23e336d27badbp+304}, {0x1.89d6c97fe99f0p-509, 0x1.23e336d27badbp+254},
	{0x1.89d6c97fe99f0p-409, 0x1.23e336d27badbp+204}, {0x1.89d6c97fe99f0p-309, 0x1.23e336d27badbp+154},
	{0x1.89d6c97fe99f0p-209, 0x1.23e336d27badbp+104}, {0x1.89d6c97fe99f0p-109, 0x1.23e336d27badbp+54},
	{0x1.89d6c97fe99f0p-9, 0x1.23e336d27badbp+4}, {0x1.89d6c97fe99f0p+91, 0x1.23e336d27badbp-46},
	{0x1.89d6c97fe99f0p+191, 0x1.23e336d27badbp-96}, {0x1.89d6c97fe99f0p+291, 0x1.23e336d27badbp-146},
	{0x1.f53a13538c0c0p-34, 0x1.6de8a2d54d921p+16}, {0x1.f53a13538c0c0p+66, 0x1.6de8a2d54d921p-34},
	{0x1.f53a13538c0c0p+166, 0x1.6de8a2d54d921p-84}, {0x1.f53a13538c0c0p+266, 0x1.6de8a2d54d921p-134},
	{0x1.f53a13538c0c0p+366, 0x1.6de8a2d54d921p-184}, {0x1.f53a13538c0c0p+466, 0x1.6de8a2d54d921p-234},
	{0x1.f53a13538c0c0p+566, 0x1.6de8a2d54d921p-284}, {0x1.f53a13538c0c0p+666, 0x1.6de8a2d54d921p-334},
	{0x1.f53a13538c0c0p+766, 0x1.6de8a2d54d921p-384}, {0x1.f53a13538c0c0p+866, 0x1.6de8a2d54d921p-434},
	{0x1.f53a13538c0c0p+966, 0x1.6de8a2d54d921p-484}, {0x1.208a803d11a15p+202, 0x1.e243f57c2db2cp-102},
	{0x1.208a803d11a15p+302, 0x1.e243f57c2db2cp-152}, {0x1.208a803d11a15p+402, 0x1.e243f57c2db2cp-202},
	{0x1.208a803d11a15p+502, 0x1.e243f57c2db2cp-252}, {0x1.208a803d11a15p+602, 0x1.e243f57c2db2cp-302},
	{0x1.208a803d11a15p+702, 0x1.e243f57c2db2cp-352}, {0x1.208a803d11a15p+802, 0x1.e243f57c2db2cp-402},
	{0x1.208a803d11a15p+902, 0x1.e243f57c2db2cp-452}, {0x1.208a803d11a15p+1002, 0x1.e243f57c2db2cp-502},
	{0x1.1751f56a379aep-683, 0x1.5a98a156d7fe7p+341}, {0x1.1751f56a379aep-583, 0x1.5a98a156d7fe7p+291},
	{0x1.1751f56a379aep-483, 0x1.5a98a156d7fe7p+241}, {0x1.1751f56a379aep-383, 0x1.5a98a156d7fe7p+191},
	{0x1.1751f56a379aep-283, 0x1.5a98a156d7fe7p+141}, {0x1.1751f56a379aep-183, 0x1.5a98a156d7fe7p+91},
	{0x1.1751f56a379aep-83, 0x1.5a98a156d7fe7p+41}, {0x1.1751f56a379aep+17, 0x1.5a98a156d7fe7p-9},
	{0x1.1751f56a379aep+117, 0x1.5a98a156d7fe7p-59}, {0x1.1751f56a379aep+217, 0x1.5a98a156d7fe7p-109},
	{0x1.1751f56a379aep+317, 0x1.5a98a156d7fe7p-159}, {0x1.1751f56a379aep+417, 0x1.5a98a156d7fe7p-209},
	{0x1.1751f56a379aep+517, 0x1.5a98a156d7fe7p-259}, {0x1.1751f56a379aep+617, 0x1.5a98a156d7fe7p-309},
	{0x1.1751f56a379aep+717, 0x1.5a98a156d7fe7p-359}, {0x1.1751f56a379aep+817, 0x1.5a98a156d7fe7p-409},
	{0x1.1751f56a379aep+917, 0x1.5a98a156d7fe7p-459}, {0x1.ebc052da1d3afp-991, 0x1.0537ab8013676p+495},
	{0x1.ebc052da1d3afp-891, 0x1.0537ab8013676p+445}, {0x1.ebc052da1d3afp-791, 0x1.0537ab8013676p+395},
	{0x1.ebc052da1d3afp-691, 0x1.0537ab8013676p+345}, {0x1.ebc052da1d3afp-591, 0x1.0537ab8013676p+295},
	{0x1.ebc052da1d3afp-491, 0x1.0537ab8013676p+245}, {0x1.ebc052da1d3afp-391, 0x1.0537ab8013676p+195},
	{0x1.ebc052da1d3afp-291, 0x1.0537ab8013676p+145}, {0x1.ebc052da1d3afp-191, 0x1.0537ab8013676p+95},
	{0x1.ebc052da1d3afp-91, 0x1.0537ab8013676p+45}, {0x1.ebc052da1d3afp+9, 0x1.0537ab8013676p-5},
	{0x1.ebc052da1d3afp+109, 0x1.0537ab8013676p-55}, {0x1.ebc052da1d3afp+209, 0x1.0537ab8013676p-105},
	{0x1.ebc052da1d3afp+309, 0x1.0537ab8013676p-155}, {0x1.ebc052da1d3afp+409, 0x1.0537ab8013676p-205},
	{0x1.ebc052da1d3afp+509, 0x1.0537ab8013676p-255}, {0x0.0000000198d47p-1022, 0x1.95271234c1c04p+526},
	{0x1.98d475c40acbdp-954, 0x1.95270f595fbdbp+476}, {0x1.98d475c40acbdp-854, 0x1.95270f595fbdbp+426},
	{0x1.98d475c40acbdp-754, 0x1.95270f595fbdbp+376}, {0x1.98d475c40acbdp-654, 0x1.95270f595fbdbp+326},
	{0x1.98d475c40acbdp-554, 0x1.95270f595fbdbp+276}, {0x1.98d475c40acbdp-454, 0x1.95270f595fbdbp+226},
	{0x1.98d475c40acbdp-354, 0x1.95270f595fbdbp+176}, {0x1.98d475c40acbdp-254, 0x1.95270f595fbdbp+126},
	{0x1.98d475c40acbdp-154, 0x1.95270f595fbdbp+76}, {0x1.98d475c40acbdp-54, 0x1.95270f595fbdbp+26},
	{0x1.98d475c40acbdp+46, 0x1.95270f595fbdbp-24}, {0x1.98d475c40acbdp+146, 0x1.95270f595fbdbp-74},
	{0x1.360e7c91eb0c8p+73, 0x1.48f7f70666ed2p-37}, {0x1.360e7c91eb0c8p+173, 0x1.48f7f70666ed2p-87},
	{0x1.360e7c91eb0c8p+273, 0x1.48f7f70666ed2p-137}, {0x1.360e7c91eb0c8p+373, 0x1.48f7f70666ed2p-187},
	{0x1.360e7c91eb0c8p+473, 0x1.48f7f70666ed2p-237}, {0x1.360e7c91eb0c8p+573, 0x1.48f7f70666ed2p-287},
	{0x1.360e7c91eb0c8p+673, 0x1.48f7f70666ed2p-337}, {0x1.360e7c91eb0c8p+773, 0x1.48f7f70666ed2p-387},
	{0x1.360e7c91eb0c8p+873, 0x1.48f7f70666ed2p-437}, {0x1.360e7c91eb0c8p+973, 0x1.48f7f70666ed2p-487},
	{0x1.5c021c1c23534p-115, 0x1.3683667061df1p+57}, {0x1.5c021c1c23534p-15, 0x1.3683667061df1p+7},
	{0x1.5c021c1c23534p+85, 0x1.3683667061df1p-43}, {0x1.5c021c1c23534p+185, 0x1.3683667061df1p-93},
	{0x1.5c021c1c23534p+285, 0x1.3683667061df1p-143}, {0x1.5c021c1c23534p+385, 0x1.3683667061df1p-193},
	{0x1.5c021c1c23534p+485, 0x1.3683667061df1p-243}, {0x1.5c021c1c23534p+585, 0x1.3683667061df1p-293},
	{0x1.5c021c1c23534p+685, 0x1.3683667061df1p-343}, {0x1.5c021c1c23534p+785, 0x1.3683667061df1p-393},
	{0x1.5c021c1c23534p+885, 0x1.3683667061df1p-443}, {0x1.5c021c1c23534p+985, 0x1.3683667061df1p-493},
	{0x0.003b99077cbaep-1022, 0x1.09494fe1701edp+516}, {0x1.dcc83be5d6d10p-933, 0x1.09494fe1702bep+466},
	{0x1.dcc83be5d6d10p-833, 0x1.09494fe1702bep+416}, {0x1.dcc83be5d6d10p-733, 0x1.09494fe1702bep+366},
	{0x1.dcc83be5d6d10p-633, 0x1.09494fe1702bep+316}, {0x1.dcc83be5d6d10p-533, 0x1.09494fe1702bep+266},
	{0x1.dcc83be5d6d10p-433, 0x1.09494fe1702bep+216}, {0x1.dcc83be5d6d10p-333, 0x1.09494fe1702bep+166},
	{0x1.dcc83be5d6d10p-233, 0x1.09494fe1702bep+116}, {0x1.dcc83be5d6d10p-133, 0x1.09494fe1702bep+66},
	{0x1.dcc83be5d6d10p-33, 0x1.09494fe1702bep+16}, {0x1.dcc83be5d6d10p+67, 0x1.09494fe1702bep-34},
	{0x1.dcc83be5d6d10p+167, 0x1.09494fe1702bep-84}, {0x1.dcc83be5d6d10p+267, 0x1.09494fe1702bep-134},
	{0x0.1309ff5ba4d82p-1022, 0x1.d55c7e983b74cp+512}, {0x1.309ff5ba4d81ap-926, 0x1.d55c7e983b750p+462},
	{0x1.309ff5ba4d81ap-826, 0x1.d55c7e983b750p+412}, {0x1.309ff5ba4d81ap-726, 0x1.d55c7e983b750p+362},
	{0x1.309ff5ba4d81ap-626, 0x1.d55c7e983b750p+312}, {0x1.309ff5ba4d81ap-526, 0x1.d55c7e983b750p+262},
	{0x1.309ff5ba4d81ap-426, 0x1.d55c7e983b750p+212}, {0x1.309ff5ba4d81ap-326, 0x1.d55c7e983b750p+162},
	{0x1.309ff5ba4d81ap-226, 0x1.d55c7e983b750p+112}, {0x1.309ff5ba4d81ap-126, 0x1.d55c7e983b750p+62},
	{0x0.00000000318f3p-1022, 0x1.22ea6f8986c83p+528}, {0x1.8c799dbaebf91p-957, 0x1.22ea64a13f6e6p+478},
	{0x1.8c799dbaebf91p-857, 0x1.22ea64a13f6e6p+428}, {0x1.8c799dbaebf91p-757, 0x1.22ea64a13f6e6p+378},
	{0x1.8c799dbaebf91p-657, 0x1.22ea64a13f6e6p+328}, {0x1.8c799dbaebf91p-557, 0x1.22ea64a13f6e6p+278},
	{0x1.8c799dbaebf91p-457, 0x1.22ea64a13f6e6p+228}, {0x1.8c799dbaebf91p-357, 0x1.22ea64a13f6e6p+178},
	{0x1.8c799dbaebf91p-257, 0x1.22ea64a13f6e6p+128}, {0x1.8c799dbaebf91p-157, 0x1.22ea64a13f6e6p+78},
	{0x1.8c799dbaebf91p-57, 0x1.22ea64a13f6e6p+28}, {0x1.8c799dbaebf91p+43, 0x1.22ea64a13f6e6p-22},
	{0x1.8c799dbaebf91p+143, 0x1.22ea64a13f6e6p-72}, {0x1.8c799dbaebf91p+243, 0x1.22ea64a13f6e6p-122},
	{0x1.8c799dbaebf91p+343, 0x1.22ea64a13f6e6p-172}, {0x1.8c799dbaebf91p+443, 0x1.22ea64a13f6e6p-222},
	{0x1.24d10da9a5cd4p-986, 0x1.debb49770ced5p+492}, {0x1.24d10da9a5cd4p-886, 0x1.debb49770ced5p+442},
	{0x1.24d10da9a5cd4p-786, 0x1.debb49770ced5p+392}, {0x1.24d10da9a5cd4p-686, 0x1.debb49770ced5p+342},
	{0x1.24d10da9a5cd4p-586, 0x1.debb49770ced5p+292}, {0x1.24d10da9a5cd4p-486, 0x1.debb49770ced5p+242},
	{0x1.24d10da9a5cd4p-386, 0x1.debb49770ced5p+192}, {0x1.24d10da9a5cd4p-286, 0x1.debb49770ced5p+142},
	{0x1.24d10da9a5cd4p-186, 0x1.debb49770ced5p+92}, {0x1.24d10da9a5cd4p-86, 0x1.debb49770ced5p+42},
	{0x1.06048ac71242cp-338, 0x1.fa1618b438129p+168}, {0x1.06048ac71242cp-238, 0x1.fa1618b438129p+118},
	{0x1.06048ac71242cp-138, 0x1.fa1618b438129p+68}, {0x1.06048ac71242cp-38, 0x1.fa1618b438129p+18},
	{0x1.06048ac71242cp+62, 0x1.fa1618b438129p-32}, {0x1.06048ac71242cp+162, 0x1.fa1618b438129p-82},
	{0x1.06048ac71242cp+262, 0x1.fa1618b438129p-132}, {0x1.06048ac71242cp+362, 0x1.fa1618b438129p-182},
	{0x1.06048ac71242cp+462, 0x1.fa1618b438129p-232}, {0x1.06048ac71242cp+562, 0x1.fa1618b438129p-282},
	{0x1.06048ac71242cp+662, 0x1.fa1618b438129p-332}, {0x1.06048ac71242cp+762, 0x1.fa1618b438129p-382},
	{0x1.06048ac71242cp+862, 0x1.fa1618b438129p-432}, {0x1.06048ac71242cp+962, 0x1.fa1618b438129p-482},
	{0x1.a64f7609d6340p-378, 0x1.8ea23d020eb61p+188}, {0x1.a64f7609d6340p-278, 0x1.8ea23d020eb61p+138},
	{0x1.a64f7609d6340p-178, 0x1.8ea23d020eb61p+88}, {0x1.a64f7609d6340p-78, 0x1.8ea23d020eb61p+38},
	{0x1.a64f7609d6340p+22, 0x1.8ea23d020eb61p-12}, {0x1.a64f7609d6340p+122, 0x1.8ea23d020eb61p-62},
	{0x1.a64f7609d6340p+222, 0x1.8ea23d020eb61p-112}, {0x1.a64f7609d6340p+322, 0x1.8ea23d020eb61p-162},
	{0x1.a64f7609d6340p+422, 0x1.8ea23d020eb61p-212}, {0x1.a64f7609d6340p+522, 0x1.8ea23d020eb61p-262},
	{0x1.a64f7609d6340p+622, 0x1.8ea23d020eb61p-312}, {0x1.a64f7609d6340p+722, 0x1.8ea23d020eb61p-362},
	{0x1.a64f7609d6340p+822, 0x1.8ea23d020eb61p-412}, {0x1.a64f7609d6340p+922, 0x1.8ea23d020eb61p-462},
	{0x1.a64f7609d6340p+1022, 0x1.8ea23d020eb61p-512}, {0x1.83e3f0993e6a1p-141, 0x1.261e0164b9d60p+70},
	{0x1.83e3f0993e6a1p-41, 0x1.261e0164b9d60p+20}, {0x1.83e3f0993e6a1p+59, 0x1.261e0164b9d60p-30},
	{0x1.83e3f0993e6a1p+159, 0x1.261e0164b9d60p-80}, {0x1.83e3f0993e6a1p+259, 0x1.261e0164b9d60p-130},
	{0x1.83e3f0993e6a1p+359, 0x1.261e0164b9d60p-180}, {0x1.83e3f0993e6a1p+459, 0x1.261e0164b9d60p-230},
	{0x1.83e3f0993e6a1p+559, 0x1.261e0164b9d60p-280}, {0x1.83e3f0993e6a1p+659, 0x1.261e0164b9d60p-330},
	{0x1.83e3f0993e6a1p+759, 0x1.261e0164b9d60p-380}, {0x1.83e3f0993e6a1p+859, 0x1.261e0164b9d60p-430},
	{0x1.83e3f0993e6a1p+959, 0x1.261e0164b9d60p-480}, {0x1.e0cdad61c10c4p-181, 0x1.082cb6f996fc6p+90},
	{0x1.e0cdad61c10c4p-81, 0x1.082cb6f996fc6p+40}, {0x1.e0cdad61c10c4p+19, 0x1.082cb6f996fc6p-10},
	{0x1.e0cdad61c10c4p+119, 0x1.082cb6f996fc6p-60}, {0x1.e0cdad61c10c4p+219, 0x1.082cb6f996fc6p-110},
	{0x1.e0cdad61c10c4p+319, 0x1.082cb6f996fc6p-160}, {0x1.e0cdad61c10c4p+419, 0x1.082cb6f996fc6p-210},
	{0x1.e0cdad61c10c4p+519, 0x1.082cb6f996fc6p-260}, {0x1.e0cdad61c10c4p+619, 0x1.082cb6f996fc6p-310},
	{0x1.e0cdad61c10c4p+719, 0x1.082cb6f996fc6p-360}, {0x1.e0cdad61c10c4p+819, 0x1.082cb6f996fc6p-410},
	{0x1.e0cdad61c10c4p+919, 0x1.082cb6f996fc6p-460}, {0x1.e0cdad61c10c4p+1019, 0x1.082cb6f996fc6p-510},
};

// x here was found by sampling many exponents and checking how close round(1/sqrt(x)) landed to a rounding
// midpoint; each is then scaled by 4^n (exact) to repeat the same mantissa structure across many binades.
// "want" is 1/sqrt(x) correctly rounded, computed with 60-digit decimal arithmetic (see the generating script
// referenced in the project notes) -- not with long double, which isn't guaranteed to have extra precision.
static void test_rsqrt_near_midpoints()
{
	using namespace cxcm::impl;

	int checked = 0;

	for (auto& [x, want] : rsqrt_near_midpoint_cases)
	{
		volatile double v = x;
		const double got = constexpr_rsqrt(double(v));
		CHECK(same_bits(got, want), "rsqrt(%a) = %a, want %a", x, got, want);
		++checked;
	}

	CHECK(checked == static_cast<int>(std::size(rsqrt_near_midpoint_cases)), "expected to check all near-midpoint cases");

	// and evaluated in a real constant expression
	{
		constexpr double a = cxcm::rsqrt(0x1.a64f7609d6340p+922);
		CHECK(same_bits(a, 0x1.8ea23d020eb61p-462), "consteval rsqrt near midpoint: %.17g", a);
	}
}

static void test_sqrt_near_midpoints()
{
	using namespace cxcm::impl;

	for (double x0 : near_midpoint_inputs)
	{
		// scaling by 4^n keeps the same mantissa structure in every binade
		for (int n = -440; n <= 440; n += 11)
		{
			volatile double x = std::ldexp(x0, 2 * n);
			const double c = constexpr_sqrt(double(x));
			const double s = std::sqrt(double(x));
			CHECK(same_bits(c, s), "sqrt(%a): %.17g vs %.17g", (double)x, c, s);
		}
	}

	// and evaluated in a real constant expression
	{
		constexpr double a = cxcm::sqrt(0x1.fffffffffffffp+107), b = cxcm::sqrt(0x1.ffffffffffffdp+107);
		volatile double v1 = 0x1.fffffffffffffp+107, v2 = 0x1.ffffffffffffdp+107;
		CHECK(same_bits(a, std::sqrt(double(v1))), "consteval sqrt near midpoint: %.17g", a);
		CHECK(same_bits(b, std::sqrt(double(v2))), "consteval sqrt near midpoint: %.17g", b);
	}
}

static void integer_abs_runtime()
{
	auto throws = [](auto f) { try { (void)f(); return false; } catch (const std::domain_error&) { return true; } };

	volatile unsigned u0 = 0;
	volatile int imin = INT_MIN, i0 = 0;
	volatile long long llmin = LLONG_MIN;

	CHECK(!throws([&] { return cxcm::abs(unsigned(u0)); }), "abs(0u) must not throw");
	CHECK(!throws([&] { return cxcm::fabs(unsigned(u0)); }), "fabs(0u) must not throw");
	CHECK(!throws([&] { return cxcm::fabs(int(imin)); }), "fabs(INT_MIN) must not throw");
	CHECK(!throws([&] { return cxcm::fabs((long long)(llmin)); }), "fabs(LLONG_MIN) must not throw");
	CHECK(!throws([&] { return cxcm::abs(int(i0)); }), "abs(0) must not throw");
	CHECK(throws([&] { return cxcm::abs(int(imin)); }), "abs(INT_MIN) should still throw (not representable)");
	CHECK(throws([&] { return cxcm::abs((long long)(llmin)); }), "abs(LLONG_MIN) should still throw");
}

int claude_main()
{
	std::mt19937_64 rng(12345);

	part2_consteval_vs_std();
	integer_abs_runtime();
	test_sqrt_near_midpoints();
	test_rsqrt_near_midpoints();

	fuzz_rounding<double>(rng, 300000);
	fuzz_rounding<float>(rng, 300000);
	fuzz_fmod<double>(rng, 1000000);
	fuzz_fmod<float>(rng, 1000000);
	fuzz_sqrt<double>(rng, 500000);
	fuzz_sqrt<float>(rng, 500000);

	if (failures == 0)
		std::printf("all tests passed\n");
	else
		std::printf("%d failure(s)\n", failures);

	return failures == 0 ? 0 : 1;
}
