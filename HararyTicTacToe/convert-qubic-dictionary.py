import sys

# tail +52 qubic.dictionary | python convert-qubic-dictionary.py | python simple-compress-oracle.py | sort > oracle.qubic.txt

for line in sys.stdin.readlines():
  parts = line.split()
  if len(parts) == 1:
    assert parts[0] == '0'
    print('X' + ('.' * 63))
  else:
    assert len(parts) == 2
    s = parts[0]
    m = int(parts[1])
    result = ''
    i = 0
    while i < len(s):
      if s[i] == 'x':
        result += 'x'
      elif s[i] == 'o':
        result += 'o'
      elif s[i+1].isdigit():
        result += int(s[i:i+2]) * '.'
        i += 1
      else:
        result += int(s[i:i+1]) * '.'
      i += 1
    result += (64-len(result)) * '.'
    assert result[m] == '.'
    result = result[:m] + 'X' + result[m+1:]
    print(result)
