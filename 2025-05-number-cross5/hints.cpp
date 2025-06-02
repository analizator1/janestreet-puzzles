#include "hints.h"

#include <cmath>

bool is_multiple_of(uint64_t const number, int arg)
{
	return number % arg == 0;
}

bool is_square(uint64_t const number, int /*arg*/)
{
	uint32_t const maybe_root = (uint32_t)std::sqrt(double(number));
	return (uint64_t)maybe_root * maybe_root == number;
}

bool product_of_digits_is(uint64_t const number, int arg)
{
	uint64_t product = 1;
	uint64_t num = number;
	while (num)
	{
		uint32_t digit = num % 10;
		num = num / 10;

		product *= digit;
	}
	return product == (uint32_t)arg;
}

bool is_divisible_by_each_of_digits(uint64_t const number, int /*arg*/)
{
	uint64_t num = number;
	while (num)
	{
		uint32_t digit = num % 10;
		num = num / 10;

		if (digit == 0 || number % digit != 0)
			return false;
	}
	return true;
}

bool is_odd_and_palindrome(uint64_t const number, int /*arg*/)
{
	if (number % 2 == 0)
		return false;
	std::string str = std::to_string(number);
	int const len = (int)str.size();
	for (int i = 0; i < len / 2; ++i)
	{
		if (str[i] != str[len - 1 - i])
			return false;
	}
	return true;
}

bool is_fibonacci(uint64_t const number, int /*arg*/)
{
	uint64_t a = 0;
	uint64_t b = 1;
	while (b <= number)
	{
		if (b == number)
			return true;
		uint64_t tmp = a;
		a = b;
		b = tmp + b;
	}
	return false;
}

bool is_prime(uint64_t const number, int /*arg*/)
{
	uint32_t i = 2;
	while ((uint64_t)i * i <= number)
	{
		if (number % i == 0)
			return false;
		++i;
	}
	return true;
}

CheckHintsFun get_hints_fun(std::string const & name)
{
	if (name == "multiple")
		return is_multiple_of;
	if (name == "square")
		return is_square;
	if (name == "product_of_digits")
		return product_of_digits_is;
	if (name == "divisible_by_each_of_digits")
		return is_divisible_by_each_of_digits;
	if (name == "odd_and_palindrome")
		return is_odd_and_palindrome;
	if (name == "fibonacci")
		return is_fibonacci;
	if (name == "prime")
		return is_prime;

	return nullptr;
}
