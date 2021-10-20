import socket

def main():
    HOST = "127.0.0.1"
    PORT = 65431

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    while True:
        cmd = input("Input msg: ")
        s.send(str.encode(cmd))

if __name__ == "__main__":
    main()