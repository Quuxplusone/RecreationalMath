import itertools
from num2words import num2words
import sys

example1 = 'one a one b one c one d twenty-eight e seven f five g five h eight i one j one k one l one m\
eighteen n eighteen o one p one q four r two s ten t four u five v four w one x two y one z'
example2 = 'one a,b,c,d,g,h,i,j,k,l,m,p,q,s,v,x,y,z; two e,f,n,r,t,u,w; four o'
example3 = 'one of b,c,j,k,l,m,p,q,y,z; two of a,d,g,u,v,w,x; three of h,i,r,s; four of n,t; six f; seven e; and eight o'
example4 = "this sentence has one of b, j, k, l, m, p, q, y, and z; two of c, g, u, and w; three of r, v, and x; seven of a, f, and i; five h's; six d's; eight o's; ten t's; fourteen s's; sixteen n's; and nineteen e's"

alphabet = 'abcdefghijklmnopqrstuvwxyz'

numbers = {
  i: num2words(i)
  for i in range(100)
}
numbers[0]='no'

for i,n in numbers.items():
  print('  "%s",' % n)
sys.exit(0)

def to_inventory(sentence):
  return tuple(
    sum(1 if (ch == letter) else 0 for ch in sentence)
    for letter in alphabet
  )

def to_sentence(inventory):
  sentence = 'icontaineachand'
  d = {}
  for i in range(26):
    count = inventory[i]
    if count in d:
      d[count] += alphabet[i]
    else:
      d[count] = alphabet[i]
  for count, chars in d.items():
    sentence += numbers[count]
    if len(chars) >= 2:
      sentence += 'ofand'
    sentence += chars
    if len(chars) == 1 and count >= 2:
      sentence += 's'
  return sentence

def to_sentence(inventory):
  sentence = ''
  for i in range(26):
    sentence += numbers[inventory[i]]
    sentence += alphabet[i]
  return sentence

seent = set()
bestlen = 9999
for counts in itertools.product(range(1,29), repeat=26):
  sentence = to_sentence(counts)
  inventory = to_inventory(sentence)
  while inventory not in seent:
    seent.add(inventory)
    sentence = to_sentence(counts)
    inventory = to_inventory(sentence)
  if sentence == to_sentence(inventory):
    if len(sentence) < bestlen:
      print('<', sentence)
      bestlen = len(sentence)
    else:
      print('+', sentence)
