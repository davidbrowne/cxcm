# cxcm: constexpr cmath

cxcm is a c++20 library that provides ```constexpr``` versions of some of the functions in the ```<cmath>``` include file.

## Installation

Currently this is a single header library. All you need to do is include [cxcm.hxx](https://raw.githubusercontent.com/davidbrowne/cxcm/main/cxcm.hxx). The functions are in the ```cxcm``` namespace.

## Motivation

Originally I was working on a project dealing with periodic intervals, and I wanted to make the project as constexpr as I could. The most important functions for me from ```<cmath>``` were ```std::floor()``` and ```std::ceil()```. I looked for constexpr ```<cmath>``` projects, and discovered that none that I found matched my requirements.

[GCEM](https://github.com/kthohr/gcem) is very popular, but it doesn't try and stick to the standard for smaller numbers (subnormals). I needed more fidelity to the Standard Library, so I started creating my own small project.

## Status

Current version: `v0.1.3`

Implemented:

* ```abs```
* ```fabs```
* ```fpclassify```
* ```isfinite```
* ```isinf```
* ```isnan```
* ```isnormal```
* ```trunc```
* ```floor```
* ```ceil```
* ```round```
* ```sqrt``` : great approximation but not all exact with ```std::``` - at most seems off by only 1 ulp
* ```rsqrt``` : reciprocal sqrt - not part of ```<cmath>``` or the standard, but very helpful - same compatibility characteristics as ```sqrt```, but for ```1.0 / sqrt()```

Not sure yet how much more to try and make ```constexpr```. This library is meant to support the needs of other libraries, so I suppose things will be added as needed.

## Usage

The point of this library is to provide ```constexpr``` versions of certain functions. This is helpful for compile time programming, but we don't usually want to run the ```constexpr``` versions of the functions at runtime. If we discover that we are using these functions at runtime, we revert to the ```std::``` versions for:

* ```trunc```
* ```floor```
* ```ceil```
* ```round```
* ```sqrt```
* ```rsqrt```

To get rid of runtime optimization and always use the constexpr version, then define ```CXCM_DISABLE_RUNTIME_OPTIMIZATIONS``` before you include cxcm.hxx:

``` c++
#define CXCM_DISABLE_RUNTIME_OPTIMIZATIONS
#include "cxcm.hxx"

// thanks to CXCM_DISABLE_RUNTIME_OPTIMIZATIONS, no runtime optimizations for cxcm library.
// this will not call std::sqrt() as a runtime optimization.
// this is clearly not a constexpr context.
double length(double a, double b, double c)
{
	return cxcm::sqrt(a*a + b*b + c*c);
}

```

## Testing

This project uses [doctest](https://github.com/onqtam/doctest) for testing, and we are primarily testing the conformance of ```trunc```, ```floor```, ```ceil```, and ```round``` with ```std::```. The tests have been run on:

* MSVC 2019 - v16.8 or higher
* gcc 9.2.0
* clang 10.0

```
===============================================================================
[doctest] test cases:  24 |  24 passed | 0 failed | 0 skipped
[doctest] assertions: 672 | 672 passed | 0 failed |
[doctest] Status: SUCCESS!
```

It might work on earlier versions, and it certainly should work on later versions.

There are no specific tests for ```sqrt``` and ```rsqrt```. They don't produce the same results for ```double``` and presumably ```long double```, as the constexpr versions are off by at most 1 ulp (it appears). For testing the entire range of ```float```, they both conform 100% with ```std::```. For  ```double```, depending on the range being tested, exact conformance has been between 70% and 100%, but it would take 100,000 years to test every ```double```.

## License [![BSL](https://img.shields.io/badge/license-BSL-blue)](https://choosealicense.com/licenses/bsl-1.0/)

This project uses the [Boost Software License 1.0](https://choosealicense.com/licenses/bsl-1.0/).