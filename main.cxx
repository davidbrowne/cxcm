
//          Copyright David Browne 2020-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include "cxcm.hxx"
#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"

double fast_sqrt_simple(double x)
{
	double a = std::floor(std::log2(x));
	double b = std::ceil(std::log2(x));
	double lower_bound = std::pow(2, std::floor(a / 2.0));
	double upper_bound = std::pow(2, std::floor(b / 2.0));

	return (lower_bound + upper_bound) / 2.0;
}



void test_sqrt_float(unsigned int i, long long &above, long long &below, long long &same)
{
	float f = std::bit_cast<float>(i);

	auto val1 = std::sqrt(f);
	auto val2 = cxcm::detail::constexpr_sqrt(f);

	auto res1 = std::bit_cast<unsigned int>(val1);
	auto res2 = std::bit_cast<unsigned int>(val2);

	if (res1 < res2)
	{
		++above;
//		std::printf("trouble maker above?  %X: %X < %X\n", i, res1, res2);
	}
	else if (res1 > res2)
	{
		++below;
//		std::printf("trouble maker below?  %X: %X > %X\n", i, res1, res2);
	}
	else
	{
		++same;
	}
}

//void print_loop_count_results()
//{
//	for (auto [loops, count] : cxcm::relaxed::detail::loop_count_map)
//		std::printf("%d loops : %d\n", loops, count);
//}


void test_all_floats_sqrt()
{
	long long below{};
	long long above{};
	long long same{};
	test_sqrt_float(0xFFFFFFFF, above, below, same);
	for (unsigned int i = 0x00000000; i < 0xFFFFFFFF; ++i)
	{
		test_sqrt_float(i, above, below, same);
	}

//	print_loop_count_results();
	std::printf("\n");

	std::printf("same std : %lld\n", same);
	std::printf("below std : %lld\n", below);
	std::printf("above std : %lld\n", above);
}

// this function is a place to just test out whatever
void sandbox_function()
{
	// put fun code here

	[[ maybe_unused ]] auto a = cxcm::fabs(-3);

#if defined(__INTEL_LLVM_COMPILER)
	std::cout << "__INTEL_LLVM_COMPILER = " << __INTEL_LLVM_COMPILER << "\n";
	std::cout << "__VERSION__: " << __VERSION__ << "\n";
#endif


//	test_all_floats_sqrt();
}

int main(int argc, char *argv[])
{
	//
	// fun stuff
	//

	sandbox_function();

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
