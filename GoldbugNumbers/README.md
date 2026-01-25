## Goldbug numbers

See [A306746](https://oeis.org/A306746) and
[What is the best algorithm for finding Goldbug Numbers?](https://math.stackexchange.com/questions/3026044/what-is-the-best-algorithm-for-finding-goldbug-numbers) (December 2018).

- `goldbug.py` comes from the Math StackExchange answer linked above.
- `goldbug.cpp` is a C++ translation of that Python code.
- `findem.py` is a Python version I wrote myself before looking at `goldbug.py`.
    It is MUCH slower than `goldbug.py`; its only benefit is understandability
    (whereas `goldbug.py` has some magic I don't fully understand).
- `findem.cpp` is a C++ translation (and improvement) of that Python code.
    It remains slower than `goldbug.py`, let alone `goldbug.cpp`.

Even `goldbug.cpp` is slow: it can process thousands of n per second when n is small,
but once n is around 1,000,000, it's handling only 10 or 20 n per second, and the
rate keeps decreasing. According to [A306746](https://oeis.org/A306746), quoting

- Willie B. [Binghui] Wu, ["Introduction to Pipe Theory"](https://web.archive.org/web/20201017182446/https://170f119b-a-62cb3a1a-s-sites.googlegroups.com/site/basicpipetheory/doc/pipe2009_10.pdf?attachauth=ANoY7cofvc30uvhz_yGFP666wYVLKr6AM8xymwPjHo_EL4WU8_4ELo3Te3ukxf8bsFv8fx15KVuR0TFdleLbuel8g1LK_GmriXw2be7KWN2TGrbjfj-dKmADe2bLLHK2OhmTc0sqfSqGxl8kVlY3GUbJHWI3Y0gGSOsJIpg94gte_KiG5WD6Ki5jA9Ugbe8_Xom5k0CfdnnUCn8zGFyYQGXkBS7iKe2gN5XTP73jy7ct2Ddxves9YlQ%3D&attredirects=0&d=1)
    First Edition (October 21, 2009), page ix

Wu's own year-long computer search has already proven that there are no Goldbug numbers
between 6142 (the largest known) and 500,000,000 (the limit reached by Wu's search).

Wu writes:

> There are many look-like basic pipes of order 5
> as denoted as `P5G(E, n, m, d, e)`. The smallest value of `P5G(E, n, m, d, e)` is 173 digits long,
> and it is the pipe `P5G(E,1,1,111,107)`. To use computer to verify the pipes of value near
> that range is beyond the imagination of our time.

I didn't try to follow the discussion of this notation or how to compute that 173-digit value;
but Wu himself (on page 75) gives the value of `P5G(E,1,1,111,107)` as:

    1421004393105438469728179763138585768894964603546648791723
    0047060794326605647294255025896867407138484058379599443343
    129460393572580187625386158635949233364384758021881468840
