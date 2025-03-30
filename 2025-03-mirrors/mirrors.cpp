#include <iostream>
#include <memory>
#include <cstdint>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>

static constexpr int n = 5; // side of field

enum class CellType: uint8_t
{
	Empty = 0,
	ForwardMirror,
	BackwardMirror
};

enum LasersRowIdx
{
	top_idx,
	right_idx,
	bottom_idx,
	left_idx
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

	CellType & cell(int row, int col)
	{
		assert(row >= 0 && row < n);
		assert(col >= 0 && col < n);
		return cells[row * n + col];
	}

	CellType cell(int row, int col) const
	{
		assert(row >= 0 && row < n);
		assert(col >= 0 && col < n);
		return cells[row * n + col];
	}

	unsigned int * get_lasers() const
	{
		return lasers.get();
	}

	unsigned int & laser(int row, int col)
	{
		if (row == -1) // top
		{
			assert(col >= 0 && col < n);
			return lasers[top_idx * n + col];
		}
		if (col == n) // right
		{
			assert(row >= 0 && row < n);
			return lasers[right_idx * n + row];
		}
		if (row == n) // bottom
		{
			assert(col >= 0 && col < n);
			return lasers[bottom_idx * n + col];
		}
		if (col == -1) // left
		{
			assert(row >= 0 && row < n);
			return lasers[left_idx * n + row];
		}
		assert(false);
	}

	unsigned int laser(int row, int col) const
	{
		if (row == -1) // top
		{
			assert(col >= 0 && col < n);
			return lasers[top_idx * n + col];
		}
		if (col == n) // right
		{
			assert(row >= 0 && row < n);
			return lasers[right_idx * n + row];
		}
		if (row == n) // bottom
		{
			assert(col >= 0 && col < n);
			return lasers[bottom_idx * n + col];
		}
		if (col == -1) // left
		{
			assert(row >= 0 && row < n);
			return lasers[left_idx * n + row];
		}
		assert(false);
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

static bool is_on_board(int row, int col)
{
	return (row >= 0 && row < n) && (col >= 0 && col < n);
}

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
		int idx = top_idx * n + col;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		top_num_max_len = std::max(top_num_max_len, (int)lasers[idx].size());
	}

	// go over right lasers
	for (int row = 0; row < n; ++row)
	{
		int idx = right_idx * n + row;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		right_num_max_len = std::max(right_num_max_len, (int)lasers[idx].size());
	}

	// go over bottom lasers
	for (int col = 0; col < n; ++col)
	{
		int idx = bottom_idx * n + col;
		std::ostringstream ostr;
		if (board.get_lasers()[idx])
			ostr << board.get_lasers()[idx];
		lasers[idx] = ostr.str();
		bottom_num_max_len = std::max(bottom_num_max_len, (int)lasers[idx].size());
	}

	// go over left lasers
	for (int row = 0; row < n; ++row)
	{
		int idx = left_idx * n + row;
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
			int idx = top_idx * n + col;
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
			int idx = left_idx * n + row;
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
			int idx = right_idx * n + row;
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
			int idx = bottom_idx * n + col;
			if (i < (int)lasers[idx].size())
				out << lasers[idx][i];
			else
				out << ' ';
		}

		out << '\n';
	}

	return out;
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
		int idx = top_idx * n + col;
		std::cin >> board.get_lasers()[idx];
	}

	// go over right lasers
	std::cout << "Enter numbers of right lasers: ";
	for (int row = 0; row < n; ++row)
	{
		int idx = right_idx * n + row;
		std::cin >> board.get_lasers()[idx];
	}

	// go over bottom lasers
	std::cout << "Enter numbers of bottom lasers: ";
	for (int col = 0; col < n; ++col)
	{
		int idx = bottom_idx * n + col;
		std::cin >> board.get_lasers()[idx];
	}

	// go over left lasers
	std::cout << "Enter numbers of left lasers: ";
	for (int row = 0; row < n; ++row)
	{
		int idx = left_idx * n + row;
		std::cin >> board.get_lasers()[idx];
	}

	std::cout << "Board:\n" << board;
	std::cout << "Solving..." << std::endl;
}
