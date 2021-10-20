from wifisocket import WiFiSocket
from display import DisplayWindow
import random
import threading
import time


from itertools import count
import random
import matplotlib
from matplotlib import animation
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation



displaywindow = DisplayWindow()

displaywindow.id = random.randint(-10, 10)

print("in server.py display.id = ", displaywindow.id)



# len = 0
# ax = []
# ay = []
# az = []


def updatedata():
    t = 10000
    while(t>0):
        #t = t-1
        data = ( random.randint(-10, 10), random.randint(-10, 10), random.randint(-10, 10) )
        displaywindow.update(data)

        # ax.append(data[0])
        # ay.append(data[1])
        # az.append(data[2])

        time.sleep(0.001)







def display_func(data):
    # print(data)
    #newinput = (data[0], data[1], data[2])

    print("call display_func")
    # ax.append(data[0])
    # ay.append(data[1])
    # az.append(data[2])

    len += 1
    time.sleep(0.001)

    ##pass



def Wifi():
    wifisocket = WiFiSocket()
    wifisocket.start()
    wifisocket.server_listen(display_func=display_func)







    

def main():

    # simulate = threading.Thread(target = updatedata)
    # simulate.start()
    
    display_thread = threading.Thread(target = displaywindow.show)
    display_thread.start()

    socket = WiFiSocket()
    socket.start()

    t = threading.Thread(target = socket.server_listen(displaywindow=displaywindow), args=())
    t.start()

    
    
if __name__ == "__main__":
    main()

