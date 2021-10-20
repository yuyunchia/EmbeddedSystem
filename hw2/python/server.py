from wifisocket import WiFiSocket

def main():
    wifisocket = WiFiSocket()
    wifisocket.start()
    wifisocket.server_listen()
    
if __name__ == "__main__":
    main()

    

