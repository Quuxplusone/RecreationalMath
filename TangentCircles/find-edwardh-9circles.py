# @EdwardH's solution from https://puzzling.stackexchange.com/a/126674/3896

import math
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

def mul(n1, d1, n2, d2):
  return frac(n1*n2, d1*d2)

def div(n1, d1, n2, d2):
  return frac(n1*d2, d1*n2)

def verify_ts(ts):
  n,d = ts[0]
  for p,e in ts[1:]:
    if (d == 0): return False
    (n, d) = frac((n*e + d*p), (d*e - n*p))
  return (d == 0)

assert verify_ts(edwardh_ts)

def last_t_from_ts(ts):
  n,d = ts[0]
  for p,e in ts[1:]:
    if (d == 0): return (0, 0)
    (n, d) = frac((n*e + d*p), (d*e - n*p))
  return (d, n)


def ss_from_ts(ts):
  assert len(ts) % 2 == 1
  def f(n, d):
    return frac(2*n*d, (d**2 + n**2))
  ss = []
  for i in range(len(ts)):
    s = f(*ts[i])
    for j in range(1, len(ts)):
      tsj = ts[(i+j) % len(ts)]
      term = f(*tsj)
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


def brute_force_find_ts(maxx):
  # small_numbers = sorted(list(set([8,31, 1,4, 7,30, 13,59, 3,14, 1,5, 2,9])))
  # small_numbers = sorted(list(set([6, 23, 1, 4, 7, 29, 3, 13, 2, 9, 3, 16, 78, 379])))
  small_numbers = range(1, maxx)
  tanpi12 = math.tan(math.pi / 12)

  def possible_ts(ni, di):
    jstart = 1
    for i in range(len(small_numbers)):
      n = small_numbers[i]
      while jstart < len(small_numbers) and (n * di >= ni * small_numbers[jstart]):
        jstart += 1
      for j in range(jstart, len(small_numbers)):
        if math.gcd(n, small_numbers[j]) == 1:
          yield (n, small_numbers[j])

  # Generate 8 ts in increasing order, and the 9th to fill in the gap.
  def possible_tlists():
    started = time.time()
    for t1 in possible_ts(tanpi12, 1):
      theta1 = 4*math.atan(valueof(*t1))
      if theta1 < (2*math.pi) / 9:
        continue
      for t2 in possible_ts(*t1):
        theta2 = 4*math.atan(valueof(*t2))
        if theta2 < (2*math.pi - 2*theta1) / 7:
          continue
        for t3 in possible_ts(*t2):
          theta3 = 4*math.atan(valueof(*t3))
          if theta3 < (2*math.pi - 2*theta1 - theta2) / 6:
            continue
          for t4 in possible_ts(*t3):
            theta4 = 4*math.atan(valueof(*t4))
            if theta4 < (2*math.pi - 2*theta1 - theta2 - theta3) / 5:
              continue
            for t5 in possible_ts(*t4):
              theta5 = 4*math.atan(valueof(*t5))
              if theta5 < (2*math.pi - 2*theta1 - theta2 - theta3 - theta4) / 4:
                continue
              for t6 in possible_ts(*t5):
                theta6 = 4*math.atan(valueof(*t6))
                if theta6 < (2*math.pi - 2*theta1 - theta2 - theta3 - theta4 - theta5) / 3:
                  continue
                for t7 in possible_ts(*t6):
                  theta7 = 4*math.atan(valueof(*t7))
                  if theta7 < (2*math.pi - 2*theta1 - theta2 - theta3 - theta4 - theta5 - theta6) / 2:
                    continue
                  for t8 in possible_ts(*t7):
                    tlist = [t1,t2,t3,t4,t5,t6,t7,t8]
                    t9 = last_t_from_ts(tlist)
                    if (t9[0] > 0) and (t9[1] > 0) and (valueof(*t8) < valueof(*t9) < valueof(*t1)):
                      tlist.append(t9)
                      if verify_ts(tlist):
                        yield tlist

  for tlist in possible_tlists():
    ss = ss_from_ts(tlist)
    if ss is None:
      continue
    rs = rs_from_ss(ss)
    if rs[::-1] == sorted(rs):
      yield tlist

minc = 20462058208488231647417433617297079848497688661503
started = time.time()
for ts in brute_force_find_ts(int(sys.argv[1])):
  rs = rs_from_ss(ss_from_ts(ts))
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

  epsilon = abs(total_theta(rs) - 2*math.pi)
  assert epsilon < 0.000001, "should be zero, modulo IEEE-floating-point precision loss"

  if (c < minc):
    print("%r sec: rs=%r" % (time.time() - started, [c] + rs))
    print(" ts=%r" % (ts,))
    minc = c
