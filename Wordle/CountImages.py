from WordList import targetWords, dictionary
import itertools
import time

def colorsOf(guess, target):
  colors = ['X', 'X', 'X', 'X', 'X']
  unmatched = ''
  for i in range(5):
    if guess[i] == target[i]:
      colors[i] = 'G'
    else:
      unmatched += target[i]
  for i in range(5):
    if colors[i] == 'X' and (guess[i] in unmatched):
      colors[i] = 'Y'
      unmatched = unmatched.replace(guess[i], '', 1)
  return ''.join(colors)

assert colorsOf('coots', 'scoot') == 'YYGYY'
assert colorsOf('hello', 'world') == 'XXXGY'

start = time.time()
def elapsed():
  return int(1000 * (time.time() - start))

poss = {
  target: set(colorsOf(g, target) for g in dictionary)
  for target in targetWords
}
print('len(poss) is %d, in %d ms' % (len(poss), elapsed()))

# Make a list of all the possible rows. There should be 238 of them.
rows = sorted(set().union(*poss.values()))
print('len(rows) is %d (should be 238)' % len(rows))

poss = set(repr(sorted(v)) for v in poss.values())
# Now poss is uniqued; it's a set of string-reprs-of lists of strings
poss = [set(eval(s)) for s in poss]
# Now it's a uniqued list of sets of strings
poss = [set(rows.index(s) for s in ss) for ss in poss]
# Now it's a uniqued list of sets of ints

print('len(poss) is %d, in %d ms' % (len(poss), elapsed()))

rowIndices = list(range(238))
atomicImpossibleSets = []
for r in [1,2,3,4,5,6]:
  sixes_impossible = 0
  newAtomicImpossibleSets = []
  for rowcomb in itertools.combinations(rowIndices, r):
    rc = set(rowcomb)
    if any(ip.issubset(rc) for ip in atomicImpossibleSets):
      sixes_impossible += 1
      continue
    if not any(rc.issubset(p) for p in poss):
      sixes_impossible += 1
      newAtomicImpossibleSets.append(rc)
  atomicImpossibleSets += newAtomicImpossibleSets
  print('%d combinations of %d rows are impossible, in %d ms; len(atomicImpossibleSets) is %d' % (sixes_impossible, r, elapsed(), len(atomicImpossibleSets)))
