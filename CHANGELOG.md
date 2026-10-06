# Change Log

### v1.4.2
* Used Claude to make fast_rsqrt() more accurate.
* Added correct_rsqrt_rounding() to help with rounding issues in various inverse square root functions.
* Added static_assert() for checking if two different implementations of inverse square root functions are returning the same value for a given input.
* Added static_assert() for checking some boundaries cases for rsqrt().

### v1.4.1
Used Claude to help get exact versions of inverse square root, and helped clean up fast_rsqrt().

### v1.4.0
Big Claude analysis and refactor, fixing bugs and accuracy and compatibility with std::sqrt(), std::fmod, etc., and there are lots of Claude created tests (but not doctest-based). Tests show that using compiler flags for "fast math" is not supported by this library, as functions will not return accurate values.

### v1.3.2
Reverted changes to various static_cast uses from v1.3.1 related to bit-based types, e.g., changing int32_t back to int.

### v1.3.1
Minor refactoring.

### v1.3.0
Breaking change to internal namespaces: the **detail** namespace has been renamed to **internal** to better reflect its purpose and to avoid confusion with other libraries that may also use a detail namespace. Should not have an affect on users unless they are poking around under the sheets.

### v1.2.9
* Created this CHANGELOG.md file to keep track of changes in the library.
* Updated the relaxed::detail implementations for converging_sqrt(), converging_inverse_sqrt(), and inverse_sqrt() functions.
* Added some supporting functions to dd_real class.
