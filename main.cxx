
//          Copyright David Browne 2020-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include "cxcm.hxx"

#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"


void test_floor_float(unsigned int i, long long &above, long long &below, long long &same)
{
	float f = std::bit_cast<float>(i);

	auto val1 = std::floor(f);
	auto val2 = cxcm::detail::constexpr_floor(f);

	auto res1 = std::bit_cast<unsigned int>(val1);
	auto res2 = std::bit_cast<unsigned int>(val2);

	if (res1 < res2)
	{
		++above;
		std::printf("trouble maker above?  %X: %X < %X\n", i, res1, res2);
	}
	else if (res1 > res2)
	{
		++below;
		std::printf("trouble maker below?  %X: %X > %X\n", i, res1, res2);
	}
	else
	{
		++same;
	}
}

void test_all_floats_floor()
{
	long long below{};
	long long above{};
	long long same{};
	test_floor_float(0xFFFFFFFF, above, below, same);
	for (unsigned int i = 0x00000000; i < 0xFFFFFFFF; ++i)
	{
		test_floor_float(i, above, below, same);
	}

	std::printf("same std : %lld\n", same);
	std::printf("below std : %lld\n", below);
	std::printf("above std : %lld\n", above);
}



int main(int argc, char *argv[])
{
	//
	// fun stuff
	//

	test_all_floats_floor();

	[[ maybe_unused ]] auto a = cxcm::fabs(-3);


	//
	// doctest
	//

	int doctest_result = 0;

// comment out if we define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN somewhere to get the main() from doctest
#define DONT_USE_DOCTEST_MAIN

#if defined(DONT_USE_DOCTEST_MAIN)

	doctest::Context context;
	context.applyCommandLine(argc, argv);

	doctest_result = context.run();				// run

	if (context.shouldExit())					// important - query flags (and --exit) rely on the user doing this
		return doctest_result;					// propagate the result of the tests

#endif


	return EXIT_SUCCESS + doctest_result;
}
