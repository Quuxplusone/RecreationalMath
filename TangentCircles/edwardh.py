# @EdwardH's solution from https://puzzling.stackexchange.com/a/126674/3896

import math

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
    ss.append(s)
  return ss


def rs_from_ss(ss):
  rratios = [frac(n, d-n) for (n,d) in ss]
  r0 = math.lcm(*[d for (n,d) in rratios])
  rs = [frac(n*r0, d) for (n,d) in rratios]
  assert all(d == 1 for (n,d) in rs)
  return [r0] + [n for (n,d) in rs]


edwardh_rs = rs_from_ss(ss_from_ts(edwardh_ts))
assert edwardh_rs == [
  1346015551088116451588241696826457667928305,
  1285983548348276483652942082564013466760320,
  1238768816482055499967626824541529444769070,
  1156239292571620556013720831252688362612000,
  990358148968489476126926154954692437512561,
  961539350943357825433592453776542736631070,
  908374343417739169873995450234402764477000,
  780821295607168322239085153945384792689735
]

c = edwardh_rs[0]
rs = edwardh_rs[1:]

def theta(a, b):
  n = (a+c)**2 + (b+c)**2 - (a+b)**2
  d = 2*(a+c)*(b+c)
  return math.acos(n / d)

def total_theta(rs):
  shifted_rs = rs[1:] + [rs[0]]
  # return sum(theta(a,b) for a,b in zip(rs, shifted_rs))
  return sum(theta(a,b) for a,b in zip(reversed(rs), reversed(shifted_rs)))

print(total_theta(rs))
print(2*math.pi)
