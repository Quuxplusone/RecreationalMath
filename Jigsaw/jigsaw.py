#!/usr/bin/env python

import copy
import itertools
import sys

piecenames = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'

def rotateList(lst, k):
  return lst[k:] + lst[:k]

class Piece:
  def __init__(self, x, y):
    self.sides_ = ['S' for i in range(4)]  # 0=top, 1=right,...
    self.position_ = (x, y)
    self.name_ = 'P'

  def rotateLeft(self):
    self.sides_ = rotateList(self.sides_, 1)

  def rotateRight(self):
    self.sides_ = rotateList(self.sides_, 3)

  def isSuperimposableOn(p, q):
    return p.sides_ == q.sides_

  def isEquivalentTo(p, q):
    if (p.sides_ == q.sides_):
      return True
    return any(p.sides_ == rotateList(q.sides_, k) for k in [1,2,3])

class SolvedJigsaw:
  def __init__(self, n, m):
    self.dimensions_ = (n, m)
    self.pieces_ = [Piece(x,y) for x in range(m) for y in range(n)]

  def at(self, x, y):
    return next(p for p in self.pieces_ if p.position_ == (x, y))

  def hasUniquePieces(self):
    for i in range(len(self.pieces_)):
      for j in range(i):
        if self.pieces_[i].isEquivalentTo(self.pieces_[j]):
          return False
    return True

  def edgeToRightOf(self, x, y):
    p = self.at(x, y)
    if x+1 == self.dimensions_[1]:
      return '|'
    q = self.at(x+1, y)
    ch1 = '>' if p.sides_[1] == 'O' else '<'
    ch2 = '>' if q.sides_[3] == 'I' else '<'
    assert ch1 == ch2, (ch1, ch2)
    return ch1

  def edgeBelow(self, x, y):
    p = self.at(x, y)
    if y+1 == self.dimensions_[0]:
      return '-'
    q = self.at(x, y+1)
    ch1 = 'v' if p.sides_[2] == 'O' else '^'
    ch2 = 'v' if q.sides_[0] == 'I' else '^'
    assert ch1 == ch2, (ch1, ch2)
    return ch1

  def setEdgeToRightOf(self, x, y, what):
    p = self.at(x, y)
    q = self.at(x+1, y)
    p.sides_[1] = ('O' if what else 'I')
    q.sides_[3] = ('I' if what else 'O')

  def setEdgeBelow(self, x, y, what):
    p = self.at(x, y)
    q = self.at(x, y+1)
    p.sides_[2] = ('O' if what else 'I')
    q.sides_[0] = ('I' if what else 'O')

  def rotateLeft(self):
    (n, m) = self.dimensions_
    self.dimensions_ = (m, n)
    for p in self.pieces_:
      (x, y) = p.position_
      p.position_ = (y, m-x-1)
      p.rotateLeft()

  def rotateRight(self):
    (n, m) = self.dimensions_
    self.dimensions_ = (m, n)
    for p in self.pieces_:
      (x, y) = p.position_
      p.position_ = (n-y-1, x)
      p.rotateRight()

  def isSuperimposableOn(self, jig):
    # print('Asking whether %s is superimposable on %s...' % (self, jig))
    (n, m) = self.dimensions_
    if jig.dimensions_ != (n, m):
      return False
    for x in range(m):
      for y in range(n):
        if not self.at(x, y).isSuperimposableOn(jig.at(x, y)):
          return False
    return True

  def isEquivalentTo(self, jig):
    if self.isSuperimposableOn(jig):
      return True
    jig2 = copy.deepcopy(jig)
    jig2.rotateLeft()
    if self.isSuperimposableOn(jig2):
      return True
    jig2.rotateLeft()
    if self.isSuperimposableOn(jig2):
      return True
    jig2.rotateLeft()
    if self.isSuperimposableOn(jig2):
      return True
    return False

  def __str__(jig):
    (n, m) = jig.dimensions_
    result = (('+-' * m) + '+\n')
    for y in range(n):
      result += '|'
      for x in range(m):
        result += jig.at(x,y).name_
        result += jig.edgeToRightOf(x,y)
      result += '\n+'
      for x in range(m):
        result += jig.edgeBelow(x,y)
        result += '+'
      result += '\n'
    return result


def yield_unfiltered_jigsaws(n, m):
  jig = SolvedJigsaw(n, m)
  jig.pieces_ = [Piece(x,y) for y in range(n) for x in range(m)]
  for i in range(m*n):
    jig.pieces_[i].name_ = piecenames[i]
  edgecount = ((m-1) * n) + (m * (n-1))
  for perm in itertools.product([True, False], repeat=edgecount):
    flags = list(perm)
    for x in range(m):
      for y in range(n):
        if (x < m-1): jig.setEdgeToRightOf(x, y, flags.pop())
        if (y < n-1): jig.setEdgeBelow(x, y, flags.pop())
    yield jig


def print_jigsaw(jig):
  (n, m) = jig.dimensions_
  print(('+-' * m) + '+')
  for y in range(n):
    print('|', end='')
    for x in range(m):
      print(jig.at(x,y).name_ + jig.edgeToRightOf(x,y), end='')
    print('\n+', end='')
    for x in range(m):
      print(jig.edgeBelow(x,y) + '+', end='')
    print('')


def count_blank_jigsaws_with_unique_pieces(n, m):
  jigs = []
  for jig in yield_unfiltered_jigsaws(n,m):
    if jig.hasUniquePieces():
      if not any(jig.isEquivalentTo(j) for j in jigs):
        jigs.append(copy.deepcopy(jig))
  print('There are %d blank %dx%d jigsaws with no repeated pieces.' % (len(jigs), n, m))

(n, m) = int(sys.argv[1]), int(sys.argv[2])
count_blank_jigsaws_with_unique_pieces(n, m)
