#ifndef __BLE_BUTTON_SERVICE_H__
#define __BLE_BUTTON_SERVICE_H__

class ButtonService {
public:
    // const static uint16_t BUTTON_SERVICE_UUID              = 0xA000;
    // const static uint16_t BUTTON_STATE_CHARACTERISTIC_UUID = 0xA001;

    ButtonService(BLE &_ble, bool buttonPressedInitial, const uint16_t &BUTTON_SERVICE_UUID, const uint16_t &BUTTON_STATE_CHARACTERISTIC_UUID) :
        ble(_ble), buttonState(BUTTON_STATE_CHARACTERISTIC_UUID, &buttonPressedInitial, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
    {
        GattCharacteristic *charTable[] = {&buttonState};
        GattService         buttonService(BUTTON_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        ble.gattServer().addService(buttonService);
    }

    void updateButtonState(char newchar) {
        ble.gattServer().write(buttonState.getValueHandle(), (uint8_t *)&newchar, sizeof(char));
    }

private:
    BLE                              &ble;
    ReadOnlyGattCharacteristic<bool>  buttonState;
};

#endif /* #ifndef __BLE_BUTTON_SERVICE_H__ */
