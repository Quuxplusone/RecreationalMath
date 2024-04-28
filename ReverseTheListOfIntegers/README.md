This directory relates to "Reverse the List of Integers":

- https://mathstodon.xyz/@two_star/112242224494626411
- https://news.ycombinator.com/item?id=40010066
- https://quuxplusone.github.io/blog/2024/04/22/reverse-the-list/

----

`nksolver.cpp` computes the following values:

Compile "nksolver.cpp" with `-DN=10 -DK=5` (or whatever) to compute the values
- C(n,k) = number of solvable lists
- M(n,k) = worst-case number of moves to solve any list
- L(n,k) = worst-case intermediate list length to optimally solve any list
of length `N` and initial list length `K`.

That is, exactly `C` such lists can be solved at all; each of those `C` lists has a
no-more-than-`M`-move solution; and each of those lists has a no-more-than-`M`-move
solution where each intermediate position's list-length never exceeds `L`.

The count $$C(n,k)$$ of solvable lists with high value $$n$$ and length $$k$$ is:
(Entries suffixed with `?` come from Tomas Rokicki but have not been independently verified by my slower code.)

    n=1:  1
    n=2:  1  0
    n=3:  1  0    0
    n=4:  1  0    0     0
    n=5:  1  0    0     0      0
    n=6:  1  8   26    12      0      0
    n=7:  1 12   82   294    244      0        0
    n=8:  1 14  126   760   2316   1846        0         0
    n=9:  1 16  168  1344   8238  31678    47164         0         0
    n=10: 1 18  216  2016  15098  89078   336154    480598      2640         0
    n=11: 1 20  270  2880  25200 181308  1039174?  3907420?  5673092?        0  0
    n=12: 1 22  330  3960  39600 332582? 2323524? 12999906? 47886678? 67645030? 0 0
    n=13: 1 24  396  5280  59400      ?        ?         ?         ?         ?  ? ? 0
    n=14: 1 26  468  6864  85800      ?        ?         ?         ?         ?  ? ? ? 0
    n=15: 1 28  546  8736 120120      ?        ?         ?         ?         ?  ? ? ? 0 0
    n=16: 1 30  630 10920      ?      ?        ?         ?         ?         ?  ? ? ? ? 0 0
    n=17: 1 32  720 13440      ?      ?        ?         ?         ?         ?  ? ? ? ? ? ? 0
    n=18: 1 34  816 16320      ?      ?        ?         ?         ?         ?  ? ? ? ? ? ? ? 0
    n=19: 1 36  918 19584      ?      ?        ?         ?         ?         ?  ? ? ? ? ? ? ? 0 0
    n=20: 1 38 1026     ?      ?      ?        ?         ?         ?         ?  ? ? ? ? ? ? ? ? 0 0

The worst-case number of moves $$M(n,k)$$ to solve any solvable list with high value $$n$$ and length $$k$$ is:
(Entries suffixed with `?` come from Tomas Rokicki but have not been independently verified by my slower code.
The row-sums of this triangle are OEIS [A372008](https://oeis.org/A372008).)

    n=1:  0
    n=2:  0 -
    n=3:  0 - -
    n=4:  0 - -  -
    n=5:  0 - -  -   -
    n=6:  0 6 14  6  -  -
    n=7:  0 4 12 24 26  -    -
    n=8:  0 4 14 32 64 74    -    -
    n=9:  0 4  8 18 66 76   86    -    -
    n=10: 0 4  8 14 20 88  124  126   36    -
    n=11: 0 4  8 12 16 26   90? 100? 106?   -  -
    n=12: 0 4  8 12 16 20?  34? 112? 128? 130? - -
    n=13: 0 4  8 10 14
    n=14: 0 4  8 12 16
    n=15: 0 4  8 10 14
    n=16: 0 4  8 12
    n=17: 0 4  8 10
    n=18: 0 4  8 12
    n=19: 0 4  8 10
    n=20: 0 4  8

The sufficient intermediate list length $$L(n,k)$$ for solvable lists with high value $$n$$ and length $$k$$ is:

    n=1:  1
    n=2:  1 -
    n=3:  1 - -
    n=4:  1 - - -
    n=5:  1 - - - -
    n=6:  1 4 4 4 - -
    n=7:  1 4 5 5 5 - -
    n=8:  1 4 5 6 7 7 - -
    n=9:  1 4 5 7 7 8 8 - -
    n=10: 1 4 5 7 8 9 9 9 9 -
    n=11: 1 4 5 7 8 9 ? ? ? - -
    n=12: 1 4 5 6 8 ? ? ? ? ? - -
    n=13: 1 4 5 7 8 ? ? ? ? ? ? ? -
    n=14: 1 4 5 6 7 ? ? ? ? ? ? ? ? -
    n=15: 1 4 5 7 8 ? ? ? ? ? ? ? ? - -
    n=16: 1 4 5 6 ? ? ? ? ? ? ? ? ? ? - -
    n=17: 1 4 5 7 ? ? ? ? ? ? ? ? ? ? ? ? -
    n=18: 1 4 5 6 ? ? ? ? ? ? ? ? ? ? ? ? ? -
    n=19: 1 4 5 7 ? ? ? ? ? ? ? ? ? ? ? ? ? - -
    n=20: 1 4 5 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? - -

----

`nssolver.cpp` computes the following values:

Compile "nssolver.cpp" with `-DN=10 -DS=5` (or whatever) to compute the values
- C_s(n,s) = number of solvable lists
- M_s(n,s) = worst-case number of moves to solve any list
of length `N` and invariant list sum `S`.

That is, exactly `C_s` such lists can be solved at all; and each of those `C_s` lists has a
no-more-than-`M_s`-move solution.

This doesn't make a pretty triangle, because $$S$$ grows as $$N(N+1) / 2$$.
Also, $$C_s(n,s) = 0$$ for all $$s\leq n$$. But we can just show the values
for which $$C_s(n,s)$$ is nonzero, starting with $$S = n+1$$:

    -- C_s --
    n=6,  S=7:  - - 6 8 10 20
    n=7,  S=8:  2 2 8 8 14 22 42 60 68 104 126 148 28
    n=8,  S=9:  2 2 8 8 14 34 44 64 90 224 180 324 414 526  772    -  724 1084    -    -  548
    n=9,  S=10: 2 2 8 8 14 38 44 68 96 230 254 396 508 742 1414 1584 2112 2946 3508 4112  8326  7764 10032  9404 10300  6800 17896
    n=10, S=11: 2 2 8 8 14 38 44 68 98 236 264 426 546 802 1636 1826 2610 3558 4890 5750 11868 12270 17944 22112 25650 30542 39406  70810  72846  99346  84720  95074  91028  86604 136724   6048
    n=11, S=12: 2 2 8 8 14 38 44 68 98 242 270 432 576 834 1690 2048 2892 3948 5484 7132 13504 14858 21722 27692 39236 44002 59996 102412 114934 153726 198580 226600 268760 340126 364552 684588 ...
    n=12, S=13: 2 2 8 8 14 38 44 68 98 242 272 438 582 864 1720 2106 3062 4164 5790 7610 14898 16544 24288 31536 44150 55390 73334 123380 144174 196550 254990 ...

    -- M_s --
    n=6,  S=7:  - - 6 14  6  6
    n=7,  S=8:  4 4 8 10 12  8 12 18 18 18 24 26  6
    n=8,  S=9:  4 4 8  8 14 10 10 14 12 14 18 22 24 28 40  - 40 36  -  - 74
    n=9,  S=10: 4 4 6  6  8  8  8 12 10 12 14 14 18 18 20 24 24 30 24 36 46 48 52 74 72 58 86
    n=10, S=11: 4 4 6  6  6 10  8 12 10 10 12 12 14 14 14 18 16 18 20 22 24 30 34 36 36 36 34 42 44 48 74 88 82 126 76 38
    n=11, S=12: 4 4 6  6  6  8  8  8  8 12 12 10 12 12 12 14 16 16 18 18 18 26 22 26 24 26 28 36 34 34 36 36 44  44 54 48 ...
    n=12, S=13: 4 4 6  6  6  8  8  8  8 10 12 10 12 12 12 12 14 14 16 18 16 20 20 20 22 26 22 24 24 28 28 ...
