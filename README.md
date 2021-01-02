# cxcm: constexpr cmath

cxcm is a c++20 library that provides constexpr versions of some of the functions in the ```<cmath>``` include file.

## Installation

Currently this is a single header library. All you need to do is include cxcm.hxx and the functions are in the ```cxcm``` namespace.

## Motivation

Originally I was working on a project dealing with periodic intervals, and I wanted to make the project as constexpr as I could. The most important functions for me from ```<cmath>``` were ```std::floor()``` and ```std::ceil()```. I looked for constexpr ```<cmath>``` projects, and discovered that none that I found matched my requirements.

[GCEM](https://github.com/kthohr/gcem) from Keith O'Hara is very popular, but it doesn't try and stick to the standard for smaller numbers (subnormals). I needed more fidelity to the Standard Library, so I started creating my own small project.

## Status

Current version: v0.1.2

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

## Testing

This project uses [doctest](https://github.com/onqtam/doctest) for testing, and we are primarily testing the conformance of ```trunc```, ```floor```, ```ceil```, and ```round``` with ```std::```. The tests have been run on:

* MSVC 2019 - v16.8 or higher
* gcc 9.2.0
* clang 10.0

All tests passed 100%. It could work on early versions, and certainly should work on later versions.

There are no specific tests for ```sqrt``` and ```rsqrt```. They don't produce the same results for ```double``` and presumably ```long double```, as the constexpr versions are off by at most 1 ulp (it appears). For testing the entire range of ```float```, they both conform 100% with ```std::```. For  ```double```, depending on the range being tested, exact conformance has been between 80% and 100%, but it would take 100,000 years to test every ```double```.

## License

This project uses the [Boost Software License 1.0](https://choosealicense.com/licenses/bsl-1.0/). It is not a part of Boost, this is just the best permissive license for my needs.