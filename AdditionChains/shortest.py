
import sys

def shortest_chain(chain, target, bestchain):
  if len(chain) >= len(bestchain):
    return bestchain
  chainmax = chain[-1]
  for b in chain:
    for a in chain:
      if a > b: break
      ab = a + b
      if ab == target:
        return chain + [target]
      if chainmax < ab < target:
        bestchain = shortest_chain(chain + [ab], target, bestchain)
  return bestchain

print('OEIS sequence A003313:')
for target in range(2, 100):
  print(len(shortest_chain([1], target, list(range(1, target+1))))-1, end=" ")
  sys.stdout.flush()
