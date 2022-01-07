#ifndef __AI_SERVICE_H__
#define __AI_SERVICE_H__

#include "GattService.h"
#include "ble/BLE.h"
#include "GattCharacteristic.h"
#include <cstdint>
using namespace std;

enum STATE {
    LEFT,
    RIGHT,
    STOP,
    NONE
};

class AIService {
public:
    AIService(BLE& ble, STATE initial_state, const uint16_t &SERVICE_UUID, const uint16_t &CHARACTERISTIC_UUID):
        _ble(ble),
        AIstate(CHARACTERISTIC_UUID, &initial_state, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
    {
        GattCharacteristic *charTable[] = {&AIstate};
        GattService ai_service(SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic*));
        ble.gattServer().addService(ai_service);
    }
    void updateState(const STATE& new_state){
        char c;
        if (new_state == LEFT) c = 'l';
        else if (new_state == RIGHT) c = 'r';
        else if (new_state == STOP) c = 's';
        else c = 'n';
        _ble.gattServer().write(AIstate.getValueHandle(), (uint8_t*)&c, sizeof(char));
    }
private:
    BLE& _ble;
    ReadOnlyGattCharacteristic<STATE> AIstate;
};

#endif