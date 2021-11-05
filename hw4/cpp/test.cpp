#include <cstddef>
#include <cstdio>
#include <events/mbed_events.h>
#include <mbed.h>
#include "ThisThread.h"
#include "Thread.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ButtonService.h"
#include "LEDService.h"
#include "pretty_printer.h"
#include "Callback.h"

const static char DEVICE_NAME[] = "YutobyButton";
static EventQueue event_queue(/* event count */ 10 * EVENTS_EVENT_SIZE);


class ButtonDemo : ble::Gap::EventHandler {
public:
    ButtonDemo(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue),
        _led1(LED1, 1),
        _button(BLE_BUTTON_PIN_NAME, BLE_BUTTON_PIN_PULL),
        _button_service(NULL),
        _button_uuid(ButtonService::BUTTON_SERVICE_UUID),
        _adv_data_builder(_adv_buffer) { }

    void start() {
        printf("button started\n");
        _ble.gap().setEventHandler(this);
        
        _ble.init(this, &ButtonDemo::on_init_complete);

        _event_queue.call_every(500, this, &ButtonDemo::blink);

        _event_queue.dispatch_forever();

        ThisThread::sleep_for(100ms);
    }

private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
        if (params->error != BLE_ERROR_NONE) {
            printf("Ble initialization failed.");
            return;
        }

        print_mac_address();

        /* Setup primary service. */

        _button_service = new ButtonService(_ble, false /* initial value for button pressed */);

        _button.fall(Callback<void()>(this, &ButtonDemo::button_pressed));
        _button.rise(Callback<void()>(this, &ButtonDemo::button_released));

        start_advertising();
    }

    void start_advertising() {
        /* Create advertising parameters and payload */

        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setLocalServiceList(mbed::make_Span(&_button_uuid, 1));
        _adv_data_builder.setName(DEVICE_NAME);

        /* Setup advertising */

        ble_error_t error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingParameters() failed");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingPayload() failed");
            return;
        }

        /* Start advertising */

        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            print_error(error, "_ble.gap().startAdvertising() failed");
            return;
        }
    }

    void button_pressed(void) {}

    void button_released(void) {
        if (mes[current_index] != '\0'){
            _event_queue.call(Callback<void(char)>(_button_service, &ButtonService::updateButtonState), mes[current_index]);
            current_index ++;
        }
    }

    void blink(void) {
        _led1 = !_led1;
    }

private:
    /* Event handler */

    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&) {
        _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    }

private:
    const char mes[1024] = "B08901039 B08901074 B08901041\r\n\0";
    int current_index = 0;

    BLE &_ble;
    events::EventQueue &_event_queue;

    DigitalOut  _led1;
    InterruptIn _button;
    ButtonService *_button_service;

    UUID _button_uuid;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

class LEDDemo : ble::Gap::EventHandler {
public:
    LEDDemo(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _event_queue(event_queue),
        _alive_led(LED1, 1),
        _actuated_led(LED2, 0),
        _led_uuid(LEDService::LED_SERVICE_UUID),
        _led_service(NULL),
        _adv_data_builder(_adv_buffer) { }
 
    ~LEDDemo() {
        delete _led_service;
    }
    void start() {
        printf("led started\n");

        _ble.gap().setEventHandler(this);
 
        _ble.init(this, &LEDDemo::on_init_complete);
 
        _event_queue.call_every(500, this, &LEDDemo::blink);
 
        _event_queue.dispatch_forever();

        ThisThread::sleep_for(100ms);
    }
 
private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params) {
        if (params->error != BLE_ERROR_NONE) {
            printf("Ble initialization failed.");
            return;
        }
 
        _led_service = new LEDService(_ble, false);
 
        _ble.gattServer().onDataWritten(this, &LEDDemo::on_data_written);
 
        print_mac_address();
 
        start_advertising();
    }
    void start_advertising() {
        /* Create advertising parameters and payload */
        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );
 
        _adv_data_builder.setFlags();
        _adv_data_builder.setLocalServiceList(mbed::make_Span(&_led_uuid, 1));
        _adv_data_builder.setName(DEVICE_NAME);
 
        /* Setup advertising */
 
        ble_error_t error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );
 
        if (error) {
            printf("_ble.gap().setAdvertisingParameters() failed\r\n");
            return;
        }
 
        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );
 
        if (error) {
            printf("_ble.gap().setAdvertisingPayload() failed\r\n");
            return;
        }
 
        /* Start advertising */
 
        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
 
        if (error) {
            printf("_ble.gap().startAdvertising() failed\r\n");
            return;
        }
    }
    void on_data_written(const GattWriteCallbackParams *params) {
        if ((params->handle == _led_service->getValueHandle()) && (params->len == 1)) {
            _actuated_led = *(params->data);
        }
    }
    void blink() {
        _alive_led = !_alive_led;
    }
private:
    /* Event handler */
    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&) {
        _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    }
private:
    BLE &_ble;
    events::EventQueue &_event_queue;
    DigitalOut _alive_led;
    DigitalOut _actuated_led;
 
    UUID _led_uuid;
    LEDService *_led_service;
 
    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

/** Schedule processing of events from the BLE middleware in the event queue. */
void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

void testfunc(){

}

int main()
{
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);

    ButtonDemo buttondemo(ble, event_queue);
    LEDDemo leddemo(ble, event_queue);
    // leddemo.start();
    Thread t2, t3;
    t3.start(callback(&leddemo, &LEDDemo::start));
    t2.start(callback(&buttondemo, &ButtonDemo::start));
    ThisThread::sleep_for(100ms);

    return 0;
}
