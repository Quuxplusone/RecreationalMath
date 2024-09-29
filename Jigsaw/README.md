
See Stand-Up Maths' video
["How can a jigsaw have two distinct solutions?"](https://www.youtube.com/watch?v=b5nElEbbnfU)
([Hacker News discussion thread](https://news.ycombinator.com/item?id=41684793))
for how I got into this particular rabbit hole.

---

Define an "NxM jigsaw puzzle" as a labeling of the interiors of the edges of
an NxM grid graph such that each edge is labeled "OUT" on one side and "IN"
on the other. For example,

    +-+-+  +-+-+-+
    |A>B|  |A>B>C|
    +v+^+  +v+^+v|
    |C>D|  |D>E>F|
    +-+-+  +-+-+-+

are a 2x2 and a 2x3 jigsaw puzzle, respectively. (Think of the small end of
each arrow as "IN" and the big end as "OUT".)

Now, a "solution" to a jigsaw puzzle is a rearrangement (without reflections)
of those labeled puzzle pieces, up to rotation of the whole puzzle.
For example, here are the two solutions to the 2x2 jigsaw above:

    +-+-+  +-+-+
    |A>B|  |A>B|
    +v+^+  +v+^+
    |C>D|  |D>C|
    +-+-+  +-+-+

This 2x2 jigsaw "obviously" has multiple solutions, because pieces C and D are
identical: they have the same arrangement of ins and outs along their edges.
The 2x3 jigsaw has no two identical pieces, and also only the one solution.

Notice that in order to avoid identical pieces, the puzzle must have exactly
these four corner pieces (since there are only four possible corner pieces).
Or again, there are only 8 possible side pieces and 6 possible center pieces,
so no rectangular puzzle can be larger than 4x4 (3x5, 2x6) without duplication.
The number of "blank" NxM jigsaw solutions (up to rotation) is:

      m=1  2  3    4    5   6

  n=1   1  1  1    1    1   -
    2      1  4   20   88 136  -
    3        72  772 1720   -
    4           1312    -
    5                   -

However, notice that a 3x5 jigsaw has only three center tiles — giving it
(6 choose 3) = 20 possible choices for those tiles — plus all 8 side pieces
and all 4 corners; so although there are 1720 ways of putting together
15 distinct tiles into a 3x5 jigsaw, there are no more than 20 distinct
"bags" of pieces that can be assembled into a 3x5 jigsaw. Each of those
1720 blank 3x5 jigsaw solutions merely rearranges the pieces from one of those
20 "bags."

---

We can choose to count solutions as unique up to rotation of the whole puzzle
(as if there were a picture on each piece), *or* up to rotation of the individual
pieces (as if each piece were color-coded with a unique color, but otherwise
undistinguished). That is, we could count this 3x3 puzzle as having two solutions
(formed by the "obvious" exchange of identical subunits BC and GH), or as having
four distinct solutions depending also on the rotation of symmetrical central piece "E":

    +-+-+-+  +-+-+-+
    |A>B>C|  |A>H<G|
    +v+^+^+  +v+^+^+
    |D>E<F|  |D>E<F|
    +v+v+v+  +v+v+v+
    |G>H<I|  |C<B<I|
    +-+-+-+  +-+-+-+


