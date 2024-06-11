## Tangent circles with integer radii

https://puzzling.stackexchange.com/questions/126548/geometry-puzzle-tangent-circles-with-integer-radii
https://math.stackexchange.com/questions/4913156/finding-a-sequence-of-tangent-circles-with-integer-radii
https://quuxplusone.github.io/blog/2024/06/10/tangent-circles-of-integer-radius/

Find a set of at least 7 circles, with unique integer radii, such that if you put the biggest circle
in the middle and surround it with the other circles in strictly decreasing order, the surrounding
circles form an exactly kissing ring.

[1,1,1,1,1,1,1,1] is a solution except for the "uniqueness" requirement.
[621, 5037, 3942, 3358] is a solution except for the "biggest in the middle" requirement.
Solutions lacking only the "integer" requirement are trivial to exhibit (if not to calculate exactly).

----

`find-ieee.py` and `find-mpmath.py` are just a dumb brute-force search for sets of circles where the gap
(or overlap) is small. It runs into the limits of IEEE double-precision floating-point very quickly.
To use the arbitrary-precision version, `pip install mpmath` and then:

    $ python find-mpmath.py [precision] [initial largest-circle size]

Sample solutions (notice the different totals because floating-point addition isn't
associative):

    c=46, [44, 35, 25, 22, 18, 17, 13, 10, 9, 8, 7, 6, 4]:
      -2.62323e-11 radians (or maybe -2.62332e-11 radians) in 6792 seconds
      To close the gap, the smallest circle's radius should be 4 + 3.82403e-10

    c=47, [44, 43, 41, 36, 27, 17, 15, 10, 6, 5, 3]:
      -8.0087e-12 radians (or maybe -8.00782e-12 radians) in 16513 seconds
      To close the gap, the smallest circle's radius should really be 3 + 1.02627e-10

----

EdwardH on Puzzling.SE has come up with a set of (extremely large) integers that work!
The script `edwardh.py` repeats the reasoning from [his answer](https://puzzling.stackexchange.com/a/126674/3896)
and then verifies his solution.

    $ python find-edwardh.py [maxt]

Use maxt=60 to eventually (re)discover @EdwardH's solution. Lower values for faster
exhaustive searching; higher values for slower (but potentially even better solutions).

So far, the solution with smallest central circle is:

    $ python find-edwardh.py 149
    rs = [
      2941564115506288009572255050208,
      2641663701806844064724293810176,
      2588185711710249680183593951791,
      2354857694119310401212561211842,
      2236188368634576996067318957950,
      2089644243395382075169644129792,
      1996023217448211952755320125300,
      1967470772556389886966582369792,
    ]
    ts = [1/4, 7/29, 3/13, 2/9, 3/14, 29/138, 11/48]

----

`soddy.py` ignores the "strictly decreasing" criterion of the actual problem.
It simply picks three integer bends for three mutually tangent outer circles, and then
computes the bend of the inner Soddy circle that kisses all three of them. Then it converts
bends to integer radii. It finds that the smallest set of four mutually tangent circles
has radii (6 23 46 69), or (1 3 12 12) if we allow duplicates.
