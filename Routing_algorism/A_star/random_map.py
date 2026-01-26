# random_map.py

import numpy as np

import point

class RandomMap:
    def __init__(self, size=50):#構造函數，地圖大小預設為50x50
        self.size = size
        self.obstacle = size//8 #障礙物數量為地圖大小的八分之一
        self.GenerateObstacle() #調用GenerateObstacle方法隨機生成障礙物

    def GenerateObstacle(self):
        self.obstacle_point = []
        self.obstacle_point.append(point.Point(self.size//2, self.size//2))
        self.obstacle_point.append(point.Point(self.size//2, self.size//2-1))


        # Generate an obstacle in the middle
        for i in range(self.size//2-4, self.size//2): #在地圖中生成一個斜着的障礙物
            self.obstacle_point.append(point.Point(i, self.size-i))
            self.obstacle_point.append(point.Point(i, self.size-i-1))
            self.obstacle_point.append(point.Point(self.size-i, i))
            self.obstacle_point.append(point.Point(self.size-i, i-1))

        for i in range(self.obstacle-1): #隨機生成其他障礙物
            x = np.random.randint(0, self.size)
            y = np.random.randint(0, self.size)
            self.obstacle_point.append(point.Point(x, y))

            if (np.random.rand() > 0.5): # Random boolean 障礙物的方向也是隨機的
                for l in range(self.size//4):
                    self.obstacle_point.append(point.Point(x, y+l))
                    pass
            else:
                for l in range(self.size//4):
                    self.obstacle_point.append(point.Point(x+l, y))
                    pass

    def IsObstacle(self, i ,j): #判斷給定的座標(i, j)是否為障礙物
        for p in self.obstacle_point:
            if i==p.x and j==p.y:
                return True
        return False