## Tangent circles with integer radii

https://puzzling.stackexchange.com/questions/126548/geometry-puzzle-tangent-circles-with-integer-radii
https://math.stackexchange.com/questions/4913156/finding-a-sequence-of-tangent-circles-with-integer-radii

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

    $ python find-edwardh.py 30
    rs=[
      19849928693561210643960383705454462,
      19762344279715183720410347994440538,
      18199600765454631678943172642716800,
      17113717966522320797355163059980613,
      16206226732750885227565832833854069,
      14805196711479374505560692098486669,
      14366736306812578780218882552019488,
       9025120472078837841529592293969024
    ]
    ts=[6/23, 1/4, 7/29, 3/13, 2/9, 3/16, 78/379]

