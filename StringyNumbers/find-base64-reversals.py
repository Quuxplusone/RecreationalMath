
import base64
import sys

letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
letters += letters.lower()

# Make a list of candidate three-letter words.
# 
words = [a+b+c for a in letters for b in letters for c in letters]

# Make a list of candidate six-letter words.
# See README.md.
#
words = []
for i in range(0xFFF):
  cdef = i & 0xF
  gh = (i >> 4) & 0x3
  ij = (i >> 6) & 0x3
  klmn = (i >> 8) & 0xF
  newword = chr(0x40 + (cdef<<2) + gh) + chr(0x51 + (ij << 2)) + chr(0x50 + klmn) + chr(0x40 + (klmn << 2) + ij) + chr(0x51 + (gh<<2)) + chr(0x50+cdef)
  if all(c in letters for c in newword):
    words += [newword]

# Filter "words" to just those that are base64 reversals, and print those.
#
for w in words:
  b = base64.b64encode(w.encode()).decode()
  rb = ''.join(reversed(b))
  try:
    rw = base64.b64decode(rb).decode()
  except:
    rw = '?' * len(w)
  if rw in words:
    print('%s b64e=> %s rev=> %s b64d=> %s' % (w, b, rb, rw))
