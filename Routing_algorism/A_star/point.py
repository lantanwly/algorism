# point.py

import sys
#用來描述二維平面上的點
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.cost = sys.maxsize