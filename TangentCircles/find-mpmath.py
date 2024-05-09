import itertools
import mpmath
import sys
import time

mpmath.mp.dps = int(sys.argv[1])

started = time.time()
minn = 0.01
tau = 2 * mpmath.pi

for c in range(int(sys.argv[2]), 120):

  def theta(a, b):
    a = mpmath.mpf(a)
    b = mpmath.mpf(b)
    n = (a+c)**2 + (b+c)**2 - (a+b)**2
    d = 2*(a+c)*(b+c)
    return mpmath.acos(n / d)

  def total_theta(rs):
    shifted_rs = rs[1:] + [rs[0]]
    # return sum(theta(a,b) for a,b in zip(rs, shifted_rs))
    return sum(theta(a,b) for a,b in zip(reversed(rs), reversed(shifted_rs)))

  def smallest_radius(rs):
    rs = rs[:]
    tt = total_theta(rs)
    target = tau
    delta = mpmath.mpf(0.5 if tt < target else -0.5)
    while True:
      rs[-1] += delta
      tt2 = total_theta(rs)
      if (tt > target) != (tt2 > target):
        rs[-1] -= delta
        if (tt == tt2):
          return rs[-1]
      else:
        tt = tt2
      delta *= 0.5
      if delta + mpmath.mpf(1) == mpmath.mpf(1):
        return rs[-1]

  def explore(partial, rs):
    global minn
    total = partial + theta(rs[-1], rs[0])
    epsilon = (total - tau)
    if abs(epsilon) <= minn:
      reepsilon = total_theta(rs) - tau
      print("c=%d, %r: %r radians (or maybe %r radians) in %d seconds" % (c, rs, epsilon, reepsilon, time.time() - started))
      print("  The smallest circle's radius should really be %d + %r" % (rs[-1], smallest_radius(rs) - rs[-1]))
      minn = abs(epsilon)
    if (total < tau):
      for r in range(1, rs[-1]):
        rs.append(r)
        explore(partial + theta(rs[-2], r), rs)
        rs.pop()

  for r1 in range(1, c):
    for r2 in range(1, r1):
      explore(theta(r1, r2), [r1, r2])
