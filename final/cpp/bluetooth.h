#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "AdvertisingDataBuilder.h"
#include "AdvertisingParameters.h"
#include "BLETypes.h"
#include "Callback.h"
#include "Duration.h"
#include "EventQueue.h"
#include "GapTypes.h"
#include "Types.h"
#include "UUID.h"
#include "blecommon.h"
#include "mbed.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "AIservice.h"

#define DEVICE_NAME "RPi_reciever"
#define SERVICE_UUID 0xA000
#define CHARACTERISTIC_UUID 0xA001
#define DETECT_PER_MILLI 100       // time interval of every detected data

inline void print_address(const ble::address_t &addr){
    printf("%02x:%02x:%02x:%02x:%02x:%02x\r\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
}

inline void print_mac_address(){
    /* Print out device MAC address to the console*/
    ble::own_address_type_t addr_type;
    ble::address_t address;
    //BLE::Instance().gap().getAddress(addr, address);
    BLE::Instance().gap().getAddress(addr_type, address);
    //BLE::Instance().gap().getAddress(&addr_type, address);
    printf("\nDEVICE MAC ADDRESS: ");
    print_address(address);
}

class Bluetooth : ble::Gap::EventHandler {
public:
    Bluetooth(BLE& ble, events::EventQueue& event_queue):
        _ble(ble),
        _event_queue(event_queue),
        _uuid(SERVICE_UUID),
        ai_serivce(NULL),
        _adv_data_builder(adv_buffer){}

    ~Bluetooth(){
        delete ai_serivce;
    }

    void start(void (*detect)(Bluetooth*)){
        printf("Bluetooth started\r\n");
        _ble.gap().setEventHandler(this);
        _ble.init(this, &Bluetooth::init);
        // _event_queue.call_every(DETECT_PER_MILLI, this, &Bluetooth::detect);
        // pass a function pointer
        _event_queue.call_every(DETECT_PER_MILLI, detect, this);
        _event_queue.dispatch_forever();
    }

    AIService* get_ai_service(){
        return ai_serivce;
    }
    
private:
    void init(BLE::InitializationCompleteCallbackContext* params){
        if (params->error != BLE_ERROR_NONE){
            printf("BLE initialization failed.\r\n");
            return;
        }

        print_mac_address();

        ai_serivce = new AIService(_ble, NONE, SERVICE_UUID, CHARACTERISTIC_UUID);

        start_advertising();
    }
    void start_advertising(){
        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setLocalServiceList(mbed::make_Span(&_uuid, 1));
        _adv_data_builder.setName(DEVICE_NAME);

        // Setup advertising
        ble_error_t error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );
        if (error){
            printf("%d _ble.gap().setAdvertisingParameters failed\r\n", error);
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );
        if (error) {
            printf("%d _ble.gap().setAdvertisingPayload() failed\r\n", error);
            return;
        }

        // Start advertising
        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
        if (error){
            printf("%d _ble.gap().startAdvertising() failed\r\n", error);
            return;
        }
        printf("Advertisement success\r\n");
    }
    void detect_sample(){
        const STATE testState[] = {LEFT, RIGHT, STOP, NONE};
        const STATE rd = testState[rand() % 4];
        // TODO: use model for decision instead of random
        _event_queue.call(Callback<void(STATE)>(ai_serivce, &AIService::updateState), rd);
    }

    BLE& _ble;
    events::EventQueue& _event_queue;
    UUID _uuid;
    AIService *ai_serivce;
    uint8_t adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

#endif