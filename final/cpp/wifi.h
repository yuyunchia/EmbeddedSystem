#include "NetworkInterface.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "mbed.h"
#include <cstdio>
#include <vector>
#include "nsapi_types.h"
#include "accelerator.h"

#define MBED_CONF_APP_WIFI_SSID      "\"fy1928\""
#define MBED_CONF_APP_WIFI_PASSWORD  "\"f1127EE1688\""
#define DESTINATION_IP   "192.168.43.43"
#define DESTINATION_PORT 4000

class WiFi{
public:
    WiFi(
        const char destination_ip[16],
        const int &port
    ):_wifi(NetworkInterface::get_default_instance()), PORT(port){
        sprintf(DEST_IP, "%s", destination_ip);
    }

    bool connect(void){
        if (!_wifi){
            printf("Error: no WiFiInterface found.\r\n");
            return false;
        }
        printf("Connecting to %s...\r\n", MBED_CONF_APP_WIFI_SSID);
        nsapi_error_t result = _wifi->connect();
        if (result != 0){
            printf("\nConnection error: %d\r\n", result);
            return false;
        }

        SocketAddress a;
        _wifi->get_ip_address(&a);
        printf("IP: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
        result = _socket.open(_wifi);
        if (result != 0) {
            printf("Error! socket.open() returned: %d\n", result);
            _socket.close();
            return false;
        }

        _a.set_ip_address(DEST_IP);
        _a.set_port(PORT);
        result = _socket.connect(_a);
        if (result != 0) {
            printf("Error! socket.connect() returned: %d\n", result);
            _socket.close();
            return false;
        }

        printf("\nSuccess\n");
        printf("MAC: %s\n", _wifi->get_mac_address());
        _wifi->get_ip_address(&a);
        printf("IP: %s\n", a.get_ip_address());
        _wifi->get_netmask(&a);
        printf("Netmask: %s\n", a.get_ip_address());
        _wifi->get_gateway(&a);
        printf("Gateway: %s\n", a.get_ip_address());
        printf("Connected to IP: %s, Port: %d\n", DEST_IP, PORT);

        return true;
    }
    void send_data(vector<vector<int16_t>>&data){
        char buffer[1024];
        sprintf(buffer, "");
        for (int i = 0; i < data.size(); i++)
            for (int j = 0; j < data[i].size(); j++)
                sprintf(buffer, "%s%d ", buffer, data[i][j]);

        nsapi_size_or_error_t byte_sent = _socket.sendto(_a, buffer, strlen(buffer));
        printf("Bytes sent: %d/%d\r\n", byte_sent, strlen(buffer));
    }

private:
    NetworkInterface *_wifi;
    char DEST_IP[16];
    int PORT;
    TCPSocket _socket;
    SocketAddress _a;
};