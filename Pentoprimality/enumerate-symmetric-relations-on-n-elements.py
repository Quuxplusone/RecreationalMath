import itertools

n = 5
elts = list(range(n))

def powerset(s):
  subsets = itertools.chain.from_iterable(itertools.combinations(s, r) for r in range(len(s)+1))
  return sorted([sorted(s) for s in subsets])

def relabel(sr, permutation):
  return sorted([
    sorted([permutation[p[0]], permutation[p[1]]])
    for p in sr
  ])

def symmetric_relation_as_str(sr):
  return ''.join('T' if sorted([a,b]) in sr else 'F' for a in elts for b in elts)

permutations = list(itertools.permutations(elts))
pairs = [list(t) for t in itertools.combinations_with_replacement(elts, 2)]
symmetric_relations = powerset(pairs)
distinct_symmetric_relations = [
  sr for sr in symmetric_relations if not any(relabel(sr, perm) < sr for perm in permutations)
]
for sr in distinct_symmetric_relations:
  print(symmetric_relation_as_str(sr), ' ', sr)
