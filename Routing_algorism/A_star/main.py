# main.py

import numpy as np
import matplotlib.pyplot as plt

from matplotlib.patches import Rectangle

import random_map
import a_star

plt.figure(figsize=(5, 5))

map = random_map.RandomMap()

ax = plt.gca() #获取当前坐标轴
ax.set_xlim([0, map.size]) #设置x轴范围
ax.set_ylim([0, map.size]) #设置y轴范围

for i in range(map.size): # Draw the map
    for j in range(map.size):
        if map.IsObstacle(i,j):
            rec = Rectangle((i, j), width=1, height=1, color='gray') 
            ax.add_patch(rec)
        else:
            rec = Rectangle((i, j), width=1, height=1, edgecolor='gray', facecolor='w') 
            ax.add_patch(rec)

rec = Rectangle((0, 0), width = 1, height = 1, facecolor='b') #起点
ax.add_patch(rec)

rec = Rectangle((map.size-1, map.size-1), width = 1, height = 1, facecolor='r') #终点
ax.add_patch(rec)

plt.axis('equal')
plt.axis('off')
plt.tight_layout()
# plt.show()

a_star = a_star.AStar(map)
a_star.RunAndSaveImage(ax, plt)

