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
		self.topLeft = Point(row, col)  # relative to board
		self.gridTopLeft = None  # top-left point relative to grid origin

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

	def gridLeft(self):
		return self.gridTopLeft.col

	def gridTop(self):
		return self.gridTopLeft.row

	def gridRight(self):
		return self.gridTopLeft.col + self.side - 1

	def gridBottom(self):
		return self.gridTopLeft.row + self.side - 1


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


def sorted_unique(iterable, key):
	d = {key(item): item for item in iterable}
	return sorted(d.values(), key=key)


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

	Square(Color.BROWN, 0, 9),
	Square(Color.BROWN, 8, 9),
	Square(Color.YELLOW, 16, 9),
	Square(Color.YELLOW, 9, 17),
	Square(Color.CYAN, 0, 17),
	Square(Color.RED, 0, 26),
	Square(Color.DARK_BLUE, 5, 26),
	Square(Color.GREEN, 5, 30),
	Square(Color.CYAN, 0, 36),

	Square(Color.YELLOW, 38, 9),
	Square(Color.BLUE, 39, 16),
	Square(Color.BLUE, 39, 22),
	Square(Color.YELLOW, 32, 16),
	Square(Color.ORANGE, 29, 20),
	Square(Color.RED, 29, 23),
	Square(Color.RED, 34, 23),
	Square(Color.CYAN, 36, 28),
	Square(Color.CYAN, 27, 28),
	Square(Color.ORANGE, 24, 28),
	Square(Color.BLUE, 21, 31),
	Square(Color.BLUE, 15, 31),
	Square(Color.DARK_BLUE, 41, 41),
	Square(Color.DARK_BLUE, 41, 37),
	Square(Color.BROWN, 33, 37),
	Square(Color.BROWN, 25, 37),
	Square(Color.BROWN, 17, 37),
]

board10 = [
	Square(Color.CYAN, 0, 0),
	Square(Color.CYAN, 0, 9),
	Square(Color.BLUE, 0, 18),
	Square(Color.BLUE, 6, 18),
	Square(Color.BROWN, 9, 0),
	Square(Color.YELLOW, 9, 8),
	Square(Color.ORANGE, 9, 15),
	Square(Color.RED, 17, 0),
	Square(Color.RED, 22, 0),
	Square(Color.ORANGE, 17, 5),
	Square(Color.DARK_BLUE, 16, 8),
	Square(Color.YELLOW, 20, 5),
	Square(Color.CYAN, 27, 0),
	Square(Color.CYAN, 36, 0),
	Square(Color.CYAN, 36, 9),
	Square(Color.CYAN, 36, 18),

	Square(Color.YELLOW, 0, 31),
	Square(Color.YELLOW, 0, 38),
	Square(Color.BROWN, 7, 37),
	Square(Color.BROWN, 15, 37),
	Square(Color.BROWN, 23, 37),
	Square(Color.BROWN, 31, 37),
	Square(Color.BLUE, 39, 39),
	Square(Color.BLUE, 39, 33),
	Square(Color.DARK_BLUE, 35, 33),
	Square(Color.GREEN, 33, 35),
	Square(Color.RED, 28, 32),
	Square(Color.CYAN, 19, 28),
	Square(Color.DARK_BLUE, 15, 33),
]

board11 = [
	Square(Color.DARK_BLUE, 2, 0),
	Square(Color.YELLOW, 6, 0),
	Square(Color.YELLOW, 13, 0),
	Square(Color.BROWN, 20, 0),
	Square(Color.BROWN, 28, 0),
	Square(Color.CYAN, 36, 0),
	Square(Color.RED, 6, 7),
	Square(Color.RED, 6, 12),
	Square(Color.CYAN, 11, 7),
	Square(Color.BROWN, 20, 8),
	Square(Color.BROWN, 28, 8),
	Square(Color.BROWN, 28, 16),
	Square(Color.CYAN, 36, 9),

	Square(Color.CYAN, 0, 36),
	Square(Color.YELLOW, 0, 29),
	Square(Color.YELLOW, 0, 22),
	Square(Color.RED, 7, 31),
	Square(Color.ORANGE, 12, 42),
	Square(Color.BLUE, 15, 39),
	Square(Color.BROWN, 21, 37),
	Square(Color.YELLOW, 29, 38),
	Square(Color.YELLOW, 29, 31),
	Square(Color.DARK_BLUE, 25, 33),
	Square(Color.CYAN, 36, 36),
]

board12 = [
	Square(Color.CYAN, 0, 0),
	Square(Color.CYAN, 9, 0),
	Square(Color.CYAN, 18, 0),
	Square(Color.CYAN, 27, 0),
	Square(Color.CYAN, 36, 0),

	Square(Color.DARK_BLUE, 0, 9),
	Square(Color.DARK_BLUE, 0, 13),
	Square(Color.YELLOW, 0, 17),
	Square(Color.YELLOW, 0, 24),
	Square(Color.YELLOW, 0, 31),
	Square(Color.YELLOW, 0, 38),
	Square(Color.BLUE, 7, 39),
	Square(Color.BLUE, 7, 33),
	Square(Color.BROWN, 7, 25),
	Square(Color.CYAN, 13, 36),
	Square(Color.BLUE, 22, 39),
	Square(Color.YELLOW, 22, 32),
	Square(Color.GREEN, 20, 34),
	Square(Color.BLUE, 28, 39),
	Square(Color.BLUE, 34, 39),
	Square(Color.RED, 40, 40),
	Square(Color.RED, 40, 35),

	Square(Color.BROWN, 37, 9),
	Square(Color.BROWN, 29, 9),
	Square(Color.ORANGE, 26, 9),
	Square(Color.YELLOW, 29, 17),
	Square(Color.CYAN, 36, 17),
]

board20 = [
	Square(Color.CYAN, 0, 0),
	Square(Color.CYAN, 9, 0),
	Square(Color.CYAN, 18, 0),
	Square(Color.CYAN, 27, 0),
	Square(Color.CYAN, 36, 0),

	Square(Color.YELLOW, 0, 9),
	Square(Color.BROWN, 7, 9),
	Square(Color.YELLOW, 15, 9),
	Square(Color.YELLOW, 15, 16),
	Square(Color.ORANGE, 12, 17),
	Square(Color.YELLOW, 22, 9),
	Square(Color.BROWN, 29, 9),
	Square(Color.BROWN, 37, 9),
	Square(Color.YELLOW, 29, 17),
	Square(Color.CYAN, 36, 17),
	Square(Color.DARK_BLUE, 36, 26),
	Square(Color.RED, 40, 26),
	Square(Color.RED, 40, 31),
	Square(Color.GREEN, 38, 34),
	Square(Color.CYAN, 36, 36),
	Square(Color.BROWN, 28, 37),
	Square(Color.BROWN, 20, 37),
	Square(Color.DARK_BLUE, 16, 41),

	Square(Color.BROWN, 0, 37),
	Square(Color.CYAN, 0, 28),
	Square(Color.BLUE, 0, 22),
]

board21 = [
	Square(Color.CYAN, 0, 0),
	Square(Color.CYAN, 9, 0),
	Square(Color.CYAN, 18, 0),
	Square(Color.CYAN, 27, 0),

	Square(Color.CYAN, 0, 9),
	Square(Color.CYAN, 0, 18),
	Square(Color.YELLOW, 9, 9),
	Square(Color.YELLOW, 16, 9),
	Square(Color.BLUE, 9, 16),
	Square(Color.BLUE, 15, 16),
	Square(Color.BROWN, 21, 16),
	Square(Color.RED, 9, 22),
	Square(Color.YELLOW, 14, 22),

	Square(Color.DARK_BLUE, 0, 27),
	Square(Color.RED, 0, 31),
	Square(Color.CYAN, 0, 36),
	Square(Color.GREEN, 5, 34),
	Square(Color.YELLOW, 9, 38),
	Square(Color.YELLOW, 16, 38),
	Square(Color.BROWN, 23, 37),

	Square(Color.ORANGE, 34, 42),
	Square(Color.BROWN, 37, 37),
	Square(Color.BROWN, 37, 29),
	Square(Color.DARK_BLUE, 41, 25),
]

board22 = [
	Square(Color.CYAN, 36, 0),
	Square(Color.CYAN, 36, 9),
	Square(Color.CYAN, 36, 18),
	Square(Color.CYAN, 36, 27),
	Square(Color.CYAN, 36, 36),

	Square(Color.CYAN, 27, 36),
	Square(Color.CYAN, 18, 36),
	Square(Color.RED, 31, 31),
	Square(Color.RED, 26, 31),
	Square(Color.RED, 21, 31),
	Square(Color.YELLOW, 21, 24),
	Square(Color.BROWN, 28, 23),
	Square(Color.BROWN, 28, 15),
	Square(Color.BROWN, 28, 7),
	Square(Color.DARK_BLUE, 24, 20),
	Square(Color.ORANGE, 25, 7),
	Square(Color.YELLOW, 29, 0),
	Square(Color.YELLOW, 22, 0),

	Square(Color.BLUE, 0, 0),
	Square(Color.BLUE, 0, 6),
	Square(Color.BLUE, 0, 12),
	Square(Color.RED, 0, 18),
	Square(Color.DARK_BLUE, 0, 23),
	Square(Color.BROWN, 6, 0),
	Square(Color.BROWN, 6, 8),
]

all_boards = [
	[board00, board01, board02],
	[board10, board11, board12],
	[board20, board21, board22],
]

BOARD_SIZE = 45

for r in range(3):
	for c in range(3):
		boardPos = Point(BOARD_SIZE * r, BOARD_SIZE * c)
		for square in all_boards[r][c]:
			square.gridTopLeft = square.topLeft + boardPos

all_boards_list = [board for row in all_boards for board in row]
all_squares_list = [square for board in all_boards_list for square in board]

colorsSortedBySide = sorted(Color, key=colorToSide)

parser = argparse.ArgumentParser(description="""Solution to Some Ones, Somewhere.""")
parser.add_argument("--color", help="print raw ansi color sequences", action="store_true")

args = parser.parse_args()


def printGrid():
	for gridRow in range(3 * BOARD_SIZE):
		r = gridRow // BOARD_SIZE
		row = gridRow % BOARD_SIZE

		if row == 0:
			# print separator row (with column legend)
			sys.stdout.write("   ")
			for gridCol in range(3 * BOARD_SIZE):
				c = gridCol // BOARD_SIZE
				col = gridCol % BOARD_SIZE

				# vertical separation
				if col == 0:
					sys.stdout.write("  ")

				colChr = chr(65 + gridCol % 26)
				sys.stdout.write(" " + colChr)
			sys.stdout.write("\n")

		# print real row
		sys.stdout.write(f"{gridRow:3}")
		for gridCol in range(3 * BOARD_SIZE):
			c = gridCol // BOARD_SIZE
			col = gridCol % BOARD_SIZE
			point = Point(row, col)

			# vertical separation (with row legend)
			if col == 0:
				rowChr = chr(65 + gridRow % 26)
				sys.stdout.write(" " + rowChr)

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


def printRowLettersFromSquares(printForLeft, printForRight, colorsToConsider):
	print("\n=====")
	print("printRowLettersFromSquares:", "printForLeft:", printForLeft, "printForRight:", printForRight,
	   "colorsToConsider:", [color.name for color in colorsToConsider])
	for gridRow in range(3 * BOARD_SIZE):
		r = gridRow // BOARD_SIZE
		row = gridRow % BOARD_SIZE

		rowChr = chr(65 + gridRow % 26)
		print("row", gridRow, rowChr, end=": ")
		for gridCol in range(3 * BOARD_SIZE):
			c = gridCol // BOARD_SIZE
			col = gridCol % BOARD_SIZE
			point = Point(row, col)

			board = all_boards[r][c]
			covering_squares = [square for square in board if square.isCovering(point)]
			assert len(covering_squares) <= 1

			if covering_squares:
				square = covering_squares[0]
				if printForLeft and col == square.left() and square.color in colorsToConsider:
					print(chr(65 + square.gridLeft() % 26), end="")
				if printForRight and col == square.right() and square.color in colorsToConsider:
					print(chr(65 + square.gridRight() % 26), end="")

		print()


def printRowLettersFromSquaresForGrid():
	for numSmallestColors in range(1, len(Color)+1):
		colorsToConsider = colorsSortedBySide[:numSmallestColors]
		printRowLettersFromSquares(True, True, colorsToConsider)
		printRowLettersFromSquares(True, False, colorsToConsider)
		printRowLettersFromSquares(False, True, colorsToConsider)


def tryToDecodeSquares():
	print("\n=== (top, left) numeric ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		for r in range(3):
			for c in range(3):
				squares = sorted([square for square in all_boards[r][c] if square.color == color], key=lambda square: (square.top(), square.left()))
				print(f"board{r}{c} [{len(squares)} squares]:", end="")
				for square in squares:
					print(f" ({square.top()}, {square.left()})", end="")
				print()

	print("\n=== top ===")
	# For each color: go over each board and print location of squares in this color as letters.
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		for r in range(3):
			for c in range(3):
				print(f"board{r}{c}: ", end="")
				squares = sorted([square for square in all_boards[r][c] if square.color == color], key=lambda square: (square.top(), square.left()))
				for square in squares:
					print(chr(65 + square.gridTop() % 26), end="")
				print()

	print("\n=== left ===")
	# For each color: go over each board and print location of squares in this color as letters.
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		for r in range(3):
			for c in range(3):
				print(f"board{r}{c}: ", end="")
				# Note: sort by (left, top).
				squares = sorted([square for square in all_boards[r][c] if square.color == color], key=lambda square: (square.left(), square.top()))
				for square in squares:
					print(chr(65 + square.gridLeft() % 26), end="")
				print()

	print("\n=== top,left ===")
	# For each color: go over each board and print location of squares in this color as letters.
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		for r in range(3):
			for c in range(3):
				print(f"board{r}{c}: ", end="")
				squares = sorted([square for square in all_boards[r][c] if square.color == color], key=lambda square: (square.top(), square.left()))
				for square in squares:
					print(chr(65 + square.gridTop() % 26) + chr(65 + square.gridLeft() % 26), end="")
				print()

	print("\n=== bottom,left ===")
	# For each color: go over each board and print location of squares in this color as letters.
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		for r in range(3):
			for c in range(3):
				print(f"board{r}{c}: ", end="")
				squares = sorted([square for square in all_boards[r][c] if square.color == color], key=lambda square: (square.top(), square.left()))
				for square in squares:
					print(chr(65 + square.gridBottom() % 26) + chr(65 + square.gridLeft() % 26), end="")
				print()

	print("\n=== (gridTop, gridLeft) numeric, sorted over whole grid ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		squares = sorted([square for square in all_squares_list if square.color == color], key=lambda square: (square.gridTop(), square.gridLeft()))
		print(f"all boards [{len(squares)} squares]:", end="")
		for square in squares:
			print(f" ({square.gridTop()}, {square.gridLeft()})", end="")
		print()

	print("\n=== gridTop,gridLeft, sorted over whole grid ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		squares = sorted([square for square in all_squares_list if square.color == color], key=lambda square: (square.gridTop(), square.gridLeft()))
		for square in squares:
			print(chr(65 + square.gridTop() % 26) + chr(65 + square.gridLeft() % 26), end="")
		print()

	print("\n=== gridTop, sorted over whole grid by gridTop ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		squares = sorted([square for square in all_squares_list if square.color == color], key=lambda square: square.gridTop())
		for square in squares:
			print(chr(65 + square.gridTop() % 26), end="")
		print()

	print("\n=== gridTop, sorted_unique over whole grid by gridTop ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		squares = sorted_unique([square for square in all_squares_list if square.color == color], key=lambda square: square.gridTop())
		for square in squares:
			print(chr(65 + square.gridTop() % 26), end="")
		print()

	print("\n=== gridLeft, sorted over whole grid by gridLeft ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		# Note: sort by (gridLeft, gridTop).
		squares = sorted([square for square in all_squares_list if square.color == color], key=lambda square: square.gridLeft())
		for square in squares:
			print(chr(65 + square.gridLeft() % 26), end="")
		print()

	print("\n=== gridLeft, sorted_unique over whole grid by gridLeft ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		# Note: sort by (gridLeft, gridTop).
		squares = sorted_unique([square for square in all_squares_list if square.color == color], key=lambda square: square.gridLeft())
		for square in squares:
			print(chr(65 + square.gridLeft() % 26), end="")
		print()

	print("\n=== gridTop, sorted over whole grid by gridLeft ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		squares = sorted([square for square in all_squares_list if square.color == color], key=lambda square: (square.gridLeft(), square.gridTop()))
		for square in squares:
			print(chr(65 + square.gridTop() % 26), end="")
		print()

	print("\n=== gridLeft, sorted over whole grid by gridTop ===")
	for color in colorsSortedBySide:
		print(color.name, "squares:")
		print(f"all boards: ", end="")
		# Note: sort by (gridLeft, gridTop).
		squares = sorted([square for square in all_squares_list if square.color == color], key=lambda square: (square.gridTop(), square.gridLeft()))
		for square in squares:
			print(chr(65 + square.gridLeft() % 26), end="")
		print()

	print("\n=== gridTop,gridLeft, sorted over whole grid, sets of smallest colors ===")
	for numSmallestColors in range(1, len(Color)+1):
		colorsToConsider = colorsSortedBySide[:numSmallestColors]
		print([color.name for color in colorsToConsider], "squares:")
		print(f"all boards: ", end="")
		squares = sorted([square for square in all_squares_list if square.color in colorsToConsider], key=lambda square: (square.gridTop(), square.gridLeft()))
		for square in squares:
			print(chr(65 + square.gridTop() % 26) + chr(65 + square.gridLeft() % 26), end="")
		print()

	print("\n=== gridTop,gridLeft, sorted over whole grid, sets of smallest colors, rot13 ===")
	for numSmallestColors in range(1, len(Color)+1):
		colorsToConsider = colorsSortedBySide[:numSmallestColors]
		print([color.name for color in colorsToConsider], "squares:")
		print(f"all boards: ", end="")
		squares = sorted([square for square in all_squares_list if square.color in colorsToConsider], key=lambda square: (square.gridTop(), square.gridLeft()))
		for square in squares:
			print(chr(65 + (square.gridTop() + 13) % 26) + chr(65 + (square.gridLeft() + 13) % 26), end="")
		print()

	print("\n=== top,left, sets of smallest colors ===")
	# For each color: go over each board and print location of squares in this color as letters.
	for numSmallestColors in range(1, len(Color)+1):
		colorsToConsider = colorsSortedBySide[:numSmallestColors]
		print([color.name for color in colorsToConsider], "squares:")
		for r in range(3):
			for c in range(3):
				print(f"board{r}{c}: ", end="")
				squares = sorted([square for square in all_boards[r][c] if square.color in colorsToConsider], key=lambda square: (square.top(), square.left()))
				for square in squares:
					print(chr(65 + square.gridTop() % 26) + chr(65 + square.gridLeft() % 26), end="")
				print()

	print("\n=== top,left, sets of smallest colors, rot13 ===")
	# For each color: go over each board and print location of squares in this color as letters.
	for numSmallestColors in range(1, len(Color)+1):
		colorsToConsider = colorsSortedBySide[:numSmallestColors]
		print([color.name for color in colorsToConsider], "squares:")
		for r in range(3):
			for c in range(3):
				print(f"board{r}{c}: ", end="")
				squares = sorted([square for square in all_boards[r][c] if square.color in colorsToConsider], key=lambda square: (square.top(), square.left()))
				for square in squares:
					print(chr(65 + (square.gridTop() + 13) % 26) + chr(65 + (square.gridLeft() + 13) % 26), end="")
				print()


printGrid()
tryToDecodeSquares()
printRowLettersFromSquaresForGrid()
