from wifisocket import WiFiSocket

def display_func(data):
    # print(data)
    pass

def main():
    wifisocket = WiFiSocket()
    wifisocket.start()
    wifisocket.server_listen(display_func=display_func)

if __name__ == "__main__":
    main()