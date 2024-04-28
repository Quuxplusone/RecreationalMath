This directory relates to "Reverse the List of Integers":

- https://mathstodon.xyz/@two_star/112242224494626411
- https://news.ycombinator.com/item?id=40010066
- https://quuxplusone.github.io/blog/2024/04/22/reverse-the-list/

Compile "nksolver.cpp" with `-DN=10 -DK=5` (or whatever) to compute the values
- C(n,k) = number of solvable lists
- M(n,k) = worst-case number of moves to solve any list
- L(n,k) = worst-case intermediate list length to optimally solve any list
of length `N` and initial list length `K`. (That is, exactly `C` such lists
can be solved at all; each of those `C` lists has a no-more-than-`M`-move solution;
and each of those lists has a no-more-than-`M`-move solution where each intermediate
position's list-length never exceeds `L`.

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

