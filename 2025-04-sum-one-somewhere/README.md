This is a solution to [Sum One, Somewhere](https://www.janestreet.com/puzzles/sum-one-somewhere-index/) puzzle.

Let:
* p - probability that label is 0
* P(A): probability that there exists an infinite path that sums to 0
* P(B): probability that there exists an infinite path with sum <= 1

Then:
* P(A) = P(0 at root) * P(path with 0 sum at left child OR path with 0 sum at right child)
* P(B) = P(root==0) * P(B|root==0) + P(root==1) * P(B|root==1)
* P(B) = p * P(B at left child OR B at right child) + (1-p) * P(A at left child OR A at right child)

We can derive formulas for P(A) and P(B) in Maxima. This is for P(A):
```
$ maxima

...
(%i1) batch("solution.maxima");

read and interpret .../solution.maxima
(%i2) A = p*(1-(1-A)^2)
                                             2
(%o2)                        A = (1 - (1 - A) ) p
(%i3) a_sol:solve(%,A)
                                  2 p - 1
(%o3)                        [A = -------, A = 0]
                                     p
```
Then for P(B):
```
(%i4) B = p*(1-(1-B)^2)+(1-p)*(1-(1-A)^2)
                                 2                  2
(%o4)            B = (1 - (1 - B) ) p + (1 - (1 - A) ) (1 - p)
(%i5) ev(eq:%,a_sol[1])
                                    2 p - 1 2                2
(%o5)         B = (1 - p) (1 - (1 - -------) ) + (1 - (1 - B) ) p
                                       p
(%i6) declare(p,mainvar)
(%o6)                                done
(%i7) ev(lhs(eq)-rhs(eq),ratsimp)
                       2         3            2
                     (B  - 2 B) p  + (B + 2) p  - 3 p + 1
(%o7)                ------------------------------------
                                       2
                                      p
(%i8) ev(lhs(eq)-rhs(eq),B = 1/2,ratsimp)
                                3       2
                             3 p  - 10 p  + 12 p - 4
(%o8)                      - -----------------------
                                         2
                                      4 p
```
Finally we get a cubic polynomial:
```
(%i9) ev((lhs(eq)-rhs(eq))*4*p^2,B = 1/2,ratsimp)
...
                                    3        2
(%o10)                f(p) := (- 3 p ) + 10 p  - 12 p + 4
```
For which we can find the solution using integer arithmetic by defining helper g(n) function:
```
(%i12) f(n/10^10)
                          3                            2
                       3 n                            n                3 n
(%o12) (- -------------------------------) + -------------------- - ----------
          1000000000000000000000000000000    10000000000000000000   2500000000
                                                                            + 4
(%i13) ev(f(n/10^10)*10^30,ratsimp)
...
                     3                  2
(%o14) g(n) := (- 3 n ) + 100000000000 n  - 1200000000000000000000 n
                                              + 4000000000000000000000000000000
```
We find first 10 decimal places of p using bisection:
```
(%i15) pred:g(c) >= 0
...
(%i19) last_true:block([a:1,b:10^10,c],
                       while a < b do
                             (c:floor((a+b)/2),
                              if ev(pred) then a:c+1 else b:c),a-1)
(%o19)                            5306035754
```
It turns out Maxima has a built-in function [realroots](https://maxima.sourceforge.io/docs/manual/Equations.html#index-realroots) that uses
exact rational arithmetic and computes roots with given tolerance. It can also find the solution, but one has to make sure to set tolerance
low enough so that 10 decimal places are accurate. 10^-11 is good in this case, but if 11th decimal digit was 0 or 9, then we would have to
use even lower tolerance.
```
(%i26) ev(realroots(f(p),1.0E-11),numer)
(%o26)                      [p = 0.530603575429268]
```
