#!/usr/bin/env python

import sympy
import time

def check_m(m, sieve):
  ndps = {p for p, kp in sieve.items() if kp != m}

  for p, kp in sieve.items():
    if kp <= m:
      sieve[p] = kp + p

  if (len(ndps) == len(sieve)) and (m & 1):
    # m is prime; add it to our sieve
    # assert sympy.ntheory.isprime(m), m
    sieve[m] = 2*m
    # and, since m is prime, 2m is not a Goldbug;
    # but "1718" is in A306746, so let's check it anyway.

  n = 2*m
  # assert all((n % p) != 0 for p in ndps)
  # assert all(sympy.ntheory.isprime(p) for p in ndps)

  lo = (m + 1) >> 1
  hi = (m & ~1)

  for p in ndps:
    if not (lo <= p <= hi):
      continue
    # Compute our set of NDPs "P", starting with P={p} and expanding by adding
    # the factors of (n - p_i) until either we reach the transitive closure
    # (in which case n is a Goldbug) or attempt to add a factor that's not in ndps.
    frontier = set([p])
    interior = set()
    while frontier:
      # Shuffle a factor from frontier to interior, and expand the frontier.
      q = frontier.pop()
      interior.add(q)
      margin = set(sympy.ntheory.primefactors(n - q))
      if not (margin <= ndps):
        # Adding the NDP q to our set of primes requires us to add a non-NDP too, so give up on it.
        break
      frontier |= (margin - interior)
    else:
      # The frontier is now empty.
      assert all(f in ndps for f in interior)
      print('GOLDBUG %d = 2*%d => %r' % (n, n // 2, sorted(interior)))
      return None

  # Dropping out of the for-loop means we failed to find any workable set "P";
  # therefore n is not a Goldbug.
  return None

sieve = {}
m = 3
start = time.time()
while True:
  check_m(m, sieve)
  m += 1
  if m % 1000 == 0:
    print('%d in %d seconds' % (m, int(time.time() - start)))
