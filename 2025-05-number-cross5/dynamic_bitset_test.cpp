#include <cassert>
#include <iostream>
#include <stdexcept>

#include "utils.h"

void test_dynamic_bitset()
{
#ifdef NDEBUG
	throw std::runtime_error("expected debug config");
#endif
	std::cout << "Testing DynamicBitset\n";
	for (int skip : {1, 2, 3, 7})
	{
		int n = 100;
		DynamicBitset b(n);

		for (int k = 0; k < n; ++k)
			assert(b.get_bit(k) == false);

		for (int k = 0; k < n; k += skip)
		{
			assert(b.get_bit(k) == false);
			b.set_bit(k);
			assert(b.get_bit(k) == true);
		}

		for (int k = 0; k < n; ++k)
		{
			if (k % skip == 0)
				assert(b.get_bit(k) == true);
			else
				assert(b.get_bit(k) == false);
		}

		DynamicBitset b_copy(b, n);
		for (int k = 0; k < n; ++k)
		{
			if (k % skip == 0)
				assert(b_copy.get_bit(k) == true);
			else
				assert(b_copy.get_bit(k) == false);
		}

		for (int k = 0; k < n; k += skip)
			b.clear_bit(k);

		for (int k = 0; k < n; ++k)
			assert(b.get_bit(k) == false);

		for (int k = 0; k < n; ++k)
			b.set_bit(k);

		for (int k = 0; k < n; ++k)
			assert(b.get_bit(k) == true);

		DynamicBitset b2(n);
		assert(!b.is_equal(b2, n));
		for (int k = 0; k < n; ++k)
			b2.set_bit(k);
		assert(b.is_equal(b2, n));

		b2.clear_bit(n - 1);
		assert(!b.is_equal(b2, n));
		b.clear_bit(n - 1);
		assert(b.is_equal(b2, n));

		b2.clear_bit(0);
		assert(!b.is_equal(b2, n));
		b.clear_bit(0);
		assert(b.is_equal(b2, n));
	}
}

int main()
{
	test_dynamic_bitset();
}
