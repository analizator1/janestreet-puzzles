#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "utils.h"

struct Pos
{
	int row;
	int col;
};

Pos operator+(Pos const & a, Pos const & b)
{
	return {a.row + b.row, a.col + b.col};
}

Pos operator*(Pos const & self, int factor)
{
	return {self.row * factor, self.col * factor};
}

std::ostream & operator<<(std::ostream & out, Pos const & self)
{
	return out << '{' << self.row << ", " << self.col << '}';
}

namespace { // anonymous namespace
Pos const vec_up    = {-1, 0};
Pos const vec_right = {0, 1};
Pos const vec_down  = {1, 0};
Pos const vec_left  = {0, -1};

Pos const all_directions[4] = {vec_up, vec_right, vec_down, vec_left};
} // anonymous namespace


/*
 * Solution draft:
 * void solve_row(int current_row)
 * {
 */
	/* for (state in: rows in [current_row-1; current_row+1] have: for each cell, the region of that cell has a number in [1; 9] assigned)
	 * {
	 */

	/* for (state in: rows in [current_row-1; current_row+1] have: tiles in the row placed such that for each tile there is a
	 * non-empty set 'cells_for_displacement')
	 * {
	 * Note that when recursing over tiles assignment, it may happen that a row already has a tile assignment (from
	 * a different current_row).
	 */

	/* for (state in all possible displacements to current_row are done)
	 * {
	 * Note that for each tile we have that it either has a non-empty set 'cells_for_displacement' or it has been
	 * completely displaced (and its value is 0).
	 */

	/* Check if all numbers in current row:
	 * - satisfy row clue
	 * - don't repeat in the grid
	 * If so then: add numbers to the answer, call solve()
	 */
//}

/*
void solve()
{
	// Pick the most promising row that has not been processed yet. If no such row then print answer.
	int best_row = 42;
	// for the most promising row:
	solve_row(best_row);
}
*/

class Board
{
public:
	typedef bool (*CheckHintsFun)(int row, int arg);

	struct Region
	{
		// indices of regions that are adjacent (orthogonally) to us
		std::vector<int8_t> neighbors;
		int8_t value = -1; // value of region or -1 if not set yet
	};

	struct Tile
	{
		int8_t col; // tile's column
		// A set of orthogonally adjacent cells to which we can still displace (part of) our value, i'th bit represents
		// position of tile_pos + all_directions[i]. Bit value of true means that corresponding cell is in the set.
		// When the set is empty, the tile's value in cell_value must be 0.
		std::bitset<4> cells_for_displacement;
	};

	struct Row
	{
		std::vector<Tile> tiles;
		CheckHintsFun hints_fun;
		int hints_arg;
	};

	Board(int num_rows, int num_cols):
		num_rows(num_rows),
		num_cols(num_cols),
		cell_value(new int8_t[num_rows * num_cols]),
		cell_region(new int8_t[num_rows * num_cols]),
		cell_is_highlighted(num_rows * num_cols),
		regions(),
		rows(new Row[num_rows] {}),
		row_is_processed(num_rows)
	{
		std::fill(&cell_value[0], &cell_value[num_rows * num_cols], -1);
		std::fill(&cell_region[0], &cell_region[num_rows * num_cols], -1);
	}

	void set_num_regions(int n)
	{
		regions.resize(n);
	}

	// TODO: add method to compute neighbors

	bool is_on_board(Pos const pos) const
	{
		return pos.row >= 0 && pos.row < num_rows
			&& pos.col >= 0 && pos.col < num_cols;
	}

	int cell_index(Pos const pos) const
	{
		return pos.row * num_cols + pos.col;
	}

	int8_t get_cell_value(Pos const pos) const
	{
		assert(is_on_board(pos));
		return cell_value[cell_index(pos)];
	}

	void set_cell_value(Pos const pos, int8_t val)
	{
		assert(is_on_board(pos));
		cell_value[cell_index(pos)] = val;
	}

	int8_t get_cell_region(Pos const pos) const
	{
		assert(is_on_board(pos));
		return cell_region[cell_index(pos)];
	}

	void set_cell_region(Pos const pos, int8_t val)
	{
		assert(is_on_board(pos));
		cell_region[cell_index(pos)] = val;
	}

	int8_t get_cell_is_highlighted(Pos const pos) const
	{
		assert(is_on_board(pos));
		return cell_is_highlighted.get_bit(cell_index(pos));
	}

	void set_cell_is_highlighted(Pos const pos, bool val)
	{
		assert(is_on_board(pos));
		if (val)
			cell_is_highlighted.set_bit(cell_index(pos));
		else
			cell_is_highlighted.clear_bit(cell_index(pos));
	}

	int num_rows;
	int num_cols;

private:
	// This is a value seen on a cell, it includes displacements. The value gets set when processing a row, and
	// is initially taken from the value of cell's region. It is -1 in rows not yet processed.
	std::unique_ptr<int8_t[]> cell_value; // grid of cells

	// index into regions
	std::unique_ptr<int8_t[]> cell_region; // grid of cells

	DynamicBitset cell_is_highlighted; // grid of cells

	std::vector<Region> regions;

	std::unique_ptr<Row[]> rows; // for each row
	DynamicBitset row_is_processed; // for each row
};

Board read_data(std::istream & inp)
{
	auto oldmask = inp.exceptions();
	inp.exceptions(std::ios::failbit);

	int num_rows, num_cols;
	skipComments(inp);
	inp >> num_rows >> num_cols;

	assert(num_rows >= 0 && num_rows <= std::numeric_limits<int8_t>::max());
	assert(num_cols >= 0 && num_cols <= std::numeric_limits<int8_t>::max());

	Board board(num_rows, num_cols);

	int num_regions;
	skipComments(inp);
	inp >> num_regions;

	assert(num_regions >= 0 && num_regions <= num_rows * num_cols);
	board.set_num_regions(num_regions);

	// read regions
	skipComments(inp);
	for (int row = 0; row < num_rows; ++row)
	{
		int tmp;
		inp >> tmp;
		assert(tmp == row);
		for (int col = 0; col < num_cols; ++col)
		{
			inp >> tmp;
			assert(tmp >= 0 && tmp < num_regions);
			board.set_cell_region({row, col}, tmp);
		}
	}

	// read highlights
	skipComments(inp);
	for (int row = 0; row < num_rows; ++row)
	{
		int tmp;
		inp >> tmp;
		assert(tmp == row);
		for (int col = 0; col < num_cols; ++col)
		{
			inp >> tmp;
			assert(tmp == 0 || tmp == 1);
			board.set_cell_is_highlighted({row, col}, tmp);
		}
	}

	// read hints
	skipComments(inp);
	for (int row = 0; row < num_rows; ++row)
	{
		int tmp;
		inp >> tmp;
		assert(tmp == row);
		std::string hint;
		inp >> hint >> tmp;
		// TODO
	}

	skipComments(inp);
	inp.exceptions(oldmask);
	assert(inp.get() == std::char_traits<char>::eof());
	return board;
}

int main()
{
	Board board = read_data(std::cin);
}
