# Draw an SVG image of the mathematical object described in
# https://math.stackexchange.com/questions/4492575/infinite-sum-of-an-inverse-trig-expression
# https://achromath.substack.com/p/convergent-chain-curling-part-2

import math

# This factor makes the infinite series of circles approach the
# point of tangency of the first circle.
# However, https://achromath.substack.com/p/convergent-chain-curling-part-2
# points out that many of these circles, starting with circle 23,
# actually *overlap* circle 1.
factor = 0.8239305157

# According to https://achromath.substack.com/p/convergent-chain-curling-part-2
# this factor makes circle 29 tangent to circle 1, and then circle 30
# recedes away from circle 1 again.
factor = 0.82296


nrs = [factor ** i for i in range(50)]

def theta(a, b):
  n = (a+1)**2 + (b+1)**2 - (a+b)**2
  d = 2*(a+1)*(b+1)
  return math.acos(n / d)

def center_of_circle(i):
  if (i == 0):
    return (0, 0)
  t = 0.0
  for i in range(2, i+1):
    t += theta(nrs[i-1], nrs[i])
  r = nrs[0] + nrs[i]
  return (r * math.cos(t), r * -math.sin(t))

def drawcircle(i):
  cx, cy = center_of_circle(i)
  print('  <circle fill="none" stroke-width="%g" stroke="#000" cx="%g" cy="%g" r="%g"/>' % (0.005, cx, cy, nrs[i]))

print('<svg xmlns="http://www.w3.org/2000/svg" width="100%" viewBox="-3 -3 6 6" style="background-color: white">')
for i in range(len(nrs)):
  drawcircle(i)
print('</svg>')
