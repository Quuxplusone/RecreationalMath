
# Primality testing from https://stackoverflow.com/a/78349671/1424877
import secrets
import sys

if False:
  # If you use this, add `PYTHONPATH=../Wordle` in front of your Python invocation.
  from WordList import targetWords
else:
  # If you use this, pipe /usr/dict/words into stdin.
  targetWords = [line.strip().lower() for line in sys.stdin.readlines()]

words = [t.upper() for t in targetWords]
words += [t.title() for t in targetWords]
words += [t.lower() for t in targetWords]


def millerTest(d, n):
  a = 2 + secrets.randbelow(n - 4)
  x = pow(a, d, n)
  if (x == 1 or x == n - 1):
    return True
  while (d != n - 1):
    x = (x * x) % n
    d *= 2
    if (x == 1):
      return False
    if (x == n - 1):
      return True
  return False


def isPrime(n):
  k = min(int(len(str(n))/5) + 4, 14)
  if n <= 3:
    return n > 1
  if (n & 1 == 0):
    return False
  d = n - 1
  while (d % 2 == 0):
    d //= 2
  for i in range(k):
    if not millerTest(d, n):
      return False
  return True


for w in words:
  bigendian = ''.join('%02x' % ord(c) for c in w)
  n1 = int(bigendian, 16)
  if not isPrime(n1):
    continue
  littleendian = ''.join(reversed(['%02x' % ord(c) for c in w]))
  n2 = int(littleendian, 16)
  if not isPrime(n2):
    continue
  print('%s' % w)
