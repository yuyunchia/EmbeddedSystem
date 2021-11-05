#include <cstddef>
#include <cstdio>
#include <events/mbed_events.h>
#include <mbed.h>
#include "EventQueue.h"
#include "ThisThread.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ButtonService.h"
#include "LEDService.h"
#include "pretty_printer.h"

const static char DEVICE_NAME[] = "YutobyButton";
static EventQueue event_queue(10 * EVENTS_EVENT_SIZE);

class STM : ble::Gap::EventHandler {
public:
    STM(BLE &ble, events::EventQueue &event_queue):
        _ble(ble),
        _event_queue(event_queue),
        _led1(LED1, 1),
        _button(BLE_BUTTON_PIN_NAME, BLE_BUTTON_PIN_PULL),
        _service(NULL),
        _button_uuid(ButtonService::BUTTON_SERVICE_UUID),
        _led_uuid(LEDService::LED_SERVICE_UUID),
        _adv_data_builder(_adv_buffer){}

private:
    BLE &_ble;
    events::EventQueue &_event_queue;
    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
}