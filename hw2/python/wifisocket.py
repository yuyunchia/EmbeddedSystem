import socket
import netifaces as ni
import matplotlib.pyplot as plt

from config import config

class WiFiSocket():
    def __init__(self) -> None:
        self.bind_ip = ni.ifaddresses('en0')[ni.AF_INET][0]['addr']
        self.bind_port = config["bind_port"]
        self.socket_available = config["socket_available"]
        self.buffer_size = config["buffer_size"]
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __data_parser(self, data):
        data = data.split()
        for i in range(len(data)):
            data[i] = int(data[i])
        return data

    def start(self):
        self.server.bind((self.bind_ip, self.bind_port))
        self.server.listen(self.socket_available)
        print("Listening on %s:%d, with maximum socket available %d" % (self.bind_ip, self.bind_port, self.socket_available))

    def server_listen(self):
        while True:
            print("Server Ready")
            client, addr = self.server.accept()
            print("Connected by ", addr)

            # display
            plt.ion()
            y1, y2, y3 = [0], [0], [0]

            while True:
                try:
                    data = str(client.recv(self.buffer_size), encoding='utf-8')
                    data = self.__data_parser(data)
                    print("Data from client: ", data)

                    if data == "":
                        break
                    if data == "close":
                        self.server.close()
                        return
                    
                    if config["display"]:
                        if len(y1) != 1:
                            plt.clf()
                        y1.append(data[0])
                        y2.append(data[1])
                        y3.append(data[2])

                        plt.plot(y1, color="red")
                        plt.plot(y2, color="blue")
                        plt.plot(y3, color="green")
                        plt.legend(['ax','ay','az'])
                        plt.draw()
                        plt.pause(0.001)

                except UnicodeDecodeError:
                    print("Data format error")
                    client.close()
                    break
            i = input("Connection close, end server?(y/n)")
            if i == "y":
                break
        self.server.close()