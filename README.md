# cxcm: constexpr cmath

cxcm is a c++20 library that provides ```constexpr``` versions of some of the functions in the ```<cmath>``` include file.

## Installation

Currently this is a single header library. All you need to do is include [cxcm.hxx](https://raw.githubusercontent.com/davidbrowne/cxcm/main/cxcm.hxx). The functions are in the ```cxcm``` namespace.

There are asserts in the codebase that can be disabled by defining the macro ```CXCM_DISABLE_ASSERTS```.

## Motivation

Originally I was working on a project dealing with periodic intervals, and I wanted to make the project as constexpr as I could. The most important functions for me from ```<cmath>``` were ```std::floor()``` and ```std::ceil()```. I looked for constexpr ```<cmath>``` projects, and discovered that none that I found matched my requirements.

[GCEM](https://github.com/kthohr/gcem) seems to be very popular, but at the time I started cxcm, GCEM wasn't conforming to the standard for smaller numbers (subnormals). I needed more fidelity to the Standard Library, so I started creating my own small project.

## Library cxcm
[cxcm](https://github.com/davidbrowne/cxcm) is its own stand-alone project for extending ```<cmath>``` to have more functions be constexpr, targeted for ```c++20```. ```c++23``` and hopefully ```c++26``` extend the amount of functions in ```<cmath>``` to be constexpr, but this project aims to support ```c++20```.

There are a couple of functions where the constexpr versions are not direct replacements for the standard library versions. constexpr ```cxcm::sqrt()``` should be a 100% match to ```std::sqrt()``` when the type is ```float```, but when the type is ```double``` we get ~75.6% the same results, with the remaining results only 1 ulp away from the answer given by the standard library. Similarly, for the function constexpr ```cxcm::rsqrt()```, which is not in ```<cmath>``` but is the same as ```1 / sqrt()```, there is a 100% match for ```float``` and ```1.0f / std::sqrt(float_value)```. For ```double```, we get ~85.7% the same results as ```1.0 / std::sqrt(double_value)```, with the remaining results either within 1 ulp (~12.4%) or 2 ulps (~1.9%).

## cxcm Free Functions

Most of these constexpr functions have counterparts in [```<cmath>```](https://en.cppreference.com/w/cpp/header/cmath). Many of these functions became constexpr in ```c++23``` or ```c++26```; however, this is a ```c++20``` library, so we are stuck with doing it ourselves if we want it.

* [```cxcm::isnan```](https://en.cppreference.com/w/cpp/numeric/math/isnan)
```c++
template <std::floating_point T>
constexpr bool isnan(T value) noexcept;
```
* [```cxcm::isinf```](https://en.cppreference.com/w/cpp/numeric/math/isinf)
```c++
template <std::floating_point T>
constexpr bool isinf(T value) noexcept;
```
* [```cxcm::fpclassify```](https://en.cppreference.com/w/cpp/numeric/math/fpclassify)
```c++
template <std::floating_point T>
constexpr int fpclassify(T value) noexcept;
```
* [```cxcm::isnormal```](https://en.cppreference.com/w/cpp/numeric/math/isnormal)
```c++
template <std::floating_point T>
constexpr bool isnormal(T value) noexcept;
```
* [```cxcm::isfinite```](https://en.cppreference.com/w/cpp/numeric/math/isfinite)
```c++
template <std::floating_point T>
constexpr bool isfinite(T value) noexcept;
```
* [```cxcm::signbit```](https://en.cppreference.com/w/cpp/numeric/math/signbit)
```c++
template <std::floating_point T>
constexpr bool signbit(T value) noexcept;
```
* [```cxcm::copysign```](https://en.cppreference.com/w/cpp/numeric/math/copysign)
```c++
template <std::floating_point T>
constexpr T copysign(T value, T sgn) noexcept;
```
* [```cxcm::abs```](https://en.cppreference.com/w/cpp/numeric/math/fabs) - see [here](https://en.cppreference.com/w/cpp/numeric/math/abs) for integral ```abs```
```c++
template <std::floating_point T>
constexpr T abs(T value) noexcept;

template <std::integral T>
constexpr double abs(T value) noexcept;
```
* [```cxcm::fabs```](https://en.cppreference.com/w/cpp/numeric/math/fabs)
```c++
template <std::floating_point T>
constexpr T fabs(T value) noexcept;

template <std::integral T>
constexpr double fabs(T value) noexcept;
```
* [```cxcm::fmod```](https://en.cppreference.com/w/cpp/numeric/math/fmod) -  has efficient runtime use
```c++
template <std::floating_point T>
constexpr T fmod(T x, T y) noexcept;
```
* [```cxcm::trunc```](https://en.cppreference.com/w/cpp/numeric/math/trunc) -  has efficient runtime use
```c++
template <std::floating_point T>
constexpr T trunc(T value) noexcept;

template <std::integral T>
constexpr double trunc(T value) noexcept;
```
* [```cxcm::floor```](https://en.cppreference.com/w/cpp/numeric/math/floor) -  has efficient runtime use
```c++
template <std::floating_point T>
constexpr T floor(T value) noexcept;

template <std::integral T>
constexpr double floor(T value) noexcept;
```
* [```cxcm::ceil```](https://en.cppreference.com/w/cpp/numeric/math/ceil) -  has efficient runtime use
```c++
template <std::floating_point T>
constexpr T ceil(T value) noexcept;

template <std::integral T>
constexpr double ceil(T value) noexcept;
```
* [```cxcm::round```](https://en.cppreference.com/w/cpp/numeric/math/round) -  has efficient runtime use
```c++
template <std::floating_point T>
constexpr T round(T value) noexcept;

template <std::integral T>
constexpr double round(T value) noexcept;
```
* ```cxcm::round_even``` - not in ```<cmath>```
```c++
template <std::floating_point T>
constexpr T round_even(T value) noexcept;

template <std::integral T>
constexpr double round_even(T value) noexcept;
```
Returns a number equal to the nearest integer to value. A fractional part of 0.5 will round toward the nearest even integer.
(Both 3.5 and 4.5 for value will return 4.0.)
* ```cxcm::fract``` - not in ```<cmath>```
```c++
template <std::floating_point T>
constexpr T fract(T value) noexcept;

template <std::integral T>
constexpr double fract(T /* value */) noexcept;
```
Returns ```value - floor(value)```. The positive fractional part of a floating-point number. This is a number in the range [0.0, 1.0).
* [```cxcm::sqrt```](https://en.cppreference.com/w/cpp/numeric/math/sqrt) -  has efficient runtime use
```c++
template <std::floating_point T>
constexpr T sqrt(T value) noexcept;
```
* ```cxcm::rsqrt``` -  reciprocal square root - has efficient runtime use - not in ```<cmath>```
```c++
template <std::floating_point T>
constexpr T rsqrt(T value) noexcept;
```
Returns ```1.0 / sqrt(value)```. Results are undefined if ```value <= 0```.

## Status

Current version: `v0.9.1`

Not sure yet how much more to try and make ```constexpr```. This library is meant to support the needs of other libraries, so I suppose things will be added as needed.

## Usage

The point of this library is to provide ```constexpr``` versions of certain functions. This is helpful for compile time programming, but we don't usually want to run the ```constexpr``` versions of the functions at runtime. If we discover that we are using these functions at runtime, we revert to the ```std::``` versions for:

* ```trunc(std::floating_point)```
* ```floor(std::floating_point)```
* ```ceil(std::floating_point)```
* ```round(std::floating_point)```
* ```sqrt()```
* ```rsqrt()```
* ```fmod()```

Be aware that ```cxcm::sqrt()``` and ```cxcm::rsqrt()``` do not always have identical results with ```std::sqrt()```, but they are the same more often than not. If there is a difference, it is at most 2 ulps away from the standard library version.

## Testing

This project uses [doctest](https://github.com/onqtam/doctest) for testing, and we are primarily testing the conformance of ```trunc```, ```floor```, ```ceil```, and ```round``` with ```std::```. The tests have been run on:

* MSVC 2022 - v17.7.4
* gcc 13.1.0
* clang 16.0.6

```
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  33 |  33 passed | 0 failed | 0 skipped
[doctest] assertions: 746 | 746 passed | 0 failed |
[doctest] Status: SUCCESS!
```

It might work on earlier versions, and it certainly should work on later versions.

There are no specific tests for ```sqrt()``` and ```rsqrt()```. They don't produce the same results for ```double``` and presumably ```long double```, as the constexpr versions are off by at most 1 ulp (it appears). For testing the entire range of ```float```, they both conform 100% with ```std::```. For  ```double```, depending on the range being tested, exact conformance has been between 70% and 100%, but it would take 100,000 years to test every ```double```.

We are also missing tests for ```fmod()```, ```fract()```, and ```round_even()```.

## License [![BSL](https://img.shields.io/badge/license-BSL-blue)](https://choosealicense.com/licenses/bsl-1.0/)

This project uses the [Boost Software License 1.0](https://choosealicense.com/licenses/bsl-1.0/).