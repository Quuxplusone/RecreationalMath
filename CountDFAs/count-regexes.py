# a(n) = Count of languages over {a,b} recognizable by a regex with n characters.
# The grammar supported is:
#   regex     = <empty>           // Match the empty string
#             | starrable         // Match starrable
#             | regex '|' regex   // Match either of the terms (alternation)
#             ;
#   starrable = 'a' | 'b' | '.'   // Match a, b, or any single character, respectively
#             | '(' regex ')'     // Match regex
#             | starrable '*'     // Match starrable 0 or more times (Kleene star)
#             ;
#
# For example, a 1-character regex could be 'a', 'b', '.', or '|', accepting
# respectively {'a'}, {'b'}, {'a','b'}, or {''}. So a(1) = 4.
#
# To determine whether two superficially different regexes accept the same language,
# we just run them brute-force on all possible input strings up to some given length `i`.
# (Length `2n-1` suffices to distinguish any two `n`-state DFAs.)
#
# Run as `python count-regexes.py n i`, where `n` is the number of characters in the regex (e.g. 4)
# and `i` (optional) is the number of letters'-worth of input to test.
#
# a(n) = 1, 4, 15, 60, 243, 952, 3872, 15594, ...

import itertools
import re
import sys

def is_valid_regex(s):
  starrable = False
  parens = 0
  for c in s:
    if c in 'ab.':
      starrable = True
    elif c == '(':
      starrable = False
      parens += 1
    elif c == ')':
      if parens == 0: return False
      starrable = True
      parens -= 1
    elif c == '|':
      starrable = False
    elif c == '*':
      if not starrable: return False
    else:
      assert False, c
  return (parens == 0)

def all_regexes(n):
  for s in itertools.product('ab.*|()', repeat=n):
    if is_valid_regex(s):
      yield ''.join(s)

def pythonize(r):
  # Python raises an exception on "a**", whereas we see nothing wrong with it.
  while '**' in r:
    r = r.replace('**', '*')
  return '^' + r + '$'

def language_accepted_by(r, sample_inputs):
  return ''.join(('T' if re.match(pythonize(r), input) else 'F') for input in sample_inputs)

n = int(sys.argv[1])
si = int(sys.argv[2]) if len(sys.argv) >= 3 else (2*n - 1)

sample_inputs = []
for r in range(si):
  sample_inputs += [''.join(s) for s in itertools.product('ab', repeat=r)]

all_langs = set()
for r in all_regexes(n):
  lang = language_accepted_by(r, sample_inputs)
  if lang not in all_langs:
    print(r)
    all_langs.add(lang)
