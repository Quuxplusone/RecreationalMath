
import sys

def fastest_chain(cycles, available_inputs, target):
  if target in available_inputs:
    return cycles
  outputs = [a+b for a in available_inputs for b in available_inputs]
  return fastest_chain(cycles + 1, set(available_inputs) | set(outputs), target)

for target in range(2, 100):
  print(fastest_chain(0, [1], target), end=" ")
  sys.stdout.flush()
