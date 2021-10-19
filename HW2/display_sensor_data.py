from wifisocket import WiFiSocket

from display import DisplayWindow



import random

import threading
import time



displaywindow = DisplayWindow()

def updatedata():
    t = 10000
    while(t>0):
        #t = t-1
        data = ( random.randint(-10, 10), random.randint(-10, 10), random.randint(-10, 10) )
        displaywindow.update(data)
        time.sleep(0.001)


t = threading.Thread(target = updatedata)

t.start()




def display_func(data):
    # print(data)
    pass




def main():

    # display_thread = threading.Thread(target = displaywindow.show)
    # display_thread.start()

    

    displaywindow.show()

    


    # wifisocket = WiFiSocket()
    # wifisocket.start()
    # wifisocket.server_listen(display_func=display_func)

    

    #displaywindow.show()

        


if __name__ == "__main__":
    main()

