#!/usr/bin/env python

# https://oeis.org/A373811
# a(0) = 0
# a(n) = the minimum number of lines needed to pass through all the points (i, a(i)) for 0 <= i < n
#
# a(n) = 0, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 8, 8, 8, ...

# Represent a point as (x,y)
# Represent a line as (p1, p2)

import matplotlib
import matplotlib.pyplot as plt

MAXX = 40
MAXY = 10

def point_is_on_line(p0, l):
  p1 = l[0]
  p2 = l[1]
  if p0 in [p1, p2]:
    return True
  if p0[0] == p1[0]:
    return p0[0] == p2[0]
  elif p0[0] == p2[0]:
    return p0[0] == p1[0]
  m01 = (p0[1] - p1[1]) * (p0[0] - p2[0])
  m02 = (p0[1] - p2[1]) * (p0[0] - p1[0])
  return m01 == m02

def line_between_points(p, q):
  return (p, q)

def lines_through_points(ps, maxlen):
  # We must have a line through ps[0]. Where else?
  # Look for solutions in `maxlen` or fewer lines; otherwise return None.
  if not ps:
    return []
  if maxlen == 0:
    return None
  if len(ps) == 1:
    return [(ps[0], ps[0])]
  best = None
  for i in range(1, len(ps)):
    if ps[0][1] == ps[i][1] and (i != 1):
      # Avoid looking redundantly at lines that are strictly horizontal.
      continue
    line = line_between_points(ps[0], ps[i])
    remaining_ps = [p for p in ps if not point_is_on_line(p, line)]
    lines = lines_through_points(remaining_ps, maxlen - 1)
    if lines is not None:
      if best is None or len(lines) < len(best):
        maxlen = len(lines)
        best = lines + [line]
  return best

def draw_plot(points, lines):
  plt.clf()
  plt.gca().xaxis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))
  plt.gca().yaxis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))
  plt.gca().set_xlim([0,MAXX])
  plt.gca().set_ylim([0,MAXY])
  plt.gca().set_prop_cycle(None)
  for line in lines:
    p = [p for p in points if point_is_on_line(p, line)][0]
    q = [q for q in points if point_is_on_line(q, line)][-1]
    plt.plot([p[0], q[0]], [p[1], q[1]])
  plt.plot([p[0] for p in points], [p[1] for p in points], 'ok')
  plt.savefig('A373811-%d.png' % len(points))


if __name__ == '__main__':
  points = [(0,0), (1,1)]
  current = 2
  for i in range(MAXX):
    lines = lines_through_points(points, current+1)
    draw_plot(points, lines)
    current = len(lines)
    newpoint = (len(points), current)
    points += [newpoint]
    print(points)
