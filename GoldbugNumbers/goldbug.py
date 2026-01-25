#!/usr/bin/python3
import sys

def sieve(n):
    n //= 2
    r = [[] for _ in range(n)]
    for x in range(1, n):
        if r[x]:
            continue
        r[x] = None
        y = 3*x+1
        while y < n:
            r[y].append(2*x+1)
            y += x+x+1
    return r

def check(n, r):
    s = {}
    for i in range(3, n//2, 2):
        if r[i>>1] is not None or not n % i:
            continue
        for f in r[(n-i)>>1] or [n-i]:
            s.setdefault(f, []).append(i)
    bad = [x for x in s if x >= n//2]
    bad_seen = set(bad)
    ok = set(i for i in range(3, n // 2, 2) if r[i>>1] is None and n % i)
    while bad:
        x = s.get(bad.pop(), ())
        for p in x:
            if p not in bad_seen:
                bad.append(p)
                bad_seen.add(p)
                ok.discard(p)
    return ok

def search(lim):
    r = sieve(lim)
    for i in range(2, lim, 2):
        x = check(i, r)
        if x:
             print(i, sorted(x))

search(int(sys.argv[1]))
