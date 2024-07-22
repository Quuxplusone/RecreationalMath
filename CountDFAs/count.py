# a(n) = Count of languages recognizable by a DFA with n states.
#
# For example, with 1 state, you can't leave that one state, and it can
# either accept (accepting the universal language) or reject (accepting
# the empty language). So a(1) = 2.
#
# For a(2) = 26, there are thirteen recognizable languages plus their complements:
#  1  .*         2  NOT(1)
#  3  (a|b.)*    7  NOT(3)
#  5  (a|ba*b)*  9  NOT(5)
#  6  a*         10 NOT(6)
#  8  .*b        4  NOT(8)
#  11 (b|a.)*    15 NOT(11)
#  12 (b|ab*a)*  16 NOT(12)
#  14 b*         18 NOT(14)
#  17 .*a        13 NOT(17)
#  19 (..)*      23 NOT(19)
#  20 (.b*a)*    24 NOT(20)
#  21 (.a*b)*    25 NOT(21)
#  26 ..*        22 NOT(26)
#
# To determine whether two superficially different DFAs accept the same language,
# we just run them brute-force on all possible input strings up to some given length `i`.
# (Length `2n-1` suffices to distinguish any two `n`-state DFAs.)
#
# Run as `python count.py n i`, where `n` is the number of states (e.g. 4)
# and `i` (optional) is the number of letters'-worth of input to test.
#
# a(n) = 2, 26, 1054, 57068, 3762374, ...
#
# Actually, to save time, we avoid counting any DFA that doesn't accept the
# empty string; and then we can just double the final tally.
#
# a(n) = 1, 13,  576, 28534, 1881187, ...


import itertools
import sys

def print_dfa(d):
  for s in range(1, len(d)+1):
    if s != 1: print('; ', end='')
    print(' State %d: a->%d, b->%d%s' % (s, d[s]['a'], d[s]['b'], ', accepting' if d[s]['accept'] else ''), end='')
  print('')

# A sample one-state DFA accepting the universal language.
dfa = {
  1: {'a': 1, 'b': 1, 'accept': True}
}

# Yield all DFAs with states 1..m inclusive.
def all_dfas(n, m):
  if m == 0:
    yield {}
  else:
    for d in all_dfas(n, m-1):
      if m-1 >= 1 and d[1]['a'] == 1 and d[1]['b'] == 1:
        yield d
      else:
        for x in range(1, n+1):
          for y in range(1, n+1):
            for z in ([True] if m == 1 else [True, False]):
              d[m] = {'a': x, 'b': y, 'accept': z}
              yield d

def dfa_accepts(d, input):
  s = 1
  for i in input:
    s = d[s][i]
  return d[s]['accept']


n = int(sys.argv[1])
si = int(sys.argv[2]) if len(sys.argv) >= 3 else (2*n - 1)

sample_inputs = []
for r in range(si):
  sample_inputs += list(itertools.product('ab', repeat=r))

all_langs = set()
c = 1
for i, d in enumerate(all_dfas(n, n)):
  lang = ''.join(('x' if dfa_accepts(d, input) else '.') for input in sample_inputs)
  if lang not in all_langs:
    print(c, end='.  ')
    c += 1
    print_dfa(d)
    all_langs.add(lang)

