
See ["Counting polybar polyominoes"](https://quuxplusone.github.io/blog/2025/08/23/polybars/) (2025-08-23).

A000105(n)=1, 1, 2, 5, 12, 35, 108, 369, 1285, 4655, 17073, 63600, ...
is the number of connected free regions that can be built out of n free monominoes. These are the "free polyominoes" or "free polybars of order 1."

A056785(n)=1, 4, 23, 211, 2227, 25824, 310242, 3818983, 47752136, ...
is the number of connected free regions that can be built out of n free dominoes. These are the "free polydominoes" or "free polybars of order 2."

a(n)=1, 6, 55, 833, 14378, 269710, 5221076, ...
is the number of connected free regions that can be built out of n free I-trominoes. These are the "free polybars of order 3."

There are six dibars of order 3:

         XXX  XXX     XXXX    X
    XXX   XXX XXX        X XXXX
      XXX         XXXXXX X    X

There are 55 tribars of order 3:

    ......X ......X .....X .....X ....X ....X ....XXX ...X... X...X
    ......X XXXXXXX ..XXXX XXX..X .XXXX XXX.X ..XXX.. ...X... XXXXX
    XXXXXXX ......X XXX..X ..XXXX XXX.X .XXXX XXX.... XXXXXXX X...X

    ...X... ...X ...X... ...XX ...XX ...XXX ...XXXX ..XXX X...X XXX
    ...XXXX XXXX XXXXXXX ...XX XXXXX ..XXX. ...X... XXX.. X...X XXX
    XXXX... XXXX ...X... XXXXX ...XX XXX... XXXX... ..XXX XXXXX XXX

    .....X .....X ....X. ....X ....X ....X ....X ....X ....X ...X..
    .....X .....X ....X. ....X ....X ...XX ...XX X...X XXXXX ...X..
    .....X ..XXXX ....X. .XXXX XXX.X ...XX XXXXX XXXXX ...XX XXXX..
    XXXXXX XXX... XXXXXX XXX.. ..XXX XXXX. ...X. X.... ...X. ...XXX

    ....X ....X ....X ....X ...X ..X ..X ..X ..XXX.. ..XXX. ...XXX
    ....X ....X ....X ....X ...X ..X .XX X.X XXX.XXX XXXXXX XXXXXX
    ....X ...XX XXXXX XXXXX X..X XXX XXX XXX
    ..XXX XXXX. ...X. X.... XXXX XX. XX. XX. .....XXX ....XXX
    XXX.. ...X. ...X. X.... X... XX. X.. .X. XXXXXX.. XXXXXX.

    ...X ...X ...X ...X. ...X ...X ...X ...X. ...X ..X .X. X.X
    ...X ...X ...X ...X. ...X ...X X..X XXXX. XXXX XXX XXX X.X
    ...X ...X XXXX XXXX. XXXX XXXX X..X ...X. ...X XXX XXX X.X
    XXXX XXXX .X.. ..XXX .XXX XXX. XXXX ..XXX .XXX XX. X.X XXX
    ...X X... .X..
    ...X X... .X.. XXXXXXXXX

The total count of $$n$$-bars of order $$p$$ is:

          n=1   2    3      4        5         6          7          8         9         10
     monobars  tribars       pentabars             heptabars             nonabars
           dibars   tetrabars            hexabars              octabars             decabars

    p=1:    1   1    2      5       12         35        108        369      1285       4655
      2:    1   4   23    211     2227      25824     310242    3818983  47752136  604425323
      3:    1   6   55    833    14378     269710    5221076  103352306
      4:    1   7   93   1973    47161    1204744   31711028  852689953
      5:    1   9  144   3913   118842    3851349  128587175
      6:    1  10  204   6809   252797    9951844  403553930
      7:    1  12  277  10938   478377   22178331
      8:    1  13  359  16427   830085   44325651
      9:    1  15  454  23577  1348694   81527962
     10:    1  16  558  32491  2079909  140441453

On my laptop computing these numbers took this many seconds (CPU-bound for small numbers,
becoming RAM/swap-bound for the larger numbers). "-" means less than one second:

          n=1  2  3  4   5      6       7       8      9      10
    p=1:    -  -  -  -   -      -       -       -      -       -
      2:    -  -  -  -   -      -       2      32    538   18847
      3:    -  -  -  -   -      2      57    1563      .       .
      4:    -  -  -  -   -     14     570  151296      .       .
      5:    -  -  -  -   2     69    3380       .      .       .
      6:    -  -  -  -   4    248   52502       .      .       .
      7:    -  -  -  -  11    746       .       .      .       .
      8:    -  -  -  -  23   1934       .       .      .       .
      9:    -  -  -  -  48   4463       .       .      .       .
     10:    -  -  -  1  90  12394       .       .      .       .

---

`count-completesets.cpp` counts the 11 hexominoes buildable from a complete set of trominoes,
or the 3721336 icosominoes buildable from a complete set of tetrominoes.

`count-polybars.cpp` counts the animals buildable from $$n$$ bars of order $$p$$.
Compile it with `-DBARSIZE=p -DBARSPER=n` for some $$p$$ and $$n$$. It will print out
not only the number of $$n$$-bars of order $$p$$, but also the number of
$$n-1$$-bars, $$n-2$$-bars, etc., as a side effect.
