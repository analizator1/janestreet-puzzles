#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "hints.h"
#include "utils.h"

#if 1
// DBG logging enabled
#define DBG(expr) expr
#else
#define DBG(expr) do {} while (0)
#endif

#if 0
// DBG2 logging enabled
#define DBG2(expr) expr
#else
#define DBG2(expr) do {} while (0)
#endif

#if 0
// DBG3 logging enabled
#define DBG3(expr) expr
#else
#define DBG3(expr) do {} while (0)
#endif

using namespace std::placeholders;

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

Pos const orthogonal_dirs[4] = {vec_up, vec_right, vec_down, vec_left};
} // anonymous namespace


static constexpr int max_num_regions = 16;

class Board
{
public:
	struct Region
	{
		// indices of regions that are adjacent (orthogonally) to us
		std::vector<int8_t> neighbors_vec;
		int8_t value = -1; // value of region or -1 if not set yet
	};

	struct Tile
	{
		int8_t col; // tile's column
		// A set of orthogonally adjacent cells to which we can still displace (part of) our value, i'th bit represents
		// position of tile_pos + orthogonal_dirs[i]. Bit value of true means that corresponding cell is in the set.
		// When the set is empty, the tile's value must be 0.
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
		numbers_in_grid(),
		cell_value(new int8_t[num_rows * num_cols]),
		cell_region(new int8_t[num_rows * num_cols]),
		cell_is_highlighted(num_rows * num_cols),
		regions(),
		rows(new Row[num_rows] {}),
		row_is_processed(num_rows),
		row_has_region_assignments(num_rows),
		row_has_tiles_placement(num_rows)
	{
		std::fill(&cell_value[0], &cell_value[num_rows * num_cols], -1);
		std::fill(&cell_region[0], &cell_region[num_rows * num_cols], -1);
	}

	void set_num_regions(int n)
	{
		regions.resize(n);
	}

	int get_num_regions() const
	{
		return (int)regions.size();
	}

	std::vector<int8_t> const & get_region_neighbors(int region_idx) const
	{
		assert(region_idx >= 0 && region_idx < get_num_regions());
		return regions[region_idx].neighbors_vec;
	}

	int8_t get_region_value(int region_idx) const
	{
		assert(region_idx >= 0 && region_idx < get_num_regions());
		return regions[region_idx].value;
	}

	void set_region_value(int region_idx, int8_t val)
	{
		assert(region_idx >= 0 && region_idx < get_num_regions());
		regions[region_idx].value = val;
	}

	void compute_region_neighbors()
	{
		int const num_regions = regions.size();
		std::vector<std::unordered_set<int8_t>> region_neighbors(num_regions);
		for (int row = 0; row < num_rows; ++row)
		{
			for (int col = 0; col < num_cols; ++col)
			{
				Pos const current_pos {row, col};
				int8_t const current_region_idx = get_cell_region(current_pos);
				for (Pos const vec : orthogonal_dirs)
				{
					Pos const neighbor_pos = current_pos + vec;
					if (is_on_board(neighbor_pos))
					{
						int8_t const neighbor_region_idx = get_cell_region(neighbor_pos);
						if (neighbor_region_idx != current_region_idx)
						{
							region_neighbors[current_region_idx].insert(neighbor_region_idx);
							region_neighbors[neighbor_region_idx].insert(current_region_idx);
						}
					}
				}
			}
		}

		for (int region_idx = 0; region_idx < num_regions; ++region_idx)
		{
			std::unordered_set<int8_t> const & neigh_set = region_neighbors[region_idx];
			std::copy(neigh_set.begin(), neigh_set.end(), std::back_inserter(regions[region_idx].neighbors_vec));
			std::sort(regions[region_idx].neighbors_vec.begin(), regions[region_idx].neighbors_vec.end());
		}
	}

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

	bool get_cell_is_highlighted(Pos const pos) const
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

	void set_hints_fun(CheckHintsFun fun, int arg, int row)
	{
		assert(row >= 0 && row < num_rows);
		rows[row].hints_fun = fun;
		rows[row].hints_arg = arg;
	}

	bool call_row_hints_fun(int row, uint64_t number) const
	{
		assert(row >= 0 && row < num_rows);
		return rows[row].hints_fun(number, rows[row].hints_arg);
	}

	CheckHintsFun get_row_hints_fun(int row) const
	{
		assert(row >= 0 && row < num_rows);
		return rows[row].hints_fun;
	}

	std::vector<Tile> & get_row_tiles(int row)
	{
		assert(row >= 0 && row < num_rows);
		return rows[row].tiles;
	}

	bool get_row_is_processed(int row) const
	{
		return row_is_processed.get_bit(row);
	}

	void set_row_is_processed(int row, bool val)
	{
		if (val)
			row_is_processed.set_bit(row);
		else
			row_is_processed.clear_bit(row);
	}

	bool get_row_has_region_assignments(int row) const
	{
		return row_has_region_assignments.get_bit(row);
	}

	void set_row_has_region_assignments(int row, bool val)
	{
		if (val)
			row_has_region_assignments.set_bit(row);
		else
			row_has_region_assignments.clear_bit(row);
	}

	bool get_row_has_tiles_placement(int row) const
	{
		return row_has_tiles_placement.get_bit(row);
	}

	void set_row_has_tiles_placement(int row, bool val)
	{
		if (val)
			row_has_tiles_placement.set_bit(row);
		else
			row_has_tiles_placement.clear_bit(row);
	}

	int num_rows;
	int num_cols;

	std::unordered_set<uint64_t> numbers_in_grid;

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
	DynamicBitset row_has_region_assignments; // for each row
	DynamicBitset row_has_tiles_placement; // for each row
};

std::ostream & operator<<(std::ostream & out, Board const & board)
{
	out << "Grid:\n";
	for (int row = 0; row < board.num_rows; ++row)
	{
		out << std::setw(2) << row << ' ';
		out << (board.get_row_is_processed(row) ? '+' : ' ');
		for (int col = 0; col < board.num_cols; ++col)
		{
			int const value = board.get_cell_value({row, col});
			if (value == -1)
				out << " .";
			else
				out << ' ' << value;
		}
		out << '\n';
	}

	out << "\nRegion values:";
	for (int region_idx = 0; region_idx < board.get_num_regions(); ++region_idx)
	{
		int const value = board.get_region_value(region_idx);
		out << ' ' << region_idx << "=>" << value;
	}

	out << "\nNumbers:";
	uint64_t sum = 0;
	for (uint64_t number : board.numbers_in_grid)
	{
		out << ' ' << number;
		sum += number;
	}
	out << "\nSum: " << sum << '\n';

	return out;
}

Board read_data(std::istream & inp)
{
	auto oldmask = inp.exceptions();
	inp.exceptions(std::ios::failbit);

	int num_rows, num_cols;
	skipComments(inp);
	inp >> num_rows >> num_cols;

	assert(num_rows > 0 && num_rows <= std::numeric_limits<int8_t>::max());
	assert(num_cols > 0 && num_cols <= std::numeric_limits<int8_t>::max());

	Board board(num_rows, num_cols);

	int num_regions;
	skipComments(inp);
	inp >> num_regions;

	assert(num_regions > 0 && num_regions <= num_rows * num_cols &&
			num_regions <= std::numeric_limits<int8_t>::max() &&
			num_regions <= max_num_regions);
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
		std::string name;
		inp >> name >> tmp;

		CheckHintsFun hints_fun = get_hints_fun(name);
		if (!hints_fun)
		{
			std::cerr << "unknown hint name: " << name << "\n";
			std::exit(1);
		}
		board.set_hints_fun(hints_fun, tmp, row);
	}

	skipComments(inp);
	inp.exceptions(oldmask);
	assert(inp.get() == std::char_traits<char>::eof());
	return board;
}

void print_regions_neighbors(Board const & board)
{
	std::cout << "Region neighbors:\n";
	for (int region_idx = 0; region_idx < board.get_num_regions(); ++region_idx)
	{
		std::cout << region_idx << ':';
		for (int const neighbor_region_idx : board.get_region_neighbors(region_idx))
			std::cout << ' ' << neighbor_region_idx;
		std::cout << '\n';
	}
}

/*
 * Solution draft:
 * void solve_row(int current_row)
 * {
 */
	/* for (state in: rows in [current_row-1; current_row+1] have: for each cell, the region of that cell has a number
	 * in [1; 9] assigned)
	 * { done by AssignRowRegionsValue
	 */

	/* for (state in: rows in [current_row-1; current_row+1] have: tiles in the row placed such that for each tile there
	 * is a non-empty set 'cells_for_displacement')
	 * { done by PlaceRowTiles
	 * Note that when recursing over tiles assignment, it may happen that a row already has a tile assignment (from
	 * a different current_row).
	 */

	/* for (state in all possible displacements to current_row are done)
	 * { done by DisplaceTilesValue
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

class AssignRowRegionsValue
{
public:
	// Callback is called when each region in the row has a value assigned in [1; 9].
	// return value: true if visiting should be continued
	using Callback = std::function<bool()>;

	AssignRowRegionsValue(Board & board, int row, Callback const & callback):
		board(board),
		callback(callback),
		row(row)
	{
	}

	// return value: true if visiting should be continued
	bool run()
	{
		return rec_visit(0);
	}

private:
	// col is the first one to try
	bool rec_visit(int col)
	{
		// advance col to the first cell with region without a value
		while (col < board.num_cols && board.get_region_value(board.get_cell_region({row, col})) >= 0)
			++col;
		if (col < board.num_cols)
		{
			// found region without a value
			int const region_idx = board.get_cell_region({row, col});
			assert(board.get_region_value(region_idx) == -1);
			// try all possible digits for the region
			for (int8_t digit = 1; digit <= 9; ++digit)
			{
				// check if digit can be set for this region
				bool digit_is_ok = true;
				for (int const neighbor_region_idx : board.get_region_neighbors(region_idx))
				{
					if (board.get_region_value(neighbor_region_idx) == digit)
					{
						digit_is_ok = false;
						break;
					}
				}
				if (digit_is_ok)
				{
					board.set_region_value(region_idx, digit);
					bool const visit_more = rec_visit(col + 1);
					board.set_region_value(region_idx, -1);
					if (!visit_more)
						return false;
				}
			}

			return true;
		}
		else
		{
			// all cells in the row checked
			return callback();
		}
	}

	Board & board;
	Callback const callback;
	int const row;
};

static constexpr int min_number_len = 2;

static int max_num_tiles_in_row(int num_cols)
{
	// one tile in the first cell, the in the reamining cells there must be 3 cells per tile (numbers must be at least 2
	// digits long)
	return 1 + (num_cols - 1) / (min_number_len + 1);
}

class PlaceRowTiles
{
public:
	// Callback is called when tiles are placed in the row
	// return value: true if visiting should be continued
	using Callback = std::function<bool()>;

	PlaceRowTiles(Board & board, int row, Callback const & callback):
		board(board),
		callback(callback),
		row(row)
	{
	}

	// return value: true if visiting should be continued
	bool run()
	{
		for (int num_tiles = 0; num_tiles <= max_num_tiles_in_row(board.num_cols); ++num_tiles)
		{
			bool const visit_more = try_placing_tiles(0, num_tiles);
			if (!visit_more)
				return false;
		}
		return true;
	}

private:
	// It tries to place exactly remaining_tiles tiles, assuming that current number starts at start_col.
	bool try_placing_tiles(int const start_col, int remaining_tiles)
	{
		if (remaining_tiles == 0)
		{
			// we placed all tiles, but the last numer's length needs to be checked
			int const number_len = board.num_cols - start_col;
			if (number_len >= min_number_len || number_len == 0)
			{
				bool const visit_more = callback();
				return visit_more;
			}
		}
		else
		{
			assert(remaining_tiles > 0);
			// we need to place a tile
			for (int end_col = start_col; end_col < board.num_cols; ++end_col)
			{
				int const number_len = end_col - start_col;
				if ((number_len >= min_number_len || (number_len == 0 && end_col == 0))
						&& !board.get_cell_is_highlighted({row, end_col}))
				{
					// check tiles in adjacent rows
					bool is_ok = true;
					for (int adjacent_row : {row - 1, row + 1})
					{
						if (adjacent_row >= 0 && adjacent_row < board.num_rows)
						{
							// is there a tile in {adjacent_row, col}?
							std::vector<Board::Tile> const & adjacent_tiles = board.get_row_tiles(adjacent_row);
							auto it = std::find_if(adjacent_tiles.begin(), adjacent_tiles.end(),
									[end_col](Board::Tile const & tile) { return tile.col == end_col; });
							if (it != adjacent_tiles.end())
							{
								is_ok = false;
								break;
							}
						}
					}
					if (is_ok)
					{
						// we can place tile at {row, col}
						// but only if cells_for_displacement set is not empty
						std::bitset<4> cells_for_displacement;
						Pos const tile_pos {row, end_col};
						for (int i = 0; i < 4; ++i)
						{
							Pos const vec = orthogonal_dirs[i];
							Pos const pos_for_displacement = tile_pos + vec;
							if (board.is_on_board(pos_for_displacement) &&
									!board.get_cell_is_highlighted(pos_for_displacement))
							{
								// we can displace to pos_for_displacement
								cells_for_displacement.set(i);
							}
						}
						if (cells_for_displacement.any())
						{
							// this is a valid tile position
							board.get_row_tiles(row).push_back({(int8_t)end_col, cells_for_displacement});
							bool const visit_more = try_placing_tiles(end_col + 1, remaining_tiles - 1);
							board.get_row_tiles(row).pop_back();
							if (!visit_more)
								return false;
						}
					}
				}
			}
		}
		return true;
	}

	Board & board;
	Callback const callback;
	int const row;
};

class DisplaceTilesValue
{
public:
	// Callback is called when for each tile in rows in [current_row-1; current_row+1], part of (or whole) its value is
	// displaced to a cell in target_row.
	// return value: true if visiting should be continued
	using Callback = std::function<bool()>;

	// Called often to check if constraints are satisfied, but numbers are only valid until first_not_done_col.
	using ConstraintsCallback = std::function<bool(int first_not_done_col)>;

	DisplaceTilesValue(Board & board, int target_row, Callback const & continuation_callback,
			ConstraintsCallback const & constraints_callback):
		board(board),
		continuation_callback(continuation_callback),
		constraints_callback(constraints_callback),
		target_row(target_row),
		displacements()
	{
	}

	// return value: true if visiting should be continued
	bool run()
	{
		for (int source_row = target_row - 1; source_row <= target_row + 1; ++source_row)
		{
			if (source_row < 0 || source_row >= board.num_rows)
				continue;
			std::vector<Board::Tile> const & source_tiles = board.get_row_tiles(source_row);
			for (int source_tile_idx = 0; source_tile_idx < (int)source_tiles.size(); ++source_tile_idx)
			{
				Board::Tile const & tile = source_tiles[source_tile_idx];
				Pos const tile_pos {source_row, tile.col};
				for (int i = 0; i < 4; ++i)
				{
					if (tile.cells_for_displacement[i])
					{
						Pos const vec = orthogonal_dirs[i];
						Pos const pos_for_displacement = tile_pos + vec;
						if (pos_for_displacement.row == target_row)
						{
							displacements.push_back({(int8_t)pos_for_displacement.col, (int8_t)source_row,
									(int8_t)source_tile_idx, (int8_t)i});
						}
					}
				}
			}
		}

		// sort by column of displacement in target_row, needed by constraints_callback
		std::sort(displacements.begin(), displacements.end());

		return rec_displace(0);
	}

private:
	struct Displacement
	{
		int8_t pos_for_displacement_col;
		int8_t source_row;
		int8_t source_tile_idx;
		int8_t cells_for_displacement_idx;

		bool operator<(Displacement const & other) const
		{
			return pos_for_displacement_col < other.pos_for_displacement_col;
		}
	};

	bool rec_displace(int const displacement_idx)
	{
		if (displacement_idx >= (int)displacements.size())
		{
			// we have a proper displacement
			bool const visit_more = continuation_callback();
			return visit_more;
		}
		else
		{
			{
				int const first_not_done_col = displacements[displacement_idx].pos_for_displacement_col;
				if (!constraints_callback(first_not_done_col))
				{
					// prune this search branch early
					return true;
				}
			}

			int const source_tile_idx = displacements[displacement_idx].source_tile_idx;
			int const source_row = displacements[displacement_idx].source_row;
			std::vector<Board::Tile> & source_tiles = board.get_row_tiles(source_row);
			Board::Tile & tile = source_tiles[source_tile_idx];
			Pos const tile_pos {source_row, tile.col};
			int const i = displacements[displacement_idx].cells_for_displacement_idx;
			Pos const vec = orthogonal_dirs[i];
			Pos const pos_for_displacement = tile_pos + vec;
			assert(pos_for_displacement.row == target_row);

			int const orig_pos_for_displacement_value = board.get_cell_value(pos_for_displacement);
			int const max_addition = 9 - orig_pos_for_displacement_value;
			assert(max_addition >= 0);
			assert(max_addition <= 8);

			int const orig_tile_value = board.get_cell_value(tile_pos);

			assert(tile.cells_for_displacement[i]);
			// remote from cells_for_displacement
			tile.cells_for_displacement.reset(i);

			bool visit_more = true;
			int add = tile.cells_for_displacement.any()
				? 0                // can displace values <= orig_tile_value
				: orig_tile_value; // can displace exactly orig_tile_value
			for (; add <= orig_tile_value && add <= max_addition; ++add)
			{
				board.set_cell_value(pos_for_displacement, orig_pos_for_displacement_value + add);
				board.set_cell_value(tile_pos, orig_tile_value - add);
				visit_more = rec_displace(displacement_idx + 1);
				board.set_cell_value(tile_pos, orig_tile_value);
				board.set_cell_value(pos_for_displacement, orig_pos_for_displacement_value);
				if (!visit_more)
					break;
			}

			// add back to cells_for_displacement
			tile.cells_for_displacement.set(i);
			return visit_more;
		}
	}

	Board & board;
	Callback const continuation_callback;
	ConstraintsCallback const constraints_callback;
	int const target_row;
	std::vector<Displacement> displacements;
};

class RowProcessor
{
public:
	// Callback is called when current_row has fixed cell values. This involves fixing values of regions for each row in
	// [current_row-1; current_row+1], same for tiles. Displacements are only done to current_row. Numbers are added to
	// the global set and they satisfy row hint.
	// return value: true if visiting should be continued
	using Callback = std::function<bool()>;

	RowProcessor(Board & board, int current_row, Callback const & callback):
		board(board),
		callback(callback),
		current_row(current_row)
	{
	}

	// return value: true if visiting should be continued
	bool run()
	{
		// we must do assignments for row in [current_row-1; current_row+1]
		return perform_row_region_assignments(current_row - 1);
	}

private:
	bool perform_row_region_assignments(int const row)
	{
		if (row <= current_row + 1)
		{
			if (row < 0 || row >= board.num_rows || board.get_row_has_region_assignments(row))
			{
				return perform_row_region_assignments(row + 1);
			}

			// perform region assignments in row
			assert(!board.get_row_has_region_assignments(row));
			board.set_row_has_region_assignments(row, true);
			AssignRowRegionsValue work(board, row, std::bind(&RowProcessor::done_row_region_assignments, this, row));
			bool const visit_more = work.run();
			board.set_row_has_region_assignments(row, false);
			return visit_more;
		}
		else
		{
			// all rows in [current_row-1; current_row+1] have region assignments
			return perform_row_tiles_placement(current_row - 1);
		}
	}

	bool done_row_region_assignments(int const row)
	{
		// set cell values
		for (int col = 0; col < board.num_cols; ++col)
		{
			assert(board.get_cell_value({row, col}) == -1);
			int const region_idx = board.get_cell_region({row, col});
			board.set_cell_value({row, col}, board.get_region_value(region_idx));
		}
		// proceed to next row
		bool const visit_more = perform_row_region_assignments(row + 1);
		// restore cell values
		for (int col = 0; col < board.num_cols; ++col)
		{
			board.set_cell_value({row, col}, -1);
		}
		return visit_more;
	}

	bool perform_row_tiles_placement(int const row)
	{
		if (row <= current_row + 1)
		{
			if (row < 0 || row >= board.num_rows || board.get_row_has_tiles_placement(row))
			{
				return perform_row_tiles_placement(row + 1);
			}

			// perform tiles placement in row
			assert(!board.get_row_has_tiles_placement(row));
			board.set_row_has_tiles_placement(row, true);
			PlaceRowTiles work(board, row, std::bind(&RowProcessor::done_row_tiles_placement, this, row));
			bool const visit_more = work.run();
			board.set_row_has_tiles_placement(row, false);
			return visit_more;
		}
		else
		{
			// all rows in [current_row-1; current_row+1] have tiles placement
			return perform_displacements_to_current_row();
		}
	}

	bool done_row_tiles_placement(int const row)
	{
		// proceed to next row
		bool const visit_more = perform_row_tiles_placement(row + 1);
		return visit_more;
	}

	bool perform_displacements_to_current_row()
	{
		DisplaceTilesValue work(board, current_row,
				std::bind(&RowProcessor::check_row_hints_and_call_back, this),
				std::bind(&RowProcessor::constraints_callback, this, _1));
		bool const visit_more = work.run();
		return visit_more;
	}

	bool check_row_hints(std::vector<uint64_t> & added_numbers, int first_not_done_col)
	{
		std::vector<Board::Tile> const & tiles = board.get_row_tiles(current_row);
		bool is_ok = true;

		// collect numbers in row, check for duplicates in grid, check hints
		auto it = tiles.begin();
		while (true)
		{
			// consider number ending at current tile (it)
			int const start_col = it == tiles.begin() ? 0 : (it - 1)->col + 1;
			int const end_col = it == tiles.end() ? board.num_cols : it->col;
			if (end_col > first_not_done_col)
			{
				// number is not fixed yet (displacements not done)
				break;
			}
			// number is fixed, check it
			if (start_col < end_col)
			{
				uint64_t number = 0;
				for (int col = start_col; col < end_col; ++col)
				{
					number = number * 10 + board.get_cell_value({current_row, col});
				}
				auto p = board.numbers_in_grid.insert(number);
				if (p.second)
				{
					added_numbers.push_back(number);
					if (!board.call_row_hints_fun(current_row, number))
					{
						is_ok = false;
						break;
					}
				}
				else
				{
					// duplicate number
					is_ok = false;
					break;
				}
			}

			if (it == tiles.end())
				break;
			++it;
		}

		return is_ok;
	}

	bool constraints_callback(int first_not_done_col)
	{
		std::vector<uint64_t> added_numbers;
		bool const is_ok = check_row_hints(added_numbers, first_not_done_col);

		for (uint64_t number : added_numbers)
		{
			int const num_erased = board.numbers_in_grid.erase(number);
			assert(num_erased == 1);
		}

		return is_ok;
	}

	bool check_row_hints_and_call_back()
	{
		std::vector<uint64_t> added_numbers;
		bool const is_ok = check_row_hints(added_numbers, board.num_cols);

		bool visit_more = true;
		if (is_ok)
		{
			DBG2(std::cout << "RowProcessor: found values for row " << current_row
				<< ", board:\n" << board << std::endl);

			// hints are satisfied and numbers are unique in grid
			visit_more = callback();
		}
		else
		{
			DBG3(std::cout << "RowProcessor: did NOT find values for row " << current_row
				<< ", board:\n" << board << std::endl);
		}

		for (uint64_t number : added_numbers)
		{
			int const num_erased = board.numbers_in_grid.erase(number);
			assert(num_erased == 1);
		}

		return visit_more;
	}

	Board & board;
	Callback const callback;
	int const current_row;
};

class NumberCrossSolver
{
public:
	// Callback is called when grid is solved.
	using Callback = std::function<void()>;

	NumberCrossSolver(Board & board, Callback const & callback):
		board(board),
		callback(callback),
		rec_level(0)
	{
	}

	void run()
	{
		rec_solve();
	}

private:
	void rec_solve()
	{
		// Pick the most promising row that has not been processed yet. If no such row then print answer.
		// Row is better if it has lower branching degree, i.e. lower number of direct calls to rec_solve().

		// Prepare a vector of rows to check, in a heuristic order from the lowest branching degree.
		std::vector<std::pair<int, int>> rows_to_check;
		rows_to_check.reserve(board.num_rows);
		for (int row = 0; row < board.num_rows; ++row)
		{
			if (!board.get_row_is_processed(row))
			{
				// compute number of different regions that don't have their value set yet
				std::bitset<max_num_regions> regions_without_value;
				for (int col = 0; col < board.num_cols; ++col)
				{
					int const region_idx = board.get_cell_region({row, col});
					if (board.get_region_value(region_idx) == -1)
						regions_without_value.set(region_idx);
				}
				int order_val = regions_without_value.count();
				// HACK
				//if (board.get_row_hints_fun(row) == is_fibonacci)
				//	order_val = -1;
				rows_to_check.emplace_back(order_val, row);
			}
		}
		std::sort(rows_to_check.begin(), rows_to_check.end());

		// find best row
		int best_row = -1;
		uint32_t best_row_degree = std::numeric_limits<uint32_t>::max();
		for (auto const & p : rows_to_check)
		{
			int const current_row = p.second;

			uint32_t current_row_degree = 0;
			RowProcessor work(board, current_row, [&]()
			{
				++current_row_degree;

				DBG2(std::cout << "[degree testing callback]"
					<< " rec_level: " << rec_level
					<< " degree of current_row: " << current_row << " is: " << current_row_degree
					<< " best_row_degree: " << best_row_degree
					<< " best_row: " << best_row
					<< std::endl);

				return current_row_degree < best_row_degree;
			});
			work.run();

			if (current_row_degree < best_row_degree)
			{
				best_row_degree = current_row_degree;
				best_row = current_row;
			}

			DBG(std::cout << __func__
				<< " rec_level: " << rec_level
				<< " degree of current_row: " << current_row << " is: " << current_row_degree
				<< " best_row_degree: " << best_row_degree
				<< " best_row: " << best_row
				<< std::endl);

			if (best_row_degree == 0)
				break;
		}

		DBG(std::cout << __func__
			<< " rec_level: " << rec_level
			<< " best_row: " << best_row
			<< " best_row_degree: " << best_row_degree << std::endl);

		if (best_row_degree == std::numeric_limits<uint32_t>::max())
		{
			// all rows were processed
			callback();
		}
		else if (best_row_degree > 0)
		{
			// process best row, calling us recursively
			assert(!board.get_row_is_processed(best_row));
			board.set_row_is_processed(best_row, true);
			RowProcessor work(board, best_row, [this]()
			{
				++rec_level;
				rec_solve();
				--rec_level;
				return true;
			});
			work.run();
			board.set_row_is_processed(best_row, false);
		}
	}

	Board & board;
	Callback const callback;
	int rec_level;
};

int main()
{
#ifndef NDEBUG
	std::cout << "Running in debug config" << std::endl;
#else
	std::cout << "Running in release config" << std::endl;
#endif

	Board board = read_data(std::cin);
	board.compute_region_neighbors();
	print_regions_neighbors(board);

	NumberCrossSolver solver(board, [&]()
	{
		std::cout << "Found solution:\n" << board << std::endl;
	});
	std::cout << "Solving..." << std::endl;
	solver.run();
}
