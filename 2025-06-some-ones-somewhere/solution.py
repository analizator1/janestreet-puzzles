import argparse
from enum import Enum
import sys

class Color(Enum):
	CYAN      = 0
	BLUE      = 1
	DARK_BLUE = 2
	YELLOW    = 3
	ORANGE    = 4
	RED       = 5
	GREEN     = 6
	BROWN     = 7


class Point:
	def __init__(self, row, col):
		self.row = row
		self.col = col

	def __eq__(self, other):
		if not isinstance(other, Point):
			return False
		return self.row == other.row and self.col == other.col

	def __add__(self, other):
		return Point(self.row + other.row, self.col + other.col)


class Square:
	def __init__(self, color, row, col):
		self.color = color
		self.side = colorToSide(color)
		self.topLeft = Point(row, col)  # relative to current board

	def isCovering(self, point):
		return (self.top() <= point.row <= self.bottom() and
			self.left() <= point.col <= self.right())

	def getCorners(self):
		return [
			self.topLeft,
			Point(self.top(), self.right()),
			Point(self.bottom(), self.right()),
			Point(self.bottom(), self.left()),
		]

	def left(self):
		return self.topLeft.col

	def top(self):
		return self.topLeft.row

	def right(self):
		return self.topLeft.col + self.side - 1

	def bottom(self):
		return self.topLeft.row + self.side - 1


def colorToSide(color):
	match color:
		case Color.CYAN:
			return 9
		case Color.BLUE:
			return 6
		case Color.DARK_BLUE:
			return 4
		case Color.YELLOW:
			return 7
		case Color.ORANGE:
			return 3
		case Color.RED:
			return 5
		case Color.GREEN:
			return 2
		case Color.BROWN:
			return 8
	assert False


def colorToAnsiBasicCode(color):
	match color:
		case Color.CYAN:
			return 96
		case Color.BLUE:
			return 94
		case Color.DARK_BLUE:
			return 34
		case Color.YELLOW:
			return 33
		case Color.ORANGE:
			return 35
		case Color.RED:
			return 31
		case Color.GREEN:
			return 32
		case Color.BROWN:
			return 90
	assert False


def _colorToChr(color):
	if color == Color.BROWN:
		return "X"
	return color.name[0]


def squarePointToString(square, point):
	color = square.color
	isSide = (point.row == square.top() or point.row == square.bottom() or
		   point.col == square.left() or point.col == square.right())
	char = _colorToChr(color)
	char = char.lower()
	if not isSide:
		char = "*"
	if args.color:
		ansiCode = colorToAnsiBasicCode(color)
		return f"\33[{ansiCode}m{char}\33[0m"
	return char


board00 = [
	Square(Color.DARK_BLUE, 0, 11),
	Square(Color.ORANGE, 4, 12),
	Square(Color.CYAN, 0, 15),
	Square(Color.CYAN, 0, 24),
	Square(Color.DARK_BLUE, 0, 33),
	Square(Color.DARK_BLUE, 4, 33),
	Square(Color.BROWN, 0, 37),

	Square(Color.CYAN, 9, 0),
	Square(Color.CYAN, 18, 0),
	Square(Color.CYAN, 27, 0),
	Square(Color.CYAN, 36, 0),

	Square(Color.BLUE, 13, 9),
	Square(Color.RED, 19, 9),
	Square(Color.RED, 24, 9),
	Square(Color.BROWN, 29, 9),
	Square(Color.BROWN, 37, 9),

	Square(Color.RED, 8, 33),
	Square(Color.YELLOW, 8, 38),
	Square(Color.YELLOW, 15, 38),
	Square(Color.CYAN, 13, 29),
	Square(Color.BROWN, 22, 37),
	Square(Color.BROWN, 30, 37),
	Square(Color.BROWN, 30, 29),
	Square(Color.YELLOW, 38, 38),
	Square(Color.YELLOW, 38, 31),
]

board01 = [
	Square(Color.DARK_BLUE, 0, 0),
	Square(Color.DARK_BLUE, 0, 4),
	Square(Color.DARK_BLUE, 0, 8),
	Square(Color.YELLOW, 0, 12),
	Square(Color.YELLOW, 0, 19),
	Square(Color.BLUE, 0, 26),
	Square(Color.BROWN, 0, 32),
	Square(Color.RED, 0, 40),
	Square(Color.RED, 5, 40),
	Square(Color.CYAN, 4, 0),
	Square(Color.ORANGE, 4, 9),
	Square(Color.BROWN, 13, 0),
	Square(Color.BROWN, 13, 8),
	Square(Color.CYAN, 10, 36),
	Square(Color.CYAN, 19, 36),
	Square(Color.CYAN, 19, 27),
	Square(Color.BROWN, 28, 21),
	Square(Color.BROWN, 28, 29),
	Square(Color.BROWN, 28, 37),
	Square(Color.ORANGE, 25, 21),
	Square(Color.RED, 24, 16),
	Square(Color.YELLOW, 29, 0),
	Square(Color.YELLOW, 29, 7),
	Square(Color.YELLOW, 29, 14),
	Square(Color.CYAN, 36, 0),
	Square(Color.CYAN, 36, 9),
	Square(Color.CYAN, 36, 18),
	Square(Color.CYAN, 36, 27),
	Square(Color.CYAN, 36, 36),
]

board02 = [
	Square(Color.CYAN, 0, 0),
	Square(Color.CYAN, 9, 0),
	Square(Color.CYAN, 18, 0),
	Square(Color.CYAN, 27, 0),
	Square(Color.CYAN, 36, 0),

]

board10 = []
board11 = []
board12 = []

board20 = []
board21 = []
board22 = []

all_boards = [
	[board00, board01, board02],
	[board10, board11, board12],
	[board20, board21, board22],
]

BOARD_SIZE = 45

parser = argparse.ArgumentParser(description="""Solution to Some Ones, Somewhere.""")
parser.add_argument("--color", help="print raw ansi color sequences", action="store_true")

args = parser.parse_args()

for overallRow in range(3 * BOARD_SIZE):
	r = overallRow // BOARD_SIZE
	row = overallRow % BOARD_SIZE

	if row == 0:
		if r == 0:
			# print top legend
			for overallCol in range(3 * BOARD_SIZE):
				c = overallCol // BOARD_SIZE
				col = overallCol % BOARD_SIZE

				if col == 0:
					if c == 0:
						sys.stdout.write(" ")
					else:
						sys.stdout.write(" |")  # vertical separator

				colChr = chr(65 + overallCol % 26)
				sys.stdout.write(" " + colChr)
			sys.stdout.write("\n")
		else:
			# print horizontal separator
			for overallCol in range(3 * BOARD_SIZE):
				c = overallCol // BOARD_SIZE
				col = overallCol % BOARD_SIZE

				if col == 0:
					if c == 0:
						sys.stdout.write(" ")
					else:
						sys.stdout.write("=|")  # vertical separator

				sys.stdout.write("==")
			sys.stdout.write("\n")

	# print ordinary row
	for overallCol in range(3 * BOARD_SIZE):
		c = overallCol // BOARD_SIZE
		col = overallCol % BOARD_SIZE
		point = Point(row, col)

		if col == 0:
			if c == 0:
				# print left legend
				rowChr = chr(65 + overallRow % 26)
				sys.stdout.write(rowChr)
			else:
				sys.stdout.write(" |")  # vertical separator

		board = all_boards[r][c]
		covering_squares = [square for square in board if square.isCovering(point)]
		assert len(covering_squares) <= 1

		# print the color of covering square
		if covering_squares:
			square = covering_squares[0]
			sys.stdout.write(" " + squarePointToString(square, point))
		else:
			sys.stdout.write("  ")
	sys.stdout.write("\n")
