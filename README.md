# Wolves and Sheep

## The puzzle statement

From [Wolves and sheep](https://puzzling.stackexchange.com/questions/81737/wolves-and-sheep/)
(posted by Jyotish Robin on Puzzling StackExchange):

All the sheep were living peacefully in the Land of Shewo. But suddenly they
were struck by a danger. A few wolves dressed up as sheep entered the
territory of Shewo and started killing the sheep one by one.

To find a solution to this misery, the king of Shewo called upon all of his
sheep to the palace hall. He made the following announcement:

> From my secret sources, I came to know that the total number of 'sheep'
> (including the wolves) now present in my kingdom is 100. Among which 5 are
> wolves. Our doctors have come up with a very expensive blood test which
> could be used to differentiate the wolves and sheep.

> Each test costs $1000 and we don't have enough funds to test all 100 'sheep'.

> I discussed with our ministers and came to know that the tests can be done
> on pooled blood samples. I.e., I can collect blood from any number of 'sheep'
> and mix them. Then if I test the mixture, I will get a positive result if
> the mixture contains blood from any wolf. I will get a negative result if
> all the samples are from actual sheep.

One caveat is that the test results are available to you after all the tests are done!

> Now, I am looking for ideas where I can find ALL the wolves in the minimum
> number of pooled tests. I request the brilliant young minds of this land
> to come up with a testing strategy.

Can you help the king by devising a strategy?

## This repository

This repository has a C++ program for brute-forcing solutions to smaller instances
of the original puzzle. In the original puzzle we have $k=5$ wolves hiding among
$n=100$ sheep, and the number of blood tests required, $t(n, k)$, is unknown.

When brute-forcing solutions, here's what I think I know about the search space.

- Let's fix that we are searching for a solution for some concrete tuple $(n, k, t)$.
    We can immediately short-circuit some cases, such as if $t < \ceil{\lg (n\choose k)}$
    or $t \ge n-1$.

- Without loss of generality, we can force sheep to be introduced in order. That is,
    the first test must use blood from Sheep 0, may additionally use Sheep 1,
    may additionally use Sheep 2 only if it also used Sheep 1, and so on.
    If the first test used Sheep 0 through 3, then the second test may use any of
    those sheep, but it may additionally use Sheep 5 only if it also uses Sheep 4.

- Without loss of generality, we can force tests to be done in lexicographic/numeric
    order. That is, if test $i$ uses blood from Sheeps (4,3,1), then test $i+1$ could use
    (5) or (5,1) or (4,3,2); but not (4,3,0) or (4,2) or (3,2,1,0). If you run
    tests on (3,2,1,0) *followed by* (4,3,1), that's fine. By forcing the order,
    we avoid double-counting.

- We know that ultimately we must use blood from at least $n-1$ different sheep.
    (If we omitted two sheep from testing, then it might happen that one was a wolf,
    and we wouldn't know which one.) By the pigeonhole principle, at least one of
    our $t$ tests must use blood from at least $\ceil{(n-1) / t}$ sheep.
    Without loss of generality, we can assume that that test is the first test.
    We can apply this logic at each stage of testing, with respect to the $t - i$
    remaining tests and the $n - m$ remaining sheep.

- Consider all $(n\choose k)$ arrangements of wolves among our sheep.
    If, after some sequence of $i$ tests, we see that our tests so far would have
    failed to distinguish between $x > 2^{(t - i)}$ different possible arrangements of
    wolves, then we know that this line of questioning is hopeless: if we start with
    these $i$ tests, we can't possibly find all the wolves in our allotted $t$ tests.
    Checking this condition at each step allows us to short-circuit unproductive
    lines of questioning.

Here's what I know about the resulting sequence:

- We know $t(n, k) \le t(n+1, k)$. If we have a working solution $t(n+1, k)$, then
    we can solve $(n, k)$ by simply _introducing an extra innocent sheep,_
    producing an instance of $(n+1, k)$, which we know how to solve in $t$ tests.
    Therefore the _optimal_ solution for $(n, k)$ cannot possibly be worse than
    the optimal solution for $(n+1, k)$ (although it might be better).

- It appears that $t(n, k) \le t(n, k+1)$, but I have not seen how to prove this yet.

- We know $t(n, 0) = t(n, n) = 0$.

- We know $t(n, 1) = \ceil{\lg (n\choose k)}$.

- We know $t(n, n-1) = n-1$. If there's only one innocent sheep, then out of all
    the possible tests we could run, _every single test_ will return "wolfy"
    except for the empty test and the test containing blood from only the
    innocent sheep. That's the only test that gives us any information at all.
    So we must run tests on one sheep at a time in order to get any information.
    So we must run $n-1$ tests.


## The sequence

    k=       1  2  3  4  5  6  ...
          0
    n=1   0  0
    n=2   0  1  0
    n=3   0  2  2  0
    n=4   0  2  3  3  0
    n=5   0  3  4  4  4  0
    n=6   0  3  5  5  5  5  0
    n=7   0  3  6  6  6  6  6  0
    n=8   0  3  6  7  7  7  7  7  0
    n=9   0  4  7  8  8  8  8  8  8  0
    n=10  0  4  7  9  9  9  9  9  9  9  0
    n=11  0  . 8?  .  .  .  .  .  .  . 10  0

That's as many rows as I've completed.
