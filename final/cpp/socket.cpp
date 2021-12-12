/* Sockets Example
 * Copyright (c) 2016-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "nsapi_types.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <chrono>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <cstdint>
#include <cstdio>

#define MAX_CALLIBRATE_NUM 1024
#define SAMPLE_TIME_INTERVAL 50

class Accelerator{
public:
    Accelerator(
        const int &_sequense_length = 8,       // number of accelerate/gyro XYZ sent everytime
        const bool &enable_multi_thread=false  // if true, Accelerator won't print any information
    ){
        ACCE_X_OFFSET = ACCE_Y_OFFSET = ACCE_Z_OFFSET = 0.0;
        GYRO_X_OFFSET = GYRO_Y_OFFSET = GYRO_Z_OFFSET = 0.0;
        has_multi_thread = enable_multi_thread;
        inited = false;
        sequense_length = _sequense_length;
        if (sequense_length <= 0){
            if (!has_multi_thread)
                printf("Cannot has a non-positive sequence length!\r\n");
            return;
        }
    }
    // Initializing all the hardwares
    void init(const int &acce_sample_num, const int &gyro_sample_num){
        inited = true;
        BSP_ACCELERO_Init();
        BSP_GYRO_Init();

        inited = calibrate_accelerator(acce_sample_num) & calibrate_gyroscope(gyro_sample_num);

        if (!inited){
            if (!has_multi_thread)
                printf("Accelerator initiation failed!\r\n");
            return;
        }
        
        acce_x = new float[sequense_length];
        acce_y = new float[sequense_length];
        acce_z = new float[sequense_length];
        gyro_x = new float[sequense_length];
        gyro_y = new float[sequense_length];
        gyro_z = new float[sequense_length];

        read_data();
    }
    // Calibrating the accelerator
    bool calibrate_accelerator(const int &sample_num){
        if (!inited){
            if (!has_multi_thread)
                printf("Class Accelerator hasn't initialize, calling Accelerator::init() to initialize the states\r\n");
            return false;
        }
        if (sample_num < 0){
            if (!has_multi_thread)
                printf("Can't sample data for negative times!\r\n");
            return false;
        }
        if (sample_num > MAX_CALLIBRATE_NUM){
            if (!has_multi_thread)
                printf("Sampling for %d times overflows the maximum sample num!\r\n", sample_num);
            return false;
        }

        ACCE_X_OFFSET = ACCE_Y_OFFSET = ACCE_Z_OFFSET = 0.0;
        int16_t acc_raw_data[3];
        for (int i = 0; i < sample_num; i++){
            BSP_ACCELERO_AccGetXYZ(acc_raw_data);
            ACCE_X_OFFSET += (float)acc_raw_data[0];
            ACCE_Y_OFFSET += (float)acc_raw_data[1];
            ACCE_Z_OFFSET += (float)acc_raw_data[2];

            ThisThread::sleep_for(chrono::milliseconds(SAMPLE_TIME_INTERVAL));
        }

        ACCE_X_OFFSET /= sample_num;
        ACCE_Y_OFFSET /= sample_num;
        ACCE_Z_OFFSET /= sample_num;

        if (!has_multi_thread){
            printf("Successfully set accelerator offsets.\r\n");
            printf("ACCE_X_OFFSET: %d\r\n", int16_t(ACCE_X_OFFSET));
            printf("ACCE_Y_OFFSET: %d\r\n", int16_t(ACCE_Y_OFFSET));
            printf("ACCE_Z_OFFSET: %d\r\n", int16_t(ACCE_Z_OFFSET));
        }

        return true;
    }
    // Calibraing the gyroscope
    bool calibrate_gyroscope(const int &sample_num){
        if (!inited){
            if (!has_multi_thread)
                printf("Class Accelerator hasn't initialize, calling Accelerator::init() to initialize the states\r\n");
            return false;
        }
        if (sample_num < 0){
            if (!has_multi_thread)
                printf("Can't sample data for negative times!\r\n");
            return false;
        }
        if (sample_num > MAX_CALLIBRATE_NUM){
            if (!has_multi_thread)
                printf("Sampling for %d times overflows the maximum sample num!\r\n", sample_num);
            return false;
        }

        GYRO_X_OFFSET = GYRO_Y_OFFSET = GYRO_Z_OFFSET = 0.0;
        float gyro_raw_data[3];
        for (int i = 0; i < sample_num; i++){
            BSP_GYRO_GetXYZ(gyro_raw_data);
            GYRO_X_OFFSET += gyro_raw_data[0];
            GYRO_Y_OFFSET += gyro_raw_data[1];
            GYRO_Z_OFFSET += gyro_raw_data[2];

            ThisThread::sleep_for(chrono::milliseconds(SAMPLE_TIME_INTERVAL));
        }

        GYRO_X_OFFSET /= sample_num;
        GYRO_Y_OFFSET /= sample_num;
        GYRO_Z_OFFSET /= sample_num;

        if (!has_multi_thread){
            printf("Successfully set gyroscope offsets.\r\n");
            printf("GYRO_X_OFFSET: %d\r\n", int16_t(GYRO_X_OFFSET));
            printf("GYRO_Y_OFFSET: %d\r\n", int16_t(GYRO_Y_OFFSET));
            printf("GYRO_Z_OFFSET: %d\r\n", int16_t(GYRO_Z_OFFSET));
        }

        return true;
    }
    // Update current data
    bool update_current(const int &update_num = 1){
        if (!inited){
            if (!has_multi_thread)
                printf("Class Accelerator hasn't initialize, calling Accelerator::init() to initialize the states\r\n");
            return false;
        }
        if (update_num <= 0){
            if (!has_multi_thread)
                printf("Can't update for a non-positive time!\r\n");
            return false;
        }
        if (update_num > sequense_length){
            if (!has_multi_thread)
                printf("Can't update over sequence length(%d) times!\r\n", sequense_length);
            return false;
        }

        for (int i = 0; i < sequense_length - update_num; i++){
            acce_x[i] = acce_x[i + update_num];
            acce_y[i] = acce_y[i + update_num];
            acce_z[i] = acce_z[i + update_num];
            gyro_x[i] = gyro_x[i + update_num];
            gyro_y[i] = gyro_y[i + update_num];
            gyro_z[i] = gyro_z[i + update_num];
        }

        read_data(sequense_length - update_num);
        
        return true;
    }
    // Get current data, need to decide how to pass data to others
    void get_data(vector<vector<int16_t>> &buffer, const bool &refresh=true){
        if (refresh)
            buffer.clear();
        for (int i = 0; i < sequense_length; i++){
            vector<int16_t> data;
            data.push_back((int16_t)acce_x[i]);
            data.push_back((int16_t)acce_y[i]);
            data.push_back((int16_t)acce_z[i]);
            data.push_back((int16_t)gyro_x[i]);
            data.push_back((int16_t)gyro_y[i]);
            data.push_back((int16_t)gyro_z[i]);
            buffer.push_back(data);
        }
    }
    // Print current data
    void print_data(void){
        if (has_multi_thread)
            return;
        printf("Seq accelerate_x: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(acce_x[i]));
        printf("\n");
        printf("Seq accelerate_y: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(acce_y[i]));
        printf("\n");
        printf("Seq accelerate_z: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(acce_z[i]));
        printf("\n");
        printf("Seq gyro_x: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(gyro_x[i]));
        printf("\n");
        printf("Seq gyro_y: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(gyro_y[i]));
        printf("\n");
        printf("Seq gyro_z: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(gyro_z[i]));
        printf("\n");
    }
    // Print current offset
    void print_offset(void){
        printf("Current offset:\r\n");
        printf("ACCE_X_OFFSET: %d\r\n", int16_t(ACCE_X_OFFSET));
        printf("ACCE_Y_OFFSET: %d\r\n", int16_t(ACCE_Y_OFFSET));
        printf("ACCE_Z_OFFSET: %d\r\n", int16_t(ACCE_Z_OFFSET));
        printf("GYRO_X_OFFSET: %d\r\n", int16_t(GYRO_X_OFFSET));
        printf("GYRO_Y_OFFSET: %d\r\n", int16_t(GYRO_Y_OFFSET));
        printf("GYRO_Z_OFFSET: %d\r\n", int16_t(GYRO_Z_OFFSET));
    }

private:
    void read_data(const int &start_from = 0){
        int16_t acce_raw[3];
        float gyro_raw[3];
        for (int i = start_from; i < sequense_length; i++){
            BSP_ACCELERO_AccGetXYZ(acce_raw);
            BSP_GYRO_GetXYZ(gyro_raw);

            acce_x[i] = float(acce_raw[0]) - ACCE_X_OFFSET;
            acce_y[i] = float(acce_raw[1]) - ACCE_Y_OFFSET;
            acce_z[i] = float(acce_raw[2]) - ACCE_Z_OFFSET;
            gyro_x[i] = gyro_raw[0] - GYRO_X_OFFSET;
            gyro_y[i] = gyro_raw[1] - GYRO_Y_OFFSET;
            gyro_z[i] = gyro_raw[2] - GYRO_Z_OFFSET;

            ThisThread::sleep_for(chrono::milliseconds(SAMPLE_TIME_INTERVAL));
        }
        return;
    }

    float ACCE_X_OFFSET, ACCE_Y_OFFSET, ACCE_Z_OFFSET;
    float GYRO_X_OFFSET, GYRO_Y_OFFSET, GYRO_Z_OFFSET;
    bool has_multi_thread;
    bool inited;
    int sequense_length;
    float *acce_x;
    float *acce_y;
    float *acce_z;
    float *gyro_x;
    float *gyro_y;
    float *gyro_z;
    
};

#if MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#endif
#endif // MBED_CONF_APP_USE_TLS_SOCKET

class SocketDemo {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;

#if MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    static constexpr size_t REMOTE_PORT = 4000; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    SocketDemo() : _net(NetworkInterface::get_default_instance()){}
    ~SocketDemo(){
        if (_net) {
            _net->disconnect();
        }
    }

    void run(){
        if (!_net) {
            printf("Error! No network interface found.\r\n");
            return;
        }
        if (_net->wifiInterface()) {
            wifi_scan();
        }
        printf("Connecting to the network...\r\n");
        nsapi_size_or_error_t result = _net->connect();
        if (result != 0) {
            printf("Error! _net->connect() returned: %d\r\n", result);
            return;
        }

        print_network_info();

        /* opening the socket only allocates resources */
        result = _socket.open(_net);
        if (result != 0) {
            printf("Error! _socket.open() returned: %d\r\n", result);
            return;
        }

#if MBED_CONF_APP_USE_TLS_SOCKET
        result = _socket.set_root_ca_cert(root_ca_cert);
        if (result != NSAPI_ERROR_OK) {
            printf("Error: _socket.set_root_ca_cert() returned %d\n", result);
            return;
        }
        _socket.set_hostname(MBED_CONF_APP_HOSTNAME);
#endif // MBED_CONF_APP_USE_TLS_SOCKET

        /* now we have to find where to connect */

        SocketAddress address;

        if (!resolve_hostname(address)) {
            return;
        }

        address.set_port(REMOTE_PORT);

        printf("Opening connection to remote port %d\r\n", REMOTE_PORT);

        result = _socket.connect(address);
        if (result != 0) {
            printf("Error! _socket.connect() returned: %d\r\n", result);
            return;
        }

        Accelerator accelerator;
        accelerator.init(128, 128);
        accelerator.print_offset();

        while (true) {
            vector<vector<int16_t>> data;
            accelerator.print_data();
            accelerator.update_current(4);
            accelerator.get_data(data);
            send_data(data);
            ThisThread::sleep_for(chrono::seconds(5));
        }

        printf("Demo concluded successfully \r\n");
    }

private:
    bool resolve_hostname(SocketAddress &address){
        const char hostname[] = MBED_CONF_APP_HOSTNAME;

        /* get the host address */
        printf("\nResolve hostname %s\r\n", hostname);
        nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
        if (result != 0) {
            printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
            return false;
        }

        printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );

        return true;
    }

    void send_data(vector<vector<int16_t>>&data){
        char buf[1024];
        sprintf(buf, "");
        for (int i = 0; i < data.size(); i++)
            for (int j = 0; j < data[i].size(); j++)
                sprintf(buf, "%s %d", buf, data[i][j]);
        
        sprintf(buf, "%s\r\n", buf);
        printf("Sending data: %s", buf);
        nsapi_size_or_error_t byte_sent = _socket.send(buf, strlen(buf));
        printf("Byte sent: %d/%d\r\n", byte_sent, strlen(buf));
    }
    // bool send_http_request()
    // {
    //     /* loop until whole request sent */
    //     const char buffer[] = "GET / HTTP/1.1\r\n"
    //                           "Host: ifconfig.io\r\n"
    //                           "Connection: close\r\n"
    //                           "\r\n";

    //     nsapi_size_t bytes_to_send = strlen(buffer);
    //     nsapi_size_or_error_t bytes_sent = 0;

    //     printf("\r\nSending message: \r\n%s", buffer);

    //     while (bytes_to_send) {
    //         bytes_sent = _socket.send(buffer + bytes_sent, bytes_to_send);
    //         if (bytes_sent < 0) {
    //             printf("Error! _socket.send() returned: %d\r\n", bytes_sent);
    //             return false;
    //         } else {
    //             printf("sent %d bytes\r\n", bytes_sent);
    //         }

    //         bytes_to_send -= bytes_sent;
    //     }

    //     printf("Complete message sent\r\n");

    //     return true;
    // }

    bool receive_http_response(){
        char buffer[MAX_MESSAGE_RECEIVED_LENGTH];
        int remaining_bytes = MAX_MESSAGE_RECEIVED_LENGTH;
        int received_bytes = 0;

        /* loop until there is nothing received or we've ran out of buffer space */
        nsapi_size_or_error_t result = remaining_bytes;
        while (result > 0 && remaining_bytes > 0) {
            result = _socket.recv(buffer + received_bytes, remaining_bytes);
            if (result < 0) {
                printf("Error! _socket.recv() returned: %d\r\n", result);
                return false;
            }

            received_bytes += result;
            remaining_bytes -= result;
        }

        /* the message is likely larger but we only want the HTTP response code */

        printf("received %d bytes:\r\n%.*s\r\n\r\n", received_bytes, strstr(buffer, "\n") - buffer, buffer);

        return true;
    }
    void wifi_scan(){
        WiFiInterface *wifi = _net->wifiInterface();

        WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

        /* scan call returns number of access points found */
        int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

        if (result <= 0) {
            printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
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
        printf("\r\n");
    }
    void print_network_info(){
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }

private:
    NetworkInterface *_net;

#if MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

int main() {
    printf("\r\nStarting socket demo\r\n\r\n");

#ifdef MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
#endif

    SocketDemo *example = new SocketDemo();
    MBED_ASSERT(example);
    example->run();

    return 0;
}
