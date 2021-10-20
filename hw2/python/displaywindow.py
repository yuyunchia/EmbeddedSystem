# importing all necessary libraries
from itertools import count
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

class DisplayWindow():
    def __init__(self) -> None:
        self.len = 0
        self.ax = []
        self.ay = []
        self.az = []

    def update(self, data): # data is like (int  , int  , int )
        self.ax.append(data[0])
        self.ay.append(data[1])
        self.az.append(data[2])

        self.len += 1
    
    def show(self):
        print("displaywindow start")

        fig, axes = plt.subplots(nrows=1, ncols=1, figsize=(10, 5)) ## nrows, ncols 設不為1時有error 
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
        anim = FuncAnimation(fig, animate, interval=30)


        #plt.tight_layout()
        plt.show()
        exit(0)
       
