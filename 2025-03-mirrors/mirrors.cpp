#include <iostream>
#include <memory>
#include <cstdint>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>
#include <functional>
#include <limits>

static constexpr int n = 10; // side of field

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

static Pos const vec_up    = {-1, 0};
static Pos const vec_right = {0, 1};
static Pos const vec_down  = {1, 0};
static Pos const vec_left  = {0, -1};

static Pos const all_dirs[4] = {vec_up, vec_right, vec_down, vec_left};

enum class CellType: uint8_t
{
	Empty = 0,
	ForwardMirror,
	BackwardMirror,
	LaserBeam
};

enum Direction
{
	UpDir,
	RightDir,
	DownDir,
	LeftDir
};

static Pos const direction_to_vec[4] = {
	vec_up,
	vec_right,
	vec_down,
	vec_left
};

static Direction const dir_after_forward_mirror[4] = {
	RightDir,
	UpDir,
	LeftDir,
	DownDir
};

static Direction const dir_after_backward_mirror[4] = {
	LeftDir,
	DownDir,
	RightDir,
	UpDir
};

static Direction opposite_direction(Direction dir)
{
	return Direction((dir + 2) % 4);
}

class Board
{
public:
	Board():
		cells(new CellType[n * n]{}),
		lasers(new unsigned int[4 * n]{}),
		laser_has_path(new bool[4 * n]{})
	{
		std::cout << "Created board with side " << n << std::endl;
	}

	Board(Board const & other):
		cells(new CellType[n * n]),
		lasers(new unsigned int[4 * n]),
		laser_has_path(new bool[4 * n])
	{
		std::copy(&other.cells[0], &other.cells[n * n], &this->cells[0]);
		std::copy(&other.lasers[0], &other.lasers[4 * n], &this->lasers[0]);
		std::copy(&other.laser_has_path[0], &other.laser_has_path[4 * n], &this->laser_has_path[0]);
	}

	CellType & cell(int row, int col)
	{
		assert(is_on_board(row, col));
		return cells[row * n + col];
	}

	CellType cell(int row, int col) const
	{
		assert(is_on_board(row, col));
		return cells[row * n + col];
	}

	CellType & cell(Pos const & pos)
	{
		return cell(pos.row, pos.col);
	}

	CellType cell(Pos const & pos) const
	{
		return cell(pos.row, pos.col);
	}

	unsigned int * get_lasers()
	{
		return lasers.get();
	}

	unsigned int const * get_lasers() const
	{
		return lasers.get();
	}

	bool * get_laser_has_path()
	{
		return laser_has_path.get();
	}

	bool const * get_laser_has_path() const
	{
		return laser_has_path.get();
	}

	bool is_on_board(int row, int col) const
	{
		return (row >= 0 && row < n) && (col >= 0 && col < n);
	}

	bool is_on_board(Pos const & pos) const
	{
		return is_on_board(pos.row, pos.col);
	}

	std::pair<int, int> get_laser_section_and_offset(int row, int col) const
	{
		int laser_section_idx = -1;
		int laser_offset = -1;

		if (row == -1) // top
		{
			laser_section_idx = UpDir;
			laser_offset = col;
		}
		else if (col == n) // right
		{
			laser_section_idx = RightDir;
			laser_offset = row;
		}
		else if (row == n) // bottom
		{
			laser_section_idx = DownDir;
			laser_offset = col;
		}
		else if (col == -1) // left
		{
			laser_section_idx = LeftDir;
			laser_offset = row;
		}

		assert(laser_section_idx >= 0 && laser_section_idx < 4);
		assert(laser_offset >= 0 && laser_offset < n);
		return {laser_section_idx, laser_offset};
	}

	std::pair<int, int> get_laser_section_and_offset(Pos const & pos) const
	{
		return get_laser_section_and_offset(pos.row, pos.col);
	}

	Pos laser_section_and_offset_to_pos(int laser_section_idx, int laser_offset) const
	{
		switch (laser_section_idx)
		{
		case UpDir:
			return {-1, laser_offset};
		case RightDir:
			return {laser_offset, n};
		case DownDir:
			return {n, laser_offset};
		case LeftDir:
			return {laser_offset, -1};
		}
		assert(false);
	}

	unsigned int & laser(int row, int col)
	{
		auto [laser_section_idx, laser_offset] = get_laser_section_and_offset(row, col);
		int const laser_idx = laser_section_idx * n + laser_offset;
		return lasers[laser_idx];
	}

	unsigned int laser(int row, int col) const
	{
		auto [laser_section_idx, laser_offset] = get_laser_section_and_offset(row, col);
		int const laser_idx = laser_section_idx * n + laser_offset;
		return lasers[laser_idx];
	}

	unsigned int & laser(Pos const & pos)
	{
		return laser(pos.row, pos.col);
	}

	unsigned int laser(Pos const & pos) const
	{
		return laser(pos.row, pos.col);
	}

private:
	// n * n board
	// rows and cols are in [0; n-1] range
	std::unique_ptr<CellType[]> cells;

	// 4 * n numbers, one per laser
	// each number is a product of the segment lengths of a laser's path or 0 if unknown
	// these are cells adjacent to board:
	// top: row==-1, col in [0; n-1]
	// right: col==n, row in [0; n-1]
	// bottom: row==n, col in [0; n-1]
	// left: col==-1, row in [0; n-1]
	std::unique_ptr<unsigned int[]> lasers;

	// 4 * n numbers, one per laser
	std::unique_ptr<bool[]> laser_has_path;
};

std::ostream & operator<<(std::ostream & out, Board const & board)
{
	std::unique_ptr<std::string[]> lasers(new std::string[4 * n]);

	int top_num_max_len = 0;
	int right_num_max_len = 0;
	int bottom_num_max_len = 0;
	int left_num_max_len = 0;

	// go over top lasers
	for (int col = 0; col < n; ++col)
	{
		int const laser_idx = UpDir * n + col;
		std::ostringstream ostr;
		if (board.get_lasers()[laser_idx])
			ostr << board.get_lasers()[laser_idx];
		lasers[laser_idx] = ostr.str();
		top_num_max_len = std::max(top_num_max_len, (int)lasers[laser_idx].size());
	}

	// go over right lasers
	for (int row = 0; row < n; ++row)
	{
		int const laser_idx = RightDir * n + row;
		std::ostringstream ostr;
		if (board.get_lasers()[laser_idx])
			ostr << board.get_lasers()[laser_idx];
		lasers[laser_idx] = ostr.str();
		right_num_max_len = std::max(right_num_max_len, (int)lasers[laser_idx].size());
	}

	// go over bottom lasers
	for (int col = 0; col < n; ++col)
	{
		int const laser_idx = DownDir * n + col;
		std::ostringstream ostr;
		if (board.get_lasers()[laser_idx])
			ostr << board.get_lasers()[laser_idx];
		lasers[laser_idx] = ostr.str();
		bottom_num_max_len = std::max(bottom_num_max_len, (int)lasers[laser_idx].size());
	}

	// go over left lasers
	for (int row = 0; row < n; ++row)
	{
		int const laser_idx = LeftDir * n + row;
		std::ostringstream ostr;
		if (board.get_lasers()[laser_idx])
			ostr << board.get_lasers()[laser_idx];
		lasers[laser_idx] = ostr.str();
		left_num_max_len = std::max(left_num_max_len, (int)lasers[laser_idx].size());
	}

	// begin printing board

	// print numbers of top lasers: top_num_max_len lines
	for (int i = 0; i < top_num_max_len; ++i)
	{
		// left margin of size left_num_max_len
		for (int c = 0; c < left_num_max_len; ++c)
			out << ' ';

		// for each column: print ith digit (or space) of top laser's number when right-aligned to top_num_max_len digits
		for (int col = 0; col < n; ++col)
		{
			out << ' '; // column separator
			int const laser_idx = UpDir * n + col;
			int leading_spaces = top_num_max_len - (int)lasers[laser_idx].size();
			// print actual digit or space
			if (i < leading_spaces)
				out << ' ';
			else
				out << lasers[laser_idx][i - leading_spaces];
		}

		out << '\n';
	}

	// print n lines, one per board row
	for (int row = 0; row < n; ++row)
	{
		// line starts with left laser's number
		{
			int const laser_idx = LeftDir * n + row;
			int leading_spaces = left_num_max_len - (int)lasers[laser_idx].size();
			for (int c = 0; c < leading_spaces; ++c)
				out << ' ';
			out << lasers[laser_idx];
		}

		// for each column: print cell's contents
		for (int col = 0; col < n; ++col)
		{
			out << ' '; // column separator
			CellType cell = board.cell(row, col);
			switch (cell)
			{
			case CellType::Empty:
				out << ' ';
				break;
			case CellType::ForwardMirror:
				out << '/';
				break;
			case CellType::BackwardMirror:
				out << '\\';
				break;
			case CellType::LaserBeam:
				out << '*';
				break;
			}
		}
		out << ' '; // column separator

		// line ends with right laser's number
		{
			int const laser_idx = RightDir * n + row;
			out << lasers[laser_idx];
		}

		out << '\n';
	}

	// print numbers of bottom lasers: bottom_num_max_len lines
	for (int i = 0; i < bottom_num_max_len; ++i)
	{
		// left margin of size left_num_max_len
		for (int c = 0; c < left_num_max_len; ++c)
			out << ' ';

		// for each column: print ith digit (or space) of bottom laser's number when left-aligned to bottom_num_max_len digits
		for (int col = 0; col < n; ++col)
		{
			out << ' '; // column separator
			int const laser_idx = DownDir * n + col;
			if (i < (int)lasers[laser_idx].size())
				out << lasers[laser_idx][i];
			else
				out << ' ';
		}

		out << '\n';
	}

	return out;
}

class LaserPathsVisitor
{
public:
	// When called, a full path from start to some other laser is applied. The end laser has its number updated.
	// return value: true if visiting should be continued
	using Callback = std::function<bool(unsigned int path_product)>;

	LaserPathsVisitor(Board & board, int start_laser_section_idx, int start_laser_offset, Callback const & callback):
		board(board),
		callback(callback),
		start_pos(board.laser_section_and_offset_to_pos(start_laser_section_idx, start_laser_offset))
	{
		Direction const start_dir = opposite_direction(Direction(start_laser_section_idx));

		int const start_laser_idx = start_laser_section_idx * n + start_laser_offset;

		// with start_pos (laser) marked as "has path", do recursive visiting
		assert(!board.get_laser_has_path()[start_laser_idx]);
		board.get_laser_has_path()[start_laser_idx] = true;
		rec_visit(start_pos, start_dir, 1, board.laser(start_pos));
		board.get_laser_has_path()[start_laser_idx] = false;
	}

private:
	// path_product: product of segment lengths already on path
	// needed_product: if non-zero then remaining segments' product must be equal to it
	bool rec_visit(Pos const cur_pos, Direction const cur_dir, unsigned int path_product, unsigned int needed_product)
	{
		// try increasing segment lengths, until we hit a laser or a mirror
		bool cont = true;
		for (int segment_length = 1; cont; ++segment_length)
		{
			Pos const end_pos = cur_pos + direction_to_vec[cur_dir] * segment_length;

			if (!board.is_on_board(end_pos) || board.cell(end_pos) == CellType::ForwardMirror
					|| board.cell(end_pos) == CellType::BackwardMirror)
				cont = false; // segment cannot get any longer

			if (needed_product % segment_length == 0)
			{
				// copy segment's row or column, so that we can mark it with LaserBeam and then restore it
				CellType to_restore[n];
				if (cur_dir == RightDir || cur_dir == LeftDir)
				{
					// copy row
					for (int i = 0; i < n; ++i)
						to_restore[i] = board.cell(cur_pos.row, i);
					// mark laser segment with LaserBeam
					int const left_col = std::min(cur_pos.col, end_pos.col);
					int const right_col = std::max(cur_pos.col, end_pos.col);
					for (int i = left_col + 1; i < right_col; ++i)
						board.cell(cur_pos.row, i) = CellType::LaserBeam;
				}
				else
				{
					// copy column
					for (int i = 0; i < n; ++i)
						to_restore[i] = board.cell(i, cur_pos.col);
					// mark laser segment with LaserBeam
					int const up_row = std::min(cur_pos.row, end_pos.row);
					int const down_row = std::max(cur_pos.row, end_pos.row);
					for (int i = up_row + 1; i < down_row; ++i)
						board.cell(i, cur_pos.col) = CellType::LaserBeam;
				}

				bool visit_more = true;
				do // fake loop for visit_more (in case we need a break)
				{
					unsigned int new_needed_product = needed_product / segment_length;
					unsigned int new_path_product = path_product * segment_length;
					if (!board.is_on_board(end_pos))
					{
						// end_pos is a laser
						auto [end_laser_section_idx, end_laser_offset] = board.get_laser_section_and_offset(end_pos);
						int const end_laser_idx = end_laser_section_idx * n + end_laser_offset;

						unsigned int const end_laser_num = board.get_lasers()[end_laser_idx];
						if (new_needed_product <= 1 && (end_laser_num == 0 || end_laser_num == new_path_product))
						{
							board.get_lasers()[end_laser_idx] = new_path_product;
							assert(!board.get_laser_has_path()[end_laser_idx]);
							board.get_laser_has_path()[end_laser_idx] = true;
							unsigned int const start_laser_num = board.laser(start_pos);
							board.laser(start_pos) = new_path_product;

							visit_more = callback(new_path_product);

							board.laser(start_pos) = start_laser_num;
							board.get_laser_has_path()[end_laser_idx] = false;
							board.get_lasers()[end_laser_idx] = end_laser_num;
						}
					}
					else
					{
						// end_pos is on board
						CellType const end_pos_type = board.cell(end_pos);
						// We cannot put a mirror on a cell that has a laser beam!
						if (end_pos_type != CellType::LaserBeam)
						{
							// adjacent cells cannot have a mirror
							bool found_adjacent_mirror = false;
							for (Pos const dir : all_dirs)
							{
								Pos const neighbor = end_pos + dir;
								if (board.is_on_board(neighbor) && (board.cell(neighbor) == CellType::ForwardMirror
											|| board.cell(neighbor) == CellType::BackwardMirror))
								{
									found_adjacent_mirror = true;
									break;
								}
							}
							if (!found_adjacent_mirror)
							{
								for (CellType new_mirror : {CellType::ForwardMirror, CellType::BackwardMirror})
								{
									if (end_pos_type == CellType::Empty || end_pos_type == new_mirror)
									{
										board.cell(end_pos) = new_mirror;
										Direction const new_dir = new_mirror == CellType::ForwardMirror ?
											dir_after_forward_mirror[cur_dir] : dir_after_backward_mirror[cur_dir];
										visit_more = rec_visit(end_pos, new_dir, new_path_product, new_needed_product);
										board.cell(end_pos) = end_pos_type;

										if (!visit_more)
											break;
									}
								}
							}
						}
					}
				} while (0);

				// restore to the point before we marked segment with LaserBeam
				if (cur_dir == RightDir || cur_dir == LeftDir)
				{
					// restore row
					for (int i = 0; i < n; ++i)
						board.cell(cur_pos.row, i) = to_restore[i];
				}
				else
				{
					// restore column
					for (int i = 0; i < n; ++i)
						board.cell(i, cur_pos.col) = to_restore[i];
				}

				if (!visit_more)
					return false;
			} // if (needed_product % segment_length == 0)
		}
		return true;
	}

	Board & board;
	Callback const callback;
	Pos const start_pos;
};

class MirrorsSolver
{
public:
	using Callback = std::function<void(Board const &)>;

	MirrorsSolver(Board const & board, Callback const & callback):
		board(board),
		callback(callback)
	{
		rec_solve();
	}

private:
	void rec_solve()
	{
		unsigned int min_count_possible_paths = std::numeric_limits<unsigned int>::max();
		int best_laser_section_idx, best_laser_offset;

		// Two rounds here, going over lasers without a path.
		// First off, go over lasers with non-zero hint (as they are likely to have lower possible paths count).
		for (int hint_non_zero = 1; hint_non_zero >= 0; --hint_non_zero)
		{
			for (int laser_section_idx = 0; laser_section_idx < 4; ++laser_section_idx)
			{
				for (int laser_offset = 0; laser_offset < n; ++laser_offset)
				{
					int const laser_idx = laser_section_idx * n + laser_offset;
					if (!board.get_laser_has_path()[laser_idx] && hint_non_zero == !!board.get_lasers()[laser_idx])
					{
						unsigned int count = 0;
						LaserPathsVisitor visitor(board, laser_section_idx, laser_offset,
								[&](unsigned int /*path_product*/)
								{
									++count;
									return count < min_count_possible_paths;
								});
						if (count < min_count_possible_paths)
						{
							min_count_possible_paths = count;
							best_laser_section_idx = laser_section_idx;
							best_laser_offset = laser_offset;
						}
					}
				}
			}
		}

		if (min_count_possible_paths == std::numeric_limits<unsigned int>::max())
		{
			// All lasers have a path.
			callback(board);
		}
		else
		{
			// Recursively try all paths from the selected laser (with the lowest count of possible paths).
			LaserPathsVisitor visitor(board, best_laser_section_idx, best_laser_offset,
					[this](unsigned int /*path_product*/)
					{
						rec_solve();
						return true;
					});
		}
	}

	Board board;
	Callback const callback;
};

void print_answer(Board const & orig_board, Board const & solved_board)
{
	std::cout << "\nFound solution:\n" << solved_board << std::flush;

	unsigned long long prod = 1;
	for (int laser_section_idx = 0; laser_section_idx < 4; ++laser_section_idx)
	{
		unsigned int sum = 0;
		for (int laser_offset = 0; laser_offset < n; ++laser_offset)
		{
			int const laser_idx = laser_section_idx * n + laser_offset;
			if (orig_board.get_lasers()[laser_idx] == 0)
			{
				sum += solved_board.get_lasers()[laser_idx];
			}
		}
		std::cout << "side sum: " << sum << '\n';
		prod *= sum;
	}

	std::cout << "final answer: " << prod << std::endl;
}

int main()
{
	std::cout << "Enter n: ";
	int n_;
	std::cin >> n_;
	if (n_ != n)
	{
		std::cerr << "program was compiled for n=" << n << '\n';
		return 1;
	}

	Board board;

	// go over top lasers
	std::cout << "Enter numbers of top lasers: ";
	for (int col = 0; col < n; ++col)
	{
		int const laser_idx = UpDir * n + col;
		std::cin >> board.get_lasers()[laser_idx];
	}

	// go over right lasers
	std::cout << "Enter numbers of right lasers: ";
	for (int row = 0; row < n; ++row)
	{
		int const laser_idx = RightDir * n + row;
		std::cin >> board.get_lasers()[laser_idx];
	}

	// go over bottom lasers
	std::cout << "Enter numbers of bottom lasers: ";
	for (int col = 0; col < n; ++col)
	{
		int const laser_idx = DownDir * n + col;
		std::cin >> board.get_lasers()[laser_idx];
	}

	// go over left lasers
	std::cout << "Enter numbers of left lasers: ";
	for (int row = 0; row < n; ++row)
	{
		int const laser_idx = LeftDir * n + row;
		std::cin >> board.get_lasers()[laser_idx];
	}

	std::cout << "Board:\n" << board;
	std::cout << "Solving..." << std::endl;

	MirrorsSolver solver(board,
			[&](Board const & solved_board)
			{
				print_answer(board, solved_board);
			});
}
