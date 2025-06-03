import sys

def merge_two_lines(s, t):
  ti = None
  n = len(s)
  assert len(t) == n
  if 'O' in s:
    for i in range(n):
      if s[i] == t[i]:
        pass
      elif s[i] == 'O' and t[i] == '.':
        pass
      elif s[i] == '.' and t[i] == 'o' and ti is None:
        ti = i
      else:
        return None
  else:
    si = None
    for i in range(n):
      if s[i] == t[i]:
        pass
      elif s[i] == 'o' and t[i] == '.' and si is None:
        si = i
      elif s[i] == '.' and t[i] == 'o' and ti is None:
        ti = i
      else:
        return None
    if si is None:
      return None
    s = s[:si] + 'O' + s[si+1:]
  if ti is None:
    return None
  s = s[:ti] + 'O' + s[ti+1:]
  return s


lines = list(line.strip() for line in sys.stdin.readlines())
n = len(lines[0])
assert all(len(s) == n for s in lines)
while lines:
  s = lines.pop()
  assert all(ch in 'Xxo.' for ch in s)
  # Find other lines which differ from s in just two characters
  i = 0
  while i < len(lines):
    m = merge_two_lines(s, lines[i])
    if m is not None:
      s = m
      lines = lines[:i] + lines[i+1:]
    else:
      i += 1
  print(s)
