#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "AdvertisingDataBuilder.h"
#include "AdvertisingParameters.h"
#include "BLETypes.h"
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
#include "AIservice.h"

#define DEVICE_NAME "RPi_reciever"
#define SERVICE_UUID 0xA000
#define CHARACTERISTIC_UUID 0xA001

class Bluetooth : ble::Gap::EventHandler {
public:
    Bluetooth(BLE& ble, events::EventQueue& event_queue):
        _ble(ble),
        _event_queue(event_queue),
        _uuid(SERVICE_UUID),
        ai_serivce(NULL),
        _adv_data_builder(adv_buffer)
    {}

    ~Bluetooth(){
        delete ai_serivce;
    }

    void start(){
        printf("Bluetooth started\r\n");
        _ble.gap().setEventHandler(this);
        _ble.init(this, &Bluetooth::init);
        _event_queue.dispatch_forever();
    }
    
private:
    void init(BLE::InitializationCompleteCallbackContext* params){
        if (params->error != BLE_ERROR_NONE){
            printf("BLE initialization failed.\r\n");
            return;
        }
        ai_serivce = new AIService(_ble, NONE, SERVICE_UUID, CHARACTERISTIC_UUID);

        start_advertising();
    }
    void start_advertising(){
        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setLocalServiceList(mbed::make_Span(&_stm32_uuid, 1));
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

    BLE& _ble;
    events::EventQueue& _event_queue;
    UUID _uuid;
    AIService *ai_serivce;
    uint8_t adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

#endif