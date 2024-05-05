import sys
from WordList import targetWords, dictionary

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

class Deduction:
  def __init__(self):
    self.atleast = {}
    self.atmost = {}
    self.permit = ['abcdefghijklmnopqrstuvwxyz'] * 5
    self.guesses = []

  def clone(self):
    r = Deduction()
    r.atleast = dict(self.atleast)
    r.atmost = dict(self.atmost)
    r.permit = list(self.permit)
    r.guesses = list(self.guesses)
    return r

  def update(self, guess, colors):
    self.guesses.append(guess)
    greens = ''
    yellows = ''
    grays = ''
    for i in range(5):
      if colors[i] == 'G':
        assert guess[i] in self.permit[i]
        self.permit[i] = guess[i]
        greens += guess[i]
      elif colors[i] == 'Y':
        self.permit[i] = self.permit[i].replace(guess[i], '')
        yellows += guess[i]
      else:
        self.permit[i] = self.permit[i].replace(guess[i], '')
        grays += guess[i]
    yellowgreens = yellows + greens
    for letter in yellowgreens:
      n = yellowgreens.count(letter)
      if letter in yellows:
        self.atleast[letter] = max(self.atleast.get(letter, 0), n)
      if letter in grays:
        self.atmost[letter] = min(self.atmost.get(letter, 5), n)
    for letter in grays:
      if letter not in yellows:
        for i in range(5):
          if self.permit[i] != letter:
            self.permit[i] = self.permit[i].replace(letter, '')

  def isConsistent(self, guess):
    if guess in self.guesses: return False
    for (k, v) in self.atleast.items():
      if guess.count(k) < v: return False
    for i in range(5):
      if guess[i] not in self.permit[i]: return False
    for (k, v) in self.atmost.items():
      if guess.count(k) > v: return False
    return True

  def updated(self, seq):
    for guess, colors in seq:
      self.update(guess, colors)
    return self

  def selftest(self):
    assert Deduction().isConsistent('hello')
    assert not Deduction().updated([('goods', 'XYXXX')]).isConsistent('goods')  # repeated word
    assert not Deduction().updated([('goods', 'XYXXX')]).isConsistent('world')  # D
    assert not Deduction().updated([('sooty', 'XYXXX')]).isConsistent('world')  # O
    assert not Deduction().updated([('sooty', 'XGYXX')]).isConsistent('world')  # O
    assert not Deduction().updated([('sooty', 'XGYXX')]).isConsistent('robot')  # T
    assert Deduction().updated([('sooty', 'XGYYX')]).isConsistent('robot')
    assert Deduction().updated([('sooty', 'XGXXX')]).isConsistent('world')
    assert Deduction().updated([('sooty', 'XGYXX')]).isConsistent('zorro')
    assert not Deduction().updated([('sooty', 'XYGXX')]).isConsistent('zorro')  # O
    assert not Deduction().updated([('dryer', 'XXXGG')]).isConsistent('roger')  # R
    assert Deduction().updated([('areae', 'YYYXX')]).isConsistent('lager')
    assert not Deduction().updated([('areae', 'YYYXX')]).isConsistent('quare')  # E


Deduction().selftest()

import random
import time
random.shuffle(targetWords)
# dictionary = list(targetWords)
random.shuffle(dictionary)

deadline = 0
maxn = 0
def extend(n, target, deduction):
  global maxn
  global deadline
  n += 1
  for guess in (dictionary if True else targetWords):
    if deduction.isConsistent(guess):
      if guess == target:
        if (n > maxn):
          maxn = n
          print('Length %r, target %r, words are %r\n' % (n, target, deduction.guesses))
      else:
        if (n > maxn):
          maxn = n
          print('Length %r+, target %r, words are %r\n' % (n, target, deduction.guesses))
        colors = colorsOf(guess, target)
        if time.time() > deadline:
          return
        d = deduction.clone()
        d.update(guess, colors)
        extend(n, target, d)

def isValidSequence(guesses, target, deduction):
  d = deduction.clone()
  for g in guesses:
    if not d.isConsistent(g):
      # print('%r is not consistent' % (g))
      return False
    d.update(g, colorsOf(g, target))
  return True

def extendInternally(guesses):
  target = guesses[-1]
  for i in range(len(guesses)):
    d = Deduction()
    for g in guesses[:i]:
      d.update(g, colorsOf(g, target))
    for w in dictionary:
      if d.isConsistent(w):
        # print('Inserting %r at position %d (target is %r)' % (w, i, target))
        if isValidSequence([w] + guesses[i:], target, d):
          newList = guesses[:i] + [w] + guesses[i:]
          if isValidSequence(newList, target, Deduction()):
            yield newList

while True:
  for target in targetWords:
    random.shuffle(dictionary)
    print('%r' % target, end='\r')
    sys.stdout.flush()
    deadline = time.time() + 20
    d = Deduction()
    extend(0, target, d)

guesses = ['feoff', 'queer', 'niner', 'adder', 'rarer', 'paper', 'hazer', 'baler', 'layer', 'laxer', 'lawer', 'laver', 'later', 'laser', 'lamer', 'laker', 'lacer', 'lager']
for a in extendInternally(guesses):
  print(a)
