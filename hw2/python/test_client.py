import socket
import netifaces as ni

def main():
    HOST = ni.ifaddresses('en0')[ni.AF_INET][0]['addr']
    PORT = 4000

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    while True:
        cmd = input("Input msg: ")
        s.send(str.encode(cmd))

if __name__ == "__main__":
    main()