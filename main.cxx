
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
