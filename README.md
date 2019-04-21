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
    the optimal solution for $(n+1, k)$ (although it can be better).

- We know $t(n, k) \le t(n, k+1)$ when $k \le n-2$. If we have a working solution $t(n, k+1)$, then
    we can run that same series of tests on $(n, k)$. Suppose (for the sake of contradiction)
    that series of tests fails to distinguish two different arrangements of $k$ wolves —
    call those arrangements $A$ and $B$. Now produce two different arrangements of $k+1$ wolves
    by the following procedure: Choose two innocent sheep in $A$. If either of them is also
    innocent in $B$, then replace that animal with a wolf in both $A'$ and $B'$ and stop.
    Otherwise, you have chosen two innocent sheep in $A$ which are wolves in $B$; correspondingly
    there must be at least two innocent sheep in $B$ which are wolves in $A$.
    Replace one of your two sheep in $A$ with a wolf in $A'$.
    Also, replace one of your two sheep in $B$ with a wolf in $B'$. (If this causes $B'$
    to be identical to $A'$, then choose the other innocent sheep you identified in $B$ instead.
    This is why it's important that $k \le n-2$, so that we have that choice.)
    Now $A'$ and $B'$ are two different arrangements of $k+1$ wolves, and by construction
    they cannot be distinguished by our series of tests on $n$ animals. But our series of
    tests is known to be a solution to $(n, k+1)$! Contradiction; therefore we must reject
    our supposition that $A$ and $B$ exist; thus our solution to $(n, k+1)$ is also a
    solution to $(n, k)$.
    Therefore, when $k \le n-2$, the _optimal_ solution for $(n, k)$ cannot possibly be
    worse than the optimal solution for $(n, k+1)$ (although it can be better).

- We know $t(n, k) \le t(n+1, k) \le t(n, k)+1$ when $k < n$. We can solve $(n+1, k)$
    by running our solution for $(n, k)$ on the first $n$ animals, and then additionally
    testing the last animal individually. If the last animal is a sheep, then our testing
    strategy solves the remaining instance of $(n, k)$; if the last animal is a wolf, then
    (by the argument in the preceding bullet point) our same testing strategy solves the
    remaining instance of $(n, k-1)$.

- We know $t(n, k) < t(n+1, k+1)$ when $k < n$. Suppose we have a solution to $(n+1, k+1)$
    that takes $t$ tests, and where the most-tested animal participates in $i$ different tests.
    (We know $i \ge 1$.) Then we can solve $(n, k)$ by simply _introducing an extra wolf,_
    producing an instance of $(n+1, k+1)$, which we know how to solve in $t$ tests.
    In fact, we will place our new wolf in the most-tested position, so that $i$ of our
    $t$ tests have a foregone conclusion of "wolfy," and therefore we don't need to run them.
    Therefore we can solve $(n, k)$ in $t(n+1, k+1) - i$ tests.

- We know $t(n, 0) = t(n, n) = 0$.

- We know $t(n, 1) = \ceil{\lg (n\choose k)}$.

- We know $t(n, n-1) = n-1$. If there's only one innocent sheep, then out of all
    the possible tests we could run, _every single test_ will return "wolfy"
    except for the empty test and the test containing blood from only the
    innocent sheep. That's the only test that gives us any information at all.
    So we must run tests on one sheep at a time in order to get any information.
    So we must run $n-1$ tests.

- In fact it seems that $t(n, n/2) = n-1$ (verified empirically up to $n = 28$),
    but I don't know how to prove this. This might be simply a case of the
    Law of Small Numbers: the information-theoretic lower bound
    $\ceil{\lg (n\choose{n/2})}$ diverges only very slowly from the upper
    bound of $n-1$.


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
    n=11  0  4  8 10 10 10 10 10 10 10 10  0
    n=12  0  4  .  . 11 11 11 11 11 11 11 11  0
    n=13  0  4  .  . 12 12 12 12 12 12 12 12 12  0
    n=14  0  4  .  .  . 13 13 13 13 13 13 13 13 13  0
    n=15  0  4  .  .  . 14 14 14 14 14 14 14 14 14 14  0
    n=16  0  4  .  .  .  . 15 15 15 15 15 15 15 15 15 15  0
    n=17  0  5  .  .  .  .  . 16 16 16 16 16 16 16 16 16 16  0
    n=18  0  5  .  .  .  .  . 17 17 17 17 17 17 17 17 17 17 17  0
    n=19  0  5  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  . 18  0
    n=20  0  5  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  .  . 19  0
