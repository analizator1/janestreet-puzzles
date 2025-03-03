This is a solution to [Somewhat Square Sudoku](https://www.janestreet.com/puzzles/somewhat-square-sudoku-index/) puzzle.

To build everything, run in repo root:
```
cmake -DCMAKE_BUILD_TYPE:STRING=Release .
make
```

Performance of the first version: it finished in less than 4h.
```
$ time 2025-01-sudoku/sudoku
...
=== best solution ===
best middle row: 283950617
best gcd: 12345679
best unused digit: 4
    0   1   2   3   4   5   6   7   8  
  +---+---+---+---+---+---+---+---+---+
0 | 3 | 9 | 5 | 0 | 6 | 1 | 7 | 2 | 8 |
  +---+---+---+---+---+---+---+---+---+
1 | 0 | 6 | 1 | 7 | 2 | 8 | 3 | 9 | 5 |
  +---+---+---+---+---+---+---+---+---+
2 | 7 | 2 | 8 | 3 | 9 | 5 | 0 | 6 | 1 |
  +---+---+---+---+---+---+---+---+---+
3 | 9 | 5 | 0 | 6 | 1 | 7 | 2 | 8 | 3 |
  +---+---+---+---+---+---+---+---+---+
4 | 2 | 8 | 3 | 9 | 5 | 0 | 6 | 1 | 7 |
  +---+---+---+---+---+---+---+---+---+
5 | 6 | 1 | 7 | 2 | 8 | 3 | 9 | 5 | 0 |
  +---+---+---+---+---+---+---+---+---+
6 | 8 | 3 | 9 | 5 | 0 | 6 | 1 | 7 | 2 |
  +---+---+---+---+---+---+---+---+---+
7 | 5 | 0 | 6 | 1 | 7 | 2 | 8 | 3 | 9 |
  +---+---+---+---+---+---+---+---+---+
8 | 1 | 7 | 2 | 8 | 3 | 9 | 5 | 0 | 6 |
  +---+---+---+---+---+---+---+---+---+

real    232m14.966s
user    232m14.950s
sys     0m0.014s
$ 
```
