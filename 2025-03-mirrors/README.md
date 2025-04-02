Solution to [Hall of Mirrors 3](https://www.janestreet.com/puzzles/hall-of-mirrors-3-index/) puzzle.

Build in debug config (to enable assertions, they can sometimes detect bugs):
```
cmake -DCMAKE_BUILD_TYPE:STRING=Debug .
make
```

It finds solution almost immediately:
```
$ cd 2025-03-mirrors/
$ time ./mirrors < board.in 
Enter n: Created board with side 10
Enter numbers of top lasers: Enter numbers of right lasers: Enter numbers of bottom lasers: Enter numbers of left lasers: Board:
              3        
        1     0        
        1   4 8        
        2   8 7 9     1
                        
                        4
                        27
 27                     
                        
                        
                        16
 12                     
225                     
                        
    2     1 6 5   4    
    0     2 4     0    
    2             5    
    5                  
Solving...

Found solution:
               3        
         1     0   4    
       1 1 5 4 8   0    
     3 2 2 6 8 7 9 5 4 1
  27 * * \ * * * \ * * \ 1
2025 * * * * * * * * \ * 4
   3 / * * * / * * / * * 27
  27 * * / * * * * * * \ 9
 112 * * * * * * / * * * 64
  12 * / * * \ * * \ * * 27
  48 / * \ * * / * * / * 16
  12 * * * \ * * * * * * 56
 225 * * * * / * / * / * 4
  24 / * * * * / * * * * 5
     2 2 2 1 6 5 1 4 4 3
     0 2 4 2 4   6 0   0
     2 5           5   8
     5                 7
side sum: 480
side sum: 166
side sum: 3356
side sum: 2251
final answer: 601931086080

real    0m0.023s
user    0m0.014s
sys     0m0.004s
```
