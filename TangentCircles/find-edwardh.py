# @EdwardH's solution from https://puzzling.stackexchange.com/a/126674/3896

import math
import random
import sys
import time

# We're looking for a set of radii $r_0$ (for the central circle)
# and $r_{1..n}$ (for the rest). The angle between any two circle-centers
# $r_i, r_{i+1}$ is defined by
#
#     s_i = \frac{r_i}{r_0 + r_i}
#
#     \theta_i = \acos(1 - 2 s_i s_{i+1}) = 2 \asin(\sqrt{s_i}\sqrt{s_{i+1}})
#
# We want $r_i$, and therefore $s_i$, to be rational.
# Now we'll brute-force determine some rational $t_{1..n}$ such that
#
#     \frac{2t_i}{1 + t_i^2} = \sqrt{s_i}\sqrt{s_{i+1}}
#
# Once we have such $t_i$, then we can compute $\theta_i$ as
#
#     \theta_i = 2 \asin(\sqrt{s_i}\sqrt{s_{i+1}})
#              = 2 \asin(\frac{2t_i}{1 + t_i^2})
#              = 4 \atan(t_i)
#
# So, we're looking for those rational $t_i$ such that
#
#     \sum_{i=1}^{n} \atan(t_i) = \frac{\pi}{2}
#
# Since $\theta_i = 4\atan(t_i)$ and $\theta_i < \pi/3$, we can
# limit our search to $0 < t_i < \tan(\pi/12)$.
#
# We also know that if there are $n$ circles around the center
# and $t_1$ is the largest $t$, then $4\atan(t_1) = \theta_1 > \frac{2\pi}{n}$,
# which means $t_1 > \tan(\frac{\pi}{2n})$.
#
# To sum arctangents without losing exactitude, we use the formula
#
#     \atan(x) + \atan(y) = \atan(\frac{x+y}{1-xy})
#
# which reduces to
#
#     \atan(n/d) + \atan(p/e) = \atan(\frac{ne + dp}{de - np})
#
# When $de - np$ becomes zero, then we have the asymptote, i.e. $\atan(\pi/2)$,
# and so we have found a valid set of $t_i$.
#
# @EdwardH reports the following set of $t_i$:
#
#     8/31, 1/4, 7/30, 13/59, 3/14, 1/5, 2/9
#
# atan(8/31 + 1/4) = atan(63/116)
# atan(63/116 + 7/30) = atan(2702/3039)
# atan(2702/3039 + 13/59) = atan(109/79)
# atan(109/79 + 3/14) = atan(43/19)
# atan(43/19 + 1/5) = atan(9/2)
# atan(9/2 + 2/9) = atan(1/0)
#
# Then we reverse the process: compute $s_i$ from $t_i$,
# compute $r_0$ from $s_1$, compute $r_i$ from $s_i$,
# and we're set.
#
# A list of $t_i$ in increasing order will generate a list of $\theta_i$
# in increasing order; but it won't necessarily generate a list of $r_i$
# in increasing order. So we might have to throw out some candidates.

edwardh_ts = [
  (8,31), (1,4), (7,30), (13,59), (3,14), (1,5), (2,9)
]

def valueof(n, d):
  return n / d

def frac(n, d):
  g = math.gcd(n, d)
  return (n//g, d//g)

def add(n1, d1, n2, d2):
  return frac(n1*d2 + n2*d1, d1*d2)

def sub(n1, d1, n2, d2):
  return add(n1, d1, -n2, d2)

def mul(n1, d1, n2, d2):
  return frac(n1*n2, d1*d2)

def div(n1, d1, n2, d2):
  return frac(n1*d2, d1*n2)

def singlestep_ts_progress(prog, ti):
  n,d = prog
  p,e = ti
  return frac((n*e + d*p), (d*e - n*p))

def verify_ts(ts):
  prog = ts[0]
  for t in ts[1:]:
    if (prog[1] == 0): return False
    prog = singlestep_ts_progress(prog, t)
  return (prog[1] == 0)

assert verify_ts(edwardh_ts)

def last_t_from_ts(ts):
  prog = ts[0]
  for t in ts[1:]:
    if (prog[1] == 0): return (0, 0)
    prog = singlestep_ts_progress(prog, t)
  return frac(prog[1], prog[0])


def ss_from_ts(ts):
  # assert len(ts) == 7
  us = [frac(2*n*d, (d**2 + n**2)) for n,d in ts]
  def f(a,b,c,d, x,y,z):
    n = (us[a][0] * us[b][0] * us[c][0] * us[d][0], us[a][1] * us[b][1] * us[c][1] * us[d][1])
    d = (us[x][0] * us[y][0] * us[z][0], us[x][1] * us[y][1] * us[z][1])
    return div(*n, *d)
  return [
    f(0,2,4,6, 1,3,5),
    f(1,3,5,0, 2,4,6),
    f(2,4,6,1, 3,5,0),
    f(3,5,0,2, 4,6,1),
    f(4,6,1,3, 5,0,2),
    f(5,0,2,4, 6,1,3),
    f(6,1,3,5, 0,2,4),
  ]

  # This is the general (any-number-of-circles) approach.
  assert len(ts) % 2 == 1
  ss = []
  for i in range(len(ts)):
    s = us[i]
    for j in range(1, len(ts)):
      term = us[(i+j) % len(ts)]
      if j % 2 == 0:
        s = mul(*s, *term)
      else:
        s = div(*s, *term)
    if (s[0] <= 0) or (s[1] <= 0) or (s[0] == s[1]):
      return None
    ss.append(s)
  return ss


def rs_from_ss(ss):
  rratios = [frac(n, d-n) for (n,d) in ss]
  r0 = math.lcm(*[d for (n,d) in rratios])
  rs = [frac(n*r0, d) for (n,d) in rratios]
  return [r0] + [n for (n,d) in rs]


def ss_from_rs(rs):
  return [frac(r, rs[0] + r) for r in rs[1:]]


def ts_from_ss(ss):
  assert False, "TODO: implement this"

if True:
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
  ts=[(6,23), (1,4), (7,29), (3,13), (2,9), (3,16), (78,379)]
  assert rs == rs_from_ss(ss_from_ts(ts))
  assert ss_from_rs(rs) == ss_from_ts(ts)
  # TODO: assert ts_from_ss(ss_from_rs(rs)) == ts


def brute_force_find_ts(maxx):
  # small_numbers = sorted(list(set([8,31, 1,4, 7,30, 13,59, 3,14, 1,5, 2,9])))
  # small_numbers = sorted(list(set([6, 23, 1, 4, 7, 29, 3, 13, 2, 9, 3, 16, 78, 379])))
  small_numbers = range(1, maxx)
  tanpi12 = math.tan(math.pi / 12)
  REQUIRED = [(1,4), (2,9), (1,5)]

  def possible_ts_generator(current_ts):
    # We shouldn't yield any fractions greater than or equal to (nmax/dmax),
    # because the series needs to be strictly decreasing.
    # We shouldn't yield any fractions less than (nmin/dmin), because
    # then we couldn't make it all the way around the circle.
    # We shouldn't yield any fractions less than REQUIRED if we haven't already emitted REQUIRED.
    if current_ts:
      nmax, dmax = current_ts[-1]
      usedtheta = 4*sum(math.atan(valueof(*t)) for t in current_ts)
      usedtheta += 4*math.atan(valueof(*current_ts[0]))  # for t7, which could be as big as t1
      remainingtheta = 2*math.pi - usedtheta
      avgremainingtheta = (2*math.pi - usedtheta) / (6 - len(current_ts))
    else:
      nmax, dmax = tanpi12, 1
      avgremainingtheta = (2*math.pi) / 7
    ndmin = math.tan(avgremainingtheta/4)
    for nd in REQUIRED:
      if nd not in current_ts:
        ndmin = max(ndmin, valueof(*nd))

    jstart = 1
    for i in range(len(small_numbers)):
      n = small_numbers[i]
      while jstart < len(small_numbers) and (n * dmax >= nmax * small_numbers[jstart]):
        jstart += 1
      for j in range(jstart, len(small_numbers)):
        d = small_numbers[j]
        if (n < ndmin * d):
          break
        if math.gcd(n, d) == 1:
          yield (n, d)

  def possible_ts(current_ts):
    ts = list(possible_ts_generator(current_ts))
    random.shuffle(ts)
    return ts

  # Generate 6 ts in increasing order, and the 7th to fill in the gap.
  def possible_tlists():
    for t1 in possible_ts([]):
      prog1 = t1
      for t2 in possible_ts([t1]):
        prog2 = singlestep_ts_progress(prog1, t2)
        if (prog2[0] <= 0 or prog2[1] <= 0):
          continue
        for t3 in possible_ts([t1, t2]):
          prog3 = singlestep_ts_progress(prog2, t3)
          if (prog3[0] <= 0 or prog3[1] <= 0):
            continue
          for t4 in possible_ts([t1, t2, t3]):
            prog4 = singlestep_ts_progress(prog3, t4)
            if (prog4[0] <= 0 or prog4[1] <= 0):
              continue
            for t5 in possible_ts([t1, t2, t3, t4]):
              prog5 = singlestep_ts_progress(prog4, t5)
              if (prog5[0] <= 0 or prog5[1] <= 0):
                continue
              for t6 in possible_ts([t1, t2, t3, t4, t5]):
                prog6 = singlestep_ts_progress(prog5, t6)
                if (prog6[1] <= 0) or (prog6[0] <= 0):
                  continue
                t7 = frac(prog6[1], prog6[0])
                if (valueof(*t6) < valueof(*t7) < valueof(*t1)):
                  tlist = [t1,t2,t3,t4,t5,t6,t7]
                  # assert verify_ts(tlist)
                  yield tlist

  def is_strictly_decreasing(ts):
    return all(a > b for a, b in zip(ts, ts[1:]))

  for ts in possible_tlists():
    ss = ss_from_ts(ts)
    if ss is None:
      continue
    rs = rs_from_ss(ss)
    if is_strictly_decreasing(rs):
      yield rs, ts

minc = float("inf")
started = time.time()
for rs, ts in brute_force_find_ts(int(sys.argv[1])):
  c = rs[0]
  rs = rs[1:]

  def theta(a, b):
    n = (a+c)**2 + (b+c)**2 - (a+b)**2
    d = 2*(a+c)*(b+c)
    return math.acos(n / d)

  def total_theta(rs):
    shifted_rs = rs[1:] + [rs[0]]
    # return sum(theta(a,b) for a,b in zip(rs, shifted_rs))
    return sum(theta(a,b) for a,b in zip(reversed(rs), reversed(shifted_rs)))

  if (c < minc):
    print("%r sec: rs=%r" % (time.time() - started, [c] + rs))
    print(" ts=%r" % (ts,))
    minc = c

    # Sanity-check that this indeed looks like a solution
    epsilon = abs(total_theta(rs) - 2*math.pi)
    assert epsilon < 0.000001, "should be zero, modulo IEEE-floating-point precision loss"


print("%r sec: Finished!" % (time.time() - started))
