#include "NetworkInterface.h"
#include "SocketAddress.h"
#include "mbed.h"
#include "nsapi_types.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"
#include <cstdio>
#include <string>

#define HOST "0.0.0.0"
#define PORT 4000

class WiFiSocket{
public:
    WiFiSocket(void): MAX_BUFFER_SIZE(1024){
        get_data = nullptr;
    }
    WiFiSocket(const char* (*sensor_reader)(void), const int max_buffer_size): MAX_BUFFER_SIZE(max_buffer_size){
        get_data = sensor_reader;
    }
    ~WiFiSocket(){
        if (_net) _net->disconnect();
    }
    void run(){
        printf("WiFi connecting...\n");
        if (!_net){
            printf("Error, no network interface found.\n");
            return;
        }
        if (_net->wifiInterface())
            scan_wifi();

        // connect net work
        if (!connect_network())
            return;
        
        print_network_info();
        // open socket connection
        if (!open_socket())
            return;
        // connect to host
        if (!connect_to_host())
            return;

        // TODO: read sensor and continuously send data to server
    }

private:
    void scan_wifi(void){
        WiFiInterface *wifi = _net->wifiInterface();
        WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

        /* scan call returns number of access points found */
        int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

        if (result <= 0) {
            printf("WiFiInterface::scan() failed with return value: %d\n", result);
            return;
        }

        printf("%d networks available:\r\n", result);
        for (int i = 0; i < result; i++) {
            printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                   ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                   ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                   ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                   ap[i].get_rssi(), ap[i].get_channel());
        }
        printf("\n");
    }
    bool connect_network(void){
        printf("Connecting to the network...\n");
        nsapi_size_or_error_t result = _net->connect();
        if (result != 0){
            printf("Error, _net->connect() returned: %d\n", result);
            return false;
        }
        return true;
    }
    void print_network_info(void){
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }
    bool open_socket(void){
        nsapi_size_or_error_t result = socket.open(_net);
        if (result != 0){
            printf("Error, socket.open() returned: %d\n", result);
            return false;
        }
        return true;
    }
    bool connect_to_host(void){
        SocketAddress address;
        if (!resolve_hostname(address))
            return false;
        address.set_port(REMOTE_PORT);
        printf("Opening connectionn to remote port: %d\n", REMOTE_PORT);
        nsapi_size_or_error_t result = socket.connect(address);
        if (result != 0){
            printf("Error, socket.connect() returned :%d\n", result);
            return false;
        }
        return true;
    }
    bool resolve_hostname(SocketAddress &address){
        const char hostname[] = HOST;

        /* get the host address */
        printf("\nResolve hostname %s\n", hostname);
        nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
        if (result != 0) {
            printf("Error, gethostbyname(%s) returned: %d\n", hostname, result);
            return false;
        }

        printf("%s address is %s\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );

        return true;
    }

    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t REMOTE_PORT = PORT;
    const char* (*get_data)(void);
    const int MAX_BUFFER_SIZE;
    NetworkInterface *_net;
    TCPSocket socket;
    SocketAddress address;
};

int main(void){
    printf("Start WiFiSocket\n");
    WiFiSocket *wifisocket = new WiFiSocket();  // need to pass sensor_reader
    wifisocket->run();

    return 0;
}