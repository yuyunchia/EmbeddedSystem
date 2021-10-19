import socket
import netifaces as ni
from config import config

class WiFiSocket():
    def __init__(self) -> None:
        self.bind_ip = ni.ifaddresses('en0')[ni.AF_INET][0]['addr']
        self.bind_port = config["bind_port"]
        self.socket_available = config["socket_available"]
        self.buffer_size = config["buffer_size"]
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __data_parser(self, data):
        _data = data.split()
        _data = (int(_data[0]), int(_data[1]), int(_data[2]))
        print("Data to display: ", _data)
        return 

    def start(self):
        self.server.bind((self.bind_ip, self.bind_port))
        self.server.listen(self.socket_available)
        print("Listening on %s:%d, with maximum socket available %d" % (self.bind_ip, self.bind_port, self.socket_available))

    def server_listen(self, display_func):
        while True:
            print("Server Ready: ")
            client, addr = self.server.accept()
            print("Connected by ", addr)
            while True:
                try:
                    data = str(client.recv(self.buffer_size), encoding='utf-8')
                    print("Data from client: %s" % (data))
                    data = self.__data_parser(data)

                    if data == "":
                        break
                    if data == "close":
                        self.server.close()
                        return
                    
                    display_func(data)
                except UnicodeDecodeError:
                    print("Data format error")
                    client.close()
                    break
            i = input("Connection close, end server?(y/n)")
            if i == "y":
                break
        self.server.close()