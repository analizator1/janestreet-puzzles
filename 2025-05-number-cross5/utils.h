#ifndef _UTILS_H_
#define _UTILS_H_

#include <algorithm>
#include <istream>
#include <memory>
#include <cassert>
#include <cstdint>

void skipComments(std::istream & inp);

/*
 * A simple dynamic_bitset that does not keep its size (in release config).
 */
class DynamicBitset
{
public:
	explicit DynamicBitset(int size_bits)
		: mem(new uint8_t[(size_bits + 7) / 8] {})
#ifndef NDEBUG
		, size_bits(size_bits)
#endif
	{
		assert(size_bits >= 0);
	}

	DynamicBitset(DynamicBitset const & other, int size_bits)
		: mem()
#ifndef NDEBUG
		, size_bits(size_bits)
#endif
	{
		assert(size_bits == other.size_bits);
		int const num_bytes = (size_bits + 7) / 8;
		mem.reset(new uint8_t[num_bytes]);
		std::copy(&other.mem[0], &other.mem[num_bytes], &this->mem[0]);
	}

	bool get_bit(int k) const
	{
		assert(k >= 0 && k < size_bits);
		return mem[k / 8] & (1 << k % 8);
	}

	void set_bit(int k)
	{
		assert(k >= 0 && k < size_bits);
		mem[k / 8] |= (1 << k % 8);
	}

	void clear_bit(int k)
	{
		assert(k >= 0 && k < size_bits);
		mem[k / 8] &= ~(1 << k % 8);
	}

	bool is_equal(DynamicBitset const & other, int size_bits) const
	{
		assert(size_bits == this->size_bits);
		assert(size_bits == other.size_bits);
		int const num_bytes = (size_bits + 7) / 8;
		return std::equal(&mem[0], &mem[num_bytes], &other.mem[0]);
	}

private:
	std::unique_ptr<uint8_t[]> mem;

#ifndef NDEBUG
	int size_bits;
#endif
};

#endif // _UTILS_H_
