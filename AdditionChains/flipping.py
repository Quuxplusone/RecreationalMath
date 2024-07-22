
import sys

def ANDEM(a, b, c, d):
  return (a*c, b*d)

def OREM(a, b, c, d):
  return (a*d + b*c - a*c, b*d)

def shortest_chain(chain, target, bestchain):
  if target in chain:
    return chain
  frontier = [chain]
  while True:
    chain = frontier.pop(0)
    if len(chain) >= len(bestchain):
      return bestchain
    for b in chain:
      for a in chain:
        if a > b: continue
        aab = ANDEM(*a, *b)
        aob = OREM(*a, *b)
        if aab == target or aob == target:
          return chain | set([target])
        if aab not in chain:
          frontier.append(chain | set([aab]))
        if aob not in chain:
          frontier.append(chain | set([aob]))
  return bestchain

for bigt in range(1, 5):
  inputs = set([(1,2),(1,4),(1,8),(9,16)])
  outputs = set()
  for t in range(1, bigt):
    inputs |= outputs
    outputs = [ANDEM(*p, *q) for p in inputs for q in inputs]
    outputs += [OREM(*p, *q) for p in inputs for q in inputs]
    outputs = set(outputs) - inputs

  print('REACHABLE IN %d: %r\n' % (bigt, outputs))

  # Look for a target that is reachable in K by passing through 9/16,
  # but which isn't reachable in K by passing through (3/4, 9/16).

  MAX = set(range(10))
  for target in outputs:
    print('trying to reach %r from 9/16-formed-nonoptimally' % (target,))
    c1 = shortest_chain(set([(1,2),(1,4),(1,8),(9,16)]), target, MAX)
    if c1 == MAX:
      continue
    print(' now trying to reach %r optimally' % (target,))
    c2 = shortest_chain(set([(1,2)]), target, c1)
    if len(c1) == len(c2):
      print(target, '  shortest chain is', c1)
      print('  now trying to reach %r from 9/16-formed-optimally' % (target,))
      c3 = shortest_chain(set([(1,2),(3,4),(9,16)]), target, c1 | set([42]))
      if len(c3) > len(c1):
        print('FOUND ONE!!!', target, 'shortest chain is:', c1, ' which includes 9/16; but the shortest with 3/4, 9/16 is:', c3)
