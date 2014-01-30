#!/usr/bin/python

import math

class SizeError(Exception):
  def __init__  (self, value): self.value = value
  def __str__   (self): return repr(self.value)


def ring (points = 3):

  if points < 3:
    raise SizeError("points needs a value larger than 2")

  for k in range(points):
    theta = 2 * math.pi * k/points
    fi = 2 * math.pi * (k+1)/points

    p = [ math.cos(theta), math.sin(theta) ]
    q = [ math.cos(fi), math.sin(fi) ]
    p = map(lambda x: round(x, 2), p)
    q = map(lambda x: round(x, 2), q)

    print p
    print q
    print




ring(100)
