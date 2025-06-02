#ifndef _HINTS_H_
#define _HINTS_H_

#include <string>
#include <cstdint>

// returns true if hint condition is satisfied for the number, parametrized by arg
typedef bool (*CheckHintsFun)(uint64_t number, int arg);

bool is_multiple_of(uint64_t number, int arg);

bool is_square(uint64_t number, int /*arg*/);

bool product_of_digits_is(uint64_t number, int arg);

bool is_divisible_by_each_of_digits(uint64_t number, int /*arg*/);

bool is_odd_and_palindrome(uint64_t number, int /*arg*/);

bool is_fibonacci(uint64_t number, int /*arg*/);

bool is_prime(uint64_t number, int /*arg*/);

CheckHintsFun get_hints_fun(std::string const & name);

#endif // _HINTS_H_
