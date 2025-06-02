Solution to [Number Cross 5](https://www.janestreet.com/puzzles/number-cross-5-index/) puzzle.

For speed, this should be compiled in release config:
```
cmake -DCMAKE_BUILD_TYPE:STRING=Release .
```

I ran it in the morning:
```
krzysztof@krzysztof-Latitude-5531:~/moje/programy/janestreet/puzzles(master)$ make && time 2025-05-number-cross5/number_cross < 2025-05-number-cross5/board.in > 2025-05-number-cross5/board.out
[ 15%] Built target sudoku
[ 30%] Built target mirrors
[ 61%] Built target number_cross
[ 76%] Built target dynamic_bitset_test
[100%] Built target hints_test
```

In the evening it was still running, but it already found the solution. Though it was on June 2, so one day too late to submit.
```
krzysztof@krzysztof-Latitude-5531:~/moje/programy/janestreet/puzzles(master)$ grep -A 17 -m 1 "Found solution" 2025-05-number-cross5/board.out
Found solution:
Grid:
 0 + 0 3 4 2 2 2 5 0 3 2 4
 1 + 5 4 0 2 2 5 0 2 5 2 0
 2 + 0 5 3 4 3 0 6 5 0 2 6
 3 + 7 3 6 0 3 2 0 5 4 4 0
 4 + 0 5 5 5 0 2 2 0 8 1 6
 5 + 5 5 0 5 5 1 0 1 5 5 0
 6 + 0 3 6 7 4 4 1 2 0 1 5
 7 + 7 3 7 0 9 6 9 0 9 9 0
 8 + 3 4 0 4 6 3 6 8 0 8 9
 9 + 5 3 5 9 3 0 5 9 9 5 0
10 + 0 4 7 0 8 8 7 0 4 3 3

Region values: 0=>2 1=>4 2=>3 3=>1 4=>4 5=>6 6=>7 7=>3 8=>7
Numbers: 887 47 225 544 22 252 54 3674412 34 5343 65 26 55 46368 736 32 555 816 433 551 155 15 324 737 342225 969 99 89 53593 5995
Sum: 4135658

```
