import math

minsofar = 70
for a in range(1, 3000):
  for b in range(1,a):
    if math.lcm(a,b)/b >= minsofar:
      continue
    for c in range(1,b):
      if math.lcm(a,b,c)/c >= minsofar:
        continue
      # The sum of the squares of all four bends is half the square of their sum.
      A = 1
      B = -2*(a+b+c)
      C = (a**2 + b**2 + c**2) - 2*(a*b + a*c + b*c)
      DISCRIMINANT = B**2 - 4*A*C
      if DISCRIMINANT < 0 or int(math.sqrt(DISCRIMINANT))**2 != DISCRIMINANT:
        continue
      d1 = (-B + math.sqrt(DISCRIMINANT)) / 2*A
      d2 = (-B - math.sqrt(DISCRIMINANT)) / 2*A
      # We have a couple of solutions: (a,b,c,d1) and (a,b,c,d2).
      # These are the bends. To get the radii, we invert them.
      # So we expect that d (the middle circle) will have the smallest radius,
      # and c will have the largest radius.
      if d1 > a and int(d1) == d1:
        g1 = math.lcm(a,b,c,int(d1))
        if (g1/c < minsofar):
          minsofar = g1/c
          print(a,b,c,d1,' => ',g1/a, g1/b, g1/c, g1/d1)
      if d2 > a and int(d2) == d2:
        g2 = math.lcm(a,b,c,int(d2))
        if (g2/c < minsofar):
          minsofar = g2/c
          print(a,b,c,d2,' => ',g2/a, g2/b, g2/c, g2/d2)
