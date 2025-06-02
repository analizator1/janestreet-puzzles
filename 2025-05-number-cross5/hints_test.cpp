#include "hints.h"

#include <cassert>
#include <iostream>

void test_is_multiple_of()
{
	std::cout << __func__ << "()\n";
	assert(is_multiple_of(99225, 2025));
	assert(!is_multiple_of(99226, 2025));
}

// It tests is_square() for all integers in range [min_n**2; (max_n+1)**2-1].
void test_is_square(uint32_t min_n, uint32_t max_n)
{
	std::cout << "testing is_square() from " << (uint64_t)min_n * min_n
		<< " to " << (uint64_t)(max_n+1) * (max_n+1)-1 << "...\n";
	for (uint32_t i = min_n; i <= max_n; ++i)
	{
		// Test numbers in range [i**2, (i+1)**2-1]
		uint64_t c = (uint64_t)i * i;
		if (!is_square(c, -1))
		{
			std::cerr << "Error: expected is_square(" << c << ") to be true\n";
		}
		for (++c; c < (uint64_t)(i+1) * (i+1); ++c)
		{
			if (is_square(c, -1))
			{
				std::cerr << "Error: expected is_square(" << c << ") to be false\n";
			}
		}
	}
	std::cout << "done testing is_square()\n";
}

void test_product_of_digits_is()
{
	std::cout << __func__ << "()\n";
	assert(product_of_digits_is(99225, 9 * 9 * 2 * 2 * 5));
	assert(!product_of_digits_is(99226, 9 * 9 * 2 * 2 * 5));
	assert(product_of_digits_is(99226, 9 * 9 * 2 * 2 * 6));
	assert(product_of_digits_is(55, 25));
	assert(!product_of_digits_is(55, 24));
}

void test_is_divisible_by_each_of_digits()
{
	std::cout << __func__ << "()\n";
	assert(is_divisible_by_each_of_digits(24, -1));
	assert(!is_divisible_by_each_of_digits(25, -1));
	assert(!is_divisible_by_each_of_digits(10, -1));
	assert(is_divisible_by_each_of_digits(128, -1));
	assert(!is_divisible_by_each_of_digits(256, -1));
}

void test_is_odd_and_palindrome()
{
	std::cout << __func__ << "()\n";
	assert(is_odd_and_palindrome(3, -1));
	assert(!is_odd_and_palindrome(2, -1));
	assert(!is_odd_and_palindrome(4, -1));
	assert(is_odd_and_palindrome(12321, -1));
	assert(is_odd_and_palindrome(124421, -1));
	assert(!is_odd_and_palindrome(324421, -1));
	assert(!is_odd_and_palindrome(125421, -1));
	assert(!is_odd_and_palindrome(124431, -1));
}

void test_is_fibonacci()
{
	std::cout << __func__ << "()\n";
	assert(is_fibonacci(1, -1));
	assert(is_fibonacci(2, -1));
	assert(is_fibonacci(3, -1));
	assert(!is_fibonacci(4, -1));
	assert(is_fibonacci(5, -1));
	assert(!is_fibonacci(376, -1));
	assert(is_fibonacci(377, -1));
	assert(!is_fibonacci(378, -1));
	assert(!is_fibonacci(4180, -1));
	assert(is_fibonacci(4181, -1));
	assert(!is_fibonacci(4182, -1));
}

void test_is_prime()
{
	uint64_t const start = 54'973'215'091ull;
	uint64_t const last = start + 100;
	std::cout << "List of primes in range [" << start << "; " << last << "]:\n";
	for (uint64_t p = start; p <= last; ++p)
	{
		if (is_prime(p, -1))
			std::cout << p << '\n';
	}
	std::cout << "End of list\n";
}

int main()
{
	test_is_multiple_of();
	test_is_square(1'000'000 - 100, 1'000'000);
	test_product_of_digits_is();
	test_is_divisible_by_each_of_digits();
	test_is_odd_and_palindrome();
	test_is_fibonacci();
	test_is_prime();
}
