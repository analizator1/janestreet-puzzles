#include <iostream>
#include <memory>
#include <cstdint>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>
#include <functional>

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

static Pos const dir_up    = {-1, 0};
static Pos const dir_right = {0, 1};
static Pos const dir_down  = {1, 0};
static Pos const dir_left  = {0, -1};

static Pos const all_dirs[4] = {dir_up, dir_right, dir_down, dir_left};

enum class CellType: uint8_t
{
	Empty = 0,
	ForwardMirror,
	BackwardMirror
};

enum LaserSectionIdx
{
	TopLaserSection,
	RightLaserSection,
	BottomLaserSection,
	LeftLaserSection
};

static Pos const laser_section_idx_to_start_dir[4] = {
	dir_down, // TopLaserSection
	dir_left, // RightLaserSection
	dir_up,   // BottomLaserSection
	dir_right // LeftLaserSection
};

class Board
{
public:
	Board():
		cells(new CellType[n * n]{}),
		lasers(new unsigned int[4 * n]{})
	{
		std::cout << "Created board with side " << n << std::endl;
	}

	Board(Board const & other):
		cells(new CellType[n * n]),
		lasers(new unsigned int[4 * n])
	{
		std::copy(&other.cells[0], &other.cells[n * n], &this->cells[0]);
		std::copy(&other.lasers[0], &other.lasers[4 * n], &this->lasers[0]);
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

	unsigned int * get_lasers() const
	{
		return lasers.get();
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
			laser_section_idx = TopLaserSection;
			laser_offset = col;
		}
		else if (col == n) // right
		{
			laser_section_idx = RightLaserSection;
			laser_offset = row;
		}
		else if (row == n) // bottom
		{
			laser_section_idx = BottomLaserSection;
			laser_offset = col;
		}
		else if (col == -1) // left
		{
			laser_section_idx = LeftLaserSection;
			laser_offset = row;
		}

		assert(laser_section_idx >= 0 && laser_section_idx < 4);
		assert(laser_offset >= 0 && laser_offset < n);
		return {laser_section_idx, laser_offset};
	}

	Pos laser_section_and_offset_to_pos(int laser_section_idx, int laser_offset) const
	{
		switch (laser_section_idx)
		{
		case TopLaserSection:
			return {-1, laser_offset};
		case RightLaserSection:
			return {laser_offset, n};
		case BottomLaserSection:
			return {n, laser_offset};
		case LeftLaserSection:
			return {laser_offset, -1};
		}
		assert(false);
	}

	unsigned int & laser(int row, int col)
	{
		auto [laser_section_idx, laser_offset] = get_laser_section_and_offset(row, col);
		return lasers[laser_section_idx * n + laser_offset];
	}

	unsigned int laser(int row, int col) const
	{
		auto [laser_section_idx, laser_offset] = get_laser_section_and_offset(row, col);
		return lasers[laser_section_idx * n + laser_offset];
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
		int idx = TopLaserSection * n + col;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		top_num_max_len = std::max(top_num_max_len, (int)lasers[idx].size());
	}

	// go over right lasers
	for (int row = 0; row < n; ++row)
	{
		int idx = RightLaserSection * n + row;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		right_num_max_len = std::max(right_num_max_len, (int)lasers[idx].size());
	}

	// go over bottom lasers
	for (int col = 0; col < n; ++col)
	{
		int idx = BottomLaserSection * n + col;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		bottom_num_max_len = std::max(bottom_num_max_len, (int)lasers[idx].size());
	}

	// go over left lasers
	for (int row = 0; row < n; ++row)
	{
		int idx = LeftLaserSection * n + row;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		left_num_max_len = std::max(left_num_max_len, (int)lasers[idx].size());
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
			int idx = TopLaserSection * n + col;
			int leading_spaces = top_num_max_len - (int)lasers[idx].size();
			// print actual digit or space
			if (i < leading_spaces)
				out << ' ';
			else
				out << lasers[idx][i - leading_spaces];
		}

		out << '\n';
	}

	// print n lines, one per board row
	for (int row = 0; row < n; ++row)
	{
		// line starts with left laser's number
		{
			int idx = LeftLaserSection * n + row;
			int leading_spaces = left_num_max_len - (int)lasers[idx].size();
			for (int c = 0; c < leading_spaces; ++c)
				out << ' ';
			out << lasers[idx];
		}

		// for each column: print cell's contents
		for (int col = 0; col < n; ++col)
		{
			out << ' '; // column separator
			CellType cell = board.cell(row, col);
			switch (cell)
			{
			case CellType::Empty:
				out << '*';
				break;
			case CellType::ForwardMirror:
				out << '/';
				break;
			case CellType::BackwardMirror:
				out << '\\';
				break;
			}
		}
		out << ' '; // column separator

		// line ends with right laser's number
		{
			int idx = RightLaserSection * n + row;
			out << lasers[idx];
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
			int idx = BottomLaserSection * n + col;
			if (i < (int)lasers[idx].size())
				out << lasers[idx][i];
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
	using Callback = std::function<bool(Board const &, unsigned int path_product)>;

	LaserPathsVisitor(Board const & board, Callback const & callback, int laser_section_idx, int laser_offset):
		board(board),
		callback(callback)
	{
		Pos const start_pos = board.laser_section_and_offset_to_pos(laser_section_idx, laser_offset);
		Pos const start_dir = laser_section_idx_to_start_dir[laser_section_idx];
		rec_visit(start_pos, start_dir, 1, board.laser(start_pos));
	}

private:
	// path_product: product of segment lengths already on path
	// needed_product: if non-zero then remaining segments' product must be equal to it
	bool rec_visit(Pos const cur_pos, Pos const cur_dir, unsigned int path_product, unsigned int needed_product)
	{
		bool cont = true;
		for (int segment_length = 1; cont; ++segment_length)
		{
			// TODO:
			// 1. end_pos = cur_pos + cur_dir * segment_length.
			// 2. if end_pos is a laser or if it contains a mirror: cont = false.
			// 3. if segment_length divides needed_product:
			//   3.1. if end_pos is a laser and laser num matches: with laser num updated { callback() }
			//     3.1. if callback returned false then return false
			//   3.2. else (end_pos is not a laser) then: for each matching mirror setting:
			//     3.2.1 with mirror updated { rec_visit() }
			//       3.2.1.1 if rec_visit returned false then return false
		}
	}

	Board board;
	Callback const callback;
};

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
		int idx = TopLaserSection * n + col;
		std::cin >> board.get_lasers()[idx];
	}

	// go over right lasers
	std::cout << "Enter numbers of right lasers: ";
	for (int row = 0; row < n; ++row)
	{
		int idx = RightLaserSection * n + row;
		std::cin >> board.get_lasers()[idx];
	}

	// go over bottom lasers
	std::cout << "Enter numbers of bottom lasers: ";
	for (int col = 0; col < n; ++col)
	{
		int idx = BottomLaserSection * n + col;
		std::cin >> board.get_lasers()[idx];
	}

	// go over left lasers
	std::cout << "Enter numbers of left lasers: ";
	for (int row = 0; row < n; ++row)
	{
		int idx = LeftLaserSection * n + row;
		std::cin >> board.get_lasers()[idx];
	}

	std::cout << "Board:\n" << board;
	std::cout << "Solving..." << std::endl;
}
