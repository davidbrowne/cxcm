
//          Copyright David Browne 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>

#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"

int main(int argc, char *argv[])
{
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


//#include <limits>   
//
//namespace Detail
//{
//	double constexpr sqrtNewtonRaphson(double x, double curr, double prev)
//	{
//		return curr == prev
//			? curr
//			: sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
//	}
//}
//
///*
//* Constexpr version of the square root
//* Return value:
//*   - For a finite and non-negative value of "x", returns an approximation for the square root of "x"
//*   - Otherwise, returns NaN
//*/
//double constexpr sqrt(double x)
//{
//	return x >= 0 && x < std::numeric_limits<double>::infinity()
//		? Detail::sqrtNewtonRaphson(x, x, 0)
//		: std::numeric_limits<double>::quiet_NaN();
//}