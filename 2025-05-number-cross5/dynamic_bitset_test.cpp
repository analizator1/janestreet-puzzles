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

		for (int k = 0; k < n; k += skip)
			b.clear_bit(k);

		for (int k = 0; k < n; ++k)
			assert(b.get_bit(k) == false);

		for (int k = 0; k < n; ++k)
			b.set_bit(k);

		for (int k = 0; k < n; ++k)
			assert(b.get_bit(k) == true);
	}
}

int main()
{
	test_dynamic_bitset();
}
