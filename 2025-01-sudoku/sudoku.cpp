#include <iostream>
#include <cstdint>
#include <cassert>
#include <set>
#include <array>
#include <algorithm>

int8_t grid[9][9];

void print_row_separator()
{
	std::cout << "  +";
	for (int col = 0; col < 9; ++col)
	{
		std::cout << "---+";
	}
	std::cout << '\n';
}

void print_grid()
{
	std::cout << "   ";
	for (int col = 0; col < 9; ++col)
	{
		std::cout << ' ' << char('0' + col) << "  ";
	}
	std::cout << '\n';
	print_row_separator();
	for (int row = 0; row < 9; ++row)
	{
		std::cout << char('0' + row) << " |";
		for (int col = 0; col < 9; ++col)
		{
			int8_t elem = grid[row][col];
			std::cout << ' ' << (elem == -1 ? ' ' : char('0' + elem)) << " |";
		}
		std::cout << '\n';
		print_row_separator();
	}
}

int8_t unused_digit;

unsigned int best_middle_row;
unsigned int best_gcd;
unsigned long long num_solutions;

void process_solution()
{
	++num_solutions;
	if (num_solutions % 100000 == 0)
	{
		std::cout << "solutions considered so far: " << num_solutions << std::endl;
		std::cout << "last found solution:\n";
		std::cout << "unused digit: " << char('0' + unused_digit) << '\n';
		print_grid();
	}
	if (num_solutions >= 10000000)
	{
		std::cout << "too many solutions considered: " << num_solutions << std::endl;
		std::exit(0);
	}
}

void rec_search()
{
	// Find unfilled cell with the lowest number of digits that can be filled in.
	// If found such cell: iterate over possible digits: fill a digit in and call us recursively.
	// If no such cell: we have a solution. Compute GCD and if it's the highest so far then update best_middle_row.
	int best_num_available_digits = 10;
	std::array<bool, 10> best_available_digits;
	int best_row, best_col;

	for (int cur_row = 0; cur_row < 9; ++cur_row)
	{
		for (int cur_col = 0; cur_col < 9; ++cur_col)
		{
			if (grid[cur_row][cur_col] == -1)
			{
				// process unfilled cell
				std::array<bool, 10> cur_available_digits;
				std::fill(cur_available_digits.begin(), cur_available_digits.end(), true);
				cur_available_digits[unused_digit] = false;
				// iterate over cells in cur_row
				for (int col = 0; col < 9; ++col)
				{
					int8_t elem = grid[cur_row][col];
					if (elem != -1)
						cur_available_digits[elem] = false;
				}
				// iterate over cells in cur_col
				for (int row = 0; row < 9; ++row)
				{
					int8_t elem = grid[row][cur_col];
					if (elem != -1)
						cur_available_digits[elem] = false;
				}
				// iterate over cells in current box
				int box_row = cur_row - cur_row % 3;
				int box_col = cur_col - cur_col % 3;
				for (int row = box_row; row < box_row + 3; ++row)
				{
					for (int col = box_col; col < box_col + 3; ++col)
					{
						int8_t elem = grid[row][col];
						if (elem != -1)
							cur_available_digits[elem] = false;
					}
				}
				// count number of available digits
				int cur_num_available_digits = 0;
				for (bool avail : cur_available_digits)
					cur_num_available_digits += avail;
				assert(cur_num_available_digits < 10);
				// update best
				if (cur_num_available_digits < best_num_available_digits)
				{
					best_num_available_digits = cur_num_available_digits;
					best_available_digits = cur_available_digits;
					best_row = cur_row;
					best_col = cur_col;
				}
			}
		}
	}

	if (best_num_available_digits == 10)
	{
		process_solution();
	}
	else
	{
		// recursively try available digits
		int8_t & elem = grid[best_row][best_col];
		assert(elem == -1);
		for (int8_t digit = 0; digit < 10; ++digit)
		{
			if (best_available_digits[digit])
			{
				// digit is available
				elem = digit;
				rec_search();
				elem = -1;
			}
		}
	}
}

void search_for_solutions()
{
	std::cout << "search_for_solutions called with unused_digit: " << char('0' + unused_digit) << std::endl;
	rec_search();
}

int main()
{
	for (int row = 0; row < 9; ++row)
	{
		for (int col = 0; col < 9; ++col)
		{
			grid[row][col] = -1;
		}
	}

	grid[0][7] = 2;
	grid[1][8] = 5;
	grid[2][1] = 2;
	grid[3][2] = 0;
	grid[5][3] = 2;
	grid[6][4] = 0;
	grid[7][5] = 2;
	grid[8][6] = 5;

	std::set<int8_t> used;
	for (int row = 0; row < 9; ++row)
	{
		for (int col = 0; col < 9; ++col)
		{
			int8_t elem = grid[row][col];
			if (elem != -1)
				used.insert(elem);
		}
	}

	for (int8_t digit = 0; digit < 10; ++digit)
	{
		if (used.find(digit) == used.end())
		{
			unused_digit = digit;
			search_for_solutions();
		}
	}

	std::cout << "Done. Number of solutions considered: " << num_solutions << '\n';

	if (best_middle_row)
	{
		std::cout << "best middle row: " << best_middle_row << '\n';
		std::cout << "best bcd: " << best_gcd << '\n';
	}
	else
	{
		std::cout << "no solutions found\n";
	}
}
