#include "accelerate.h"
#include "wifi.h"

NetworkInterface *wifig;
WiFi wifi(
    DESTINATION_IP,
    DESTINATION_PORT
);
Accelerator accelerator;

int main() {
    wifi.connect();
    accelerator.init(128, 128);

    while (true){
        vector<vector<int16_t>>data;
        accelerator.update_current(4);
        accelerator.get_data(data);

        wifi.send_data(data);
        ThisThread::sleep_for(chrono::seconds(1));
    }
}