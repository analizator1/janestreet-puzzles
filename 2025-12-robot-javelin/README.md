This is a solution to [Robot Javelin](https://www.janestreet.com/puzzles/robot-javelin-index/) puzzle.

```
$ maxima

Maxima 5.46.0 https://maxima.sourceforge.io
using Lisp GNU Common Lisp (GCL) GCL 2.6.14 git tag Version_2_6_15pre7
Distributed under the GNU Public License. See the file COPYING.
Dedicated to the memory of William Schelter.
The function bug_report() provides bug reporting information.
(%i1) batch("solution.maxima");
read and interpret /home/krzysztof/moje/programy/janestreet/puzzles/2025-12-robot-javelin/solution.maxima
(%i2) m(a,b):=max(b-a,0)
(%o2)                      m(a, b) := max(b - a, 0)
(%i3) p_A_less_B:apply("+",makelist(concat(part,i),i,8))
(%o3)    part8 + part7 + part6 + part5 + part4 + part3 + part2 + part1
(%i4) part1:(1/2)*m(dx,da)*dh
                              max(0, da - dx) dh
(%o4)                         ------------------
                                      2
(%i5) part2:(1/2)*(1-max(dx,da))^2*dh
                                              2
                             (1 - max(da, dx))  dh
(%o5)                        ---------------------
                                       2
(%i6) part3:(1/2)*(1-dh^2)*m(dx,da)
                                                  2
                           max(0, da - dx) (1 - dh )
(%o6)                      -------------------------
                                       2
(%i7) part4:(1/2)*((1-max(dx,da))^2-m(max(dx,da),dh)^2)
                                 2      2
                (1 - max(da, dx))  - max (0, dh - max(da, dx))
(%o7)           ----------------------------------------------
                                      2
(%i8) part5:(1/2)*min(dx,da)*dl
                                min(da, dx) dl
(%o8)                           --------------
                                      2
(%i9) part6:(1/2)*m(da,dx)*(2-da-dx)*dl
                     max(0, dx - da) dl ((- dx) - da + 2)
(%o9)                ------------------------------------
                                      2
(%i10) part7:(1/2)*(1-dl^2)*min(da,dx)
                                                2
                             min(da, dx) (1 - dl )
(%o10)                       ---------------------
                                       2
(%i11) part8:(1/2)*(m(da,dx)*(2-da-dx)-m(da,min(dl,dx))*(m(dx,dl)+m(da,dl)))
(%o11) (max(0, dx - da) ((- dx) - da + 2)
              - max(0, min(dl, dx) - da) (max(0, dl - dx) + max(0, dl - da)))/2
(%i12) p_A_less_B:ev(p_A_less_B)
       max(0, dx - da) dl ((- dx) - da + 2)
(%o12) ------------------------------------
                        2
 + (max(0, dx - da) ((- dx) - da + 2) - max(0, min(dl, dx) - da)
                                                             2
                                          min(da, dx) (1 - dl )
 (max(0, dl - dx) + max(0, dl - da)))/2 + ---------------------
                                                    2
                                           2                     2
   min(da, dx) dl   max(0, da - dx) (1 - dh )   (1 - max(da, dx))  dh
 + -------------- + ------------------------- + ---------------------
         2                      2                         2
                                         2      2
   max(0, da - dx) dh   (1 - max(da, dx))  - max (0, dh - max(da, dx))
 + ------------------ + ----------------------------------------------
           2                                  2

...

Final solution for Java-lin: [da = 0.5833333333333334,
                   dx = 0.6180339887498949, dh = 0.6909830056250526, dl = 0.5]
Probability for Spears Robot is: 0.5060629096353509
                                   3/2
                               12 5    - 229
Probability for Java-lin is: - ------------- which equals: 0.4939370903646491
                                    192
Probability for Java-lin rounded to 10 places is: 0.4939370904
Assumptions satisfied: true
-------
(%i58) is(prob_javalin_rounded = 0.4939370904)
(%o58)                               true
```
