#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <numeric>
#include <set>

void print_row_separator()
{
	std::cout << "  +";
	for (int col = 0; col < 9; ++col)
	{
		std::cout << "---+";
	}
	std::cout << '\n';
}

void print_grid(int8_t (&grid)[9][9])
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

int8_t grid[9][9];

std::pair<unsigned int, unsigned int> compute_grid_gcd_and_middle_row()
{
	unsigned int gcd_val = 0; // neutral element for gcd
	unsigned int middle_row = -1;
	for (int row = 0; row < 9; ++row)
	{
		unsigned int row_val = 0;
		for (int col = 0; col < 9; ++col)
		{
			int8_t elem = grid[row][col];
			if (elem == -1)
			{
				// row now filled yet, ignore it
				row_val = -1;
				break;
			}
			row_val = row_val * 10 + elem;
		}
		if (row_val != (unsigned int)-1)
		{
			gcd_val = std::gcd(gcd_val, row_val);
			if (row == 4)
				middle_row = row_val;
		}
	}
	return {gcd_val, middle_row};
}

int8_t cur_unused_digit;

unsigned int best_middle_row;
unsigned int best_gcd;
int8_t best_grid[9][9];
int8_t best_unused_digit;

void print_best_solution()
{
	std::cout << '\n';
	std::cout << "=== best solution ===\n";

	if (!best_middle_row)
	{
		std::cout << "no solutions found\n";
		return;
	}

	std::cout << "best middle row: " << best_middle_row << '\n';
	std::cout << "best gcd: " << best_gcd << '\n';
	std::cout << "best unused digit: " << char('0' + best_unused_digit) << '\n';
	print_grid(best_grid);
}

void process_solution()
{
	// update best solution vars
	auto const [cur_gcd, cur_middle_row] = compute_grid_gcd_and_middle_row();
	if (cur_gcd > best_gcd)
	{
		best_middle_row = cur_middle_row;
		best_gcd = cur_gcd;
		std::memcpy(best_grid, grid, sizeof(best_grid));
		best_unused_digit = cur_unused_digit;

		print_best_solution();
		std::cout.flush();
	}
}

struct BestCell
{
	int num_available_digits;
	std::bitset<10> available_digits;
	int row;
	int col;
};

BestCell find_best_cell(int num_available_digits_threshold, int search_row_hint)
{
	BestCell best_cell;
	best_cell.num_available_digits = 10;

	// Prepare row indices order: search_row_hint should be first.
	std::array<int, 9> row_indices;
	int *row_idx_ptr = row_indices.data();
	*row_idx_ptr++ = search_row_hint;
	// Add remaining row indices.
	for (int row = 0; row < 9; ++row)
	{
		if (row != search_row_hint)
			*row_idx_ptr++ = row;
	}

	// Find unfilled cell with the lowest number of digits that can be filled in.
	// If found such cell: iterate over possible digits: fill a digit in and call us recursively.
	// If no such cell: we have a solution. Compute GCD and if it's the highest so far then update best_middle_row.
	for (int cur_row : row_indices)
	{
		for (int cur_col = 0; cur_col < 9; ++cur_col)
		{
			if (grid[cur_row][cur_col] == -1)
			{
				// process unfilled cell
				std::bitset<10> cur_available_digits;
				cur_available_digits.set();
				cur_available_digits[cur_unused_digit] = false;

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

				int const cur_num_available_digits = cur_available_digits.count();
				assert(cur_num_available_digits < 10);

				if (cur_num_available_digits < best_cell.num_available_digits)
				{
					best_cell.num_available_digits = cur_num_available_digits;
					best_cell.available_digits = cur_available_digits;
					best_cell.row = cur_row;
					best_cell.col = cur_col;

					// Don't consider any more cells if we have at most one less possibilities than our caller.
					if (best_cell.num_available_digits <= num_available_digits_threshold)
					{
						return best_cell;
					}
				}
			}
		}
	}

	return best_cell;
}

constexpr unsigned int progress_max_level = 12;

struct Progress
{
	int done;
	int total;
};

Progress progress_at_level[progress_max_level + 1];

std::chrono::steady_clock::time_point start_time;

void print_progress(unsigned int const rec_search_level)
{
	unsigned int const max_valid_level = std::min(rec_search_level, progress_max_level);

	double progress = 0;
	double level_weight = 1;
	for (unsigned int i = 0; i <= max_valid_level; ++i)
	{
		level_weight /= progress_at_level[i].total;
		progress += level_weight * progress_at_level[i].done;
	}
	std::cout << "total progress: " << progress;

	std::cout << " recursion:";
	for (unsigned int i = 0; i <= max_valid_level; ++i)
	{
		std::cout << " L" << i << ": " << progress_at_level[i].done << "/" << progress_at_level[i].total;
	}
	std::cout << '\n';

	auto const now = std::chrono::steady_clock::now();
	std::chrono::duration<double> const elapsed = now - start_time;
	std::chrono::duration<double> const est_duration = elapsed / progress;
	std::chrono::duration<double> const time_left = est_duration - elapsed;
	std::cout << "time elapsed: " << elapsed.count() << "s left: " << time_left.count()
		<< "s (" << time_left.count() / 3600 << "h)\n";
}

void rec_search(unsigned int const rec_search_level, int const num_available_digits_threshold, int const search_row_hint)
{
	// Levels up to min(rec_search_level, progress_max_level) are considered valid.
	if (rec_search_level <= progress_max_level)
		progress_at_level[rec_search_level] = {0, 1};

	// Check if existing filled rows already make GCD not higher than the best one.
	auto const [cur_gcd, cur_middle_row] = compute_grid_gcd_and_middle_row();
	if (cur_gcd && cur_gcd <= best_gcd)
	{
		// There are some filled rows, GCD is not good and it won't get any better with these rows.
		// Prune this search branch.
		return;
	}

	BestCell const best_cell = find_best_cell(num_available_digits_threshold, search_row_hint);

	if (best_cell.num_available_digits == 10)
	{
		// Each cell can have at most 9 available digits, so all cells are filled.
		process_solution();
	}
	else
	{
		if (rec_search_level <= progress_max_level)
		{
			progress_at_level[rec_search_level].total = best_cell.num_available_digits;

			std::cout << '\n';
			std::cout << "considering " << best_cell.num_available_digits
				<< " digits for cell (" << best_cell.row << ", " << best_cell.col
				<< ") at recursion level " << rec_search_level << '\n';
			print_progress(rec_search_level);

			static unsigned int num_progress_prints;
			if (++num_progress_prints % 10 == 0)
			{
				// Print extended info about algorithm progress.
				print_best_solution();

				std::cout << '\n';
				std::cout << "=== current grid ===\n";
				print_grid(grid);
			}

			std::cout.flush();
		}

		// recursively try available digits
		int8_t & elem = grid[best_cell.row][best_cell.col];
		assert(elem == -1);
		for (int8_t digit = 0; digit < 10; ++digit)
		{
			if (best_cell.available_digits[digit])
			{
				// digit is available
				elem = digit;
				rec_search(rec_search_level + 1, best_cell.num_available_digits - 1, best_cell.row);
				elem = -1;

				if (rec_search_level <= progress_max_level)
					progress_at_level[rec_search_level].done++;
			}
		}
	}
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

	int const num_unused = 10 - used.size();
	progress_at_level[0] = {0, num_unused};
	start_time = std::chrono::steady_clock::now();
	for (int8_t digit = 0; digit < 10; ++digit)
	{
		if (used.find(digit) == used.end())
		{
			cur_unused_digit = digit;
			std::cout << "considering unused digit: " << char('0' + cur_unused_digit) << '\n';
			std::cout.flush();
			rec_search(1, 0, 0);
			progress_at_level[0].done++;
		}
	}

	print_best_solution();
}
