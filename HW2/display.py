# importing all necessary libraries
from itertools import count
import random
import matplotlib
from matplotlib import animation
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


# import threading
# import time


class DisplayWindow():
    def __init__(self) -> None:
        self.len = 0
        self.ax = []
        self.ay = []
        self.az = []

      #   self.len = 40
      #   self.ax = [random.randint(-20, 4)+(points**1.88)/(random.randint(13, 14))
      # for points in range(0, 80, 2)]
      #   self.ay = [random.randint(0, 9)+(points**1.9)/(random.randint(9, 11))
      # for points in range(0, 80, 2)]
      #   self.az = [random.randint(-10, 10)-(points**1.4)/(random.randint(9, 12))
      # for points in range(0, 80, 2)]

      

    def update(self, data): # data is like (int  , int  , int )
        self.ax.append(data[0])
        self.ay.append(data[1])
        self.az.append(data[2])

        self.len += 1
    
    def show(self):
        print("displaywindow start")

        # subplots() function you can draw
        # multiple plots in one figure
        fig, axes = plt.subplots(nrows=1, ncols=1, figsize=(10, 5)) ## nrows, ncols 設不為1時有error 
        
        # set limit for x and y axis
        #axes.set_ylim(-100, 500)
        #axes.set_xlim(0, 250)
        
        # style for plotting line
        plt.style.use("ggplot")
        x1, y1, y2, y3 = [], [], [], []
        myvar = count(0, 2)
        
        def animate(i):

            if (i >= self.len):
                return

            x1.append(next(myvar))
            y1.append((self.ax[i]))
            y2.append((self.ay[i]))
            y3.append((self.az[i]))
        
            axes.plot(x1, y1, color="red")
            axes.plot(x1, y2, color="blue")
            axes.plot(x1, y3, color="green")

            axes.legend(['ax','ay','az'])
        
                
        # set ani variable to call the
        # function recursively
        anim = FuncAnimation(fig, animate, interval=30)


        #plt.tight_layout()
        plt.show()

       
