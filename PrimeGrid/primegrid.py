import copy
import itertools
import random

def is_prime_(n):
  if n == 2 or n == 3: return True
  if n < 2 or n%2 == 0: return False
  if n < 9: return True
  if n%3 == 0: return False
  r = int(n**0.5)
  # since all primes > 3 are of the form 6n ± 1
  # start with f=5 (which is prime)
  # and test f, f+2 for being prime
  # then loop by 6. 
  f = 5
  while f <= r:
    if n % f == 0: return False
    if n % (f+2) == 0: return False
    f += 6
  return True 

primes = [i for i in range(1000000) if is_prime_(i)]
def is_prime(i):
  return (i in primes)

def to_number(seq):
  sum = 0
  for digit in seq:
    sum = (10 * sum) + digit
  return sum

def all_numbers_in(grid):
  n = len(grid)
  for x in range(n):
    for y in range(n):
      # Single-digit...
      yield grid[x][y]
      # Horizontal...
      for k in range(2, y+2):
        yield to_number(grid[x][y-i] for i in range(k))
      for k in range(2, n-y+1):
        yield to_number(grid[x][y+i] for i in range(k))
      # Vertical...
      for k in range(2, x+2):
        yield to_number(grid[x-i][y] for i in range(k))
      for k in range(2, n-x+1):
        yield to_number(grid[x+i][y] for i in range(k))
      # Diagonals...
      for k in range(2, min(n-x+1, n-y+1)):
        yield to_number(grid[x+i][y+i] for i in range(k))
      for k in range(2, min(n-x+1, y+2)):
        yield to_number(grid[x+i][y-i] for i in range(k))
      for k in range(2, min(x+2, n-y+1)):
        yield to_number(grid[x-i][y+i] for i in range(k))
      for k in range(2, min(x+2, y+2)):
        yield to_number(grid[x-i][y-i] for i in range(k))

def count_primes(grid):
  myset = set()
  for p in all_numbers_in(grid):
    if is_prime(p):
      myset.add(p)
  return len(myset)

def to_grid(s):
  return [list(map(int, row)) for row in s.split(',')]

grid = to_grid('313991,983929,164312,517471,715971,937339') # 169

grid = [[3, 7, 3, 3, 9, 1], [9, 8, 3, 9, 2, 9], [3, 6, 4, 3, 4, 9], [1, 3, 7, 1, 1, 7], [1, 1, 5, 9, 7, 3], [9, 1, 1, 9, 3, 9]]  # 174
grid = to_grid('317333,995639,118142,136373,349199,379379') # 187

best = 180
while True:
  MAX = random.choice([1,1,1,1,1,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,6,7,8,9,10,11,12,13,14,15,16])
  oldgrid = copy.deepcopy(grid)
  for r in range(MAX):
    x = random.choice(range(6))
    y = random.choice(range(6))
    grid[x][y] = random.choice(range(10))
  count = count_primes(grid)
  if count > best:
    print(count, grid)
    best = count
  else:
    grid = oldgrid
print('Done!')
