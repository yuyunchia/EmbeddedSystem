import socket
import time
import netifaces as ni

def main():
    # bind_ip = "192.168.43.215"
    bind_ip = ni.ifaddresses('en0')[ni.AF_INET][0]['addr']
    bind_port = 4000
    socket_available = 10
    buffer_size = 2048
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server.bind((bind_ip, bind_port))
    server.listen(socket_available)
    print("Listening on %s:%d, with maximum socket available %d" % (bind_ip, bind_port, socket_available))

    with open("../../data/4_data.txt", "w") as data_file:
        while True:
            print("Server ready")
            client, addr = server.accept()
            print("Connected by ", addr)

            while True:
                time.sleep(0.001)
                try:
                    data = str(client.recv(buffer_size), encoding='utf-8')
                    print("Data from client: ", data)
                    choise = input("Choise [l/r/n/s]:")
                    if choise == "close":
                        client.close()
                        break
                    if choise not in ['l', 'r', 'n', 's']:
                        print("Don't have the choise: %s" % choise)
		            
                    data_file.write("%s -> %s\n" % (data, choise))
                except UnicodeDecodeError:
                    print("Data format error")
                    client.close()
                    break
            server.close()


if __name__ == "__main__":
    main()
