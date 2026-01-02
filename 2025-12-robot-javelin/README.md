This is a solution to [Robot Javelin](https://www.janestreet.com/puzzles/robot-javelin-index/) puzzle.

```
$ maxima

Maxima 5.46.0 https://maxima.sourceforge.io
using Lisp GNU Common Lisp (GCL) GCL 2.6.14 git tag Version_2_6_15pre7
Distributed under the GNU Public License. See the file COPYING.
Dedicated to the memory of William Schelter.
The function bug_report() provides bug reporting information.
(%i1) batch("solution.maxima");

read and interpret .../solution.maxima
(%i2) m2(a,b):=m_(b-a)
(%o2)                        m2(a, b) := m_(b - a)
(%i3) m_(x):=if x >= 0 then x else 0
(%o3)                  m_(x) := if x >= 0 then x else 0
(%i4) p_A_less_B:apply("+",makelist(concat(part,i),i,8))
(%o4)    part8 + part7 + part6 + part5 + part4 + part3 + part2 + part1
(%i5) part1:(1/2)*m2(dx,da)*dh
                   dh (if da - dx >= 0 then da - dx else 0)
(%o5)              ----------------------------------------
                                      2
...
(%i58) (sol:cons(opt_var_javalin,opt_vars_spears),
        print("Final solution for Java-lin:",ev(sol,numer)),
        val0:ev(ev(ev(p_A_less_B),sol),numer),
        val1_exact:ev(ev(1-p_A_less_B),sol),
        print("Probability value for Spears Robot is:",val0),
        print("Probability for Java-lin is:",ratsimp(ev(val1_exact))),
        print("Probability value for Java-lin is:",ev(val1_exact,numer)),
        print("Assumptions satisfied:",
              ev(is(dl < da and da <= dx and dx < dh),sol)),print("-------"))
Final solution for Java-lin: [da = 0.5833333333333333, 
                   dx = 0.6180339887498951, dh = 0.6909830056250519, dl = 0.5] 
Probability value for Spears Robot is: 0.5060629096353509 
                                             7/2  3/2
Probability for Java-lin is: ((199540348477 2    5    - 17853039629605 sqrt(2))
                           3/2
 sqrt(10) + 7147658302786 5    - 79939082829648)
           31/2  7/2            39/2                           3/2
/((118239 2     5    - 2087445 2    ) sqrt(10) + 437768945664 5
 - 4946148655104) 
Probability value for Java-lin is: 0.4939370903646492 
Assumptions satisfied: true 
------- 
(%i59) final_prob_javalin_rounded:ev('round(val1_exact*10^10)/10^10,numer)
(%i60) print("Probability value for Java-lin rounded to 10 places is:",
             final_prob_javalin_rounded)
Probability value for Java-lin rounded to 10 places is: 0.4939370904 
(%i61) is(final_prob_javalin_rounded = 0.4939370904)
(%o61)                               true
```
