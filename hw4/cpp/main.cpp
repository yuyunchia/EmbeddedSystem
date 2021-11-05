#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <events/mbed_events.h>
#include <mbed.h>
#include <stdint.h>
#include "AdvertisingDataBuilder.h"
#include "AdvertisingParameters.h"
#include "BLETypes.h"
#include "Duration.h"
#include "EventQueue.h"
#include "GapTypes.h"
#include "InterfaceDigitalOut.h"
#include "InterruptIn.h"
#include "ThisThread.h"
#include "Types.h"
#include "UUID.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ButtonService.h"
#include "LEDService.h"
#include "blecommon.h"
#include "pretty_printer.h"

const static char DEVICE_NAME[] = "YutobyButton";
static EventQueue event_queue(10 * EVENTS_EVENT_SIZE);

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

class STMDemo : ble::Gap::EventHandler {
public:
    const uint16_t STMDemo_SERVICE_UUID = 0xA000;
    const uint16_t STMDemo_BUTTON_CHARACTERISTIC_UUID = 0xA001;
    const uint16_t STMDemo_LED_CHRACTERISTIC_UUID = 0xA002;

    STMDemo(BLE &ble, events::EventQueue &event_queue):
        _ble(ble),
        _event_queue(event_queue),
        _alive_led(LED1, 1),
        _actuated_led(LED2, 0),
        _button(BLE_BUTTON_PIN_NAME, BLE_BUTTON_PIN_PULL),
        _button_service(NULL),
        _led_service(NULL),
        _stm32_uuid(STMDemo_SERVICE_UUID),
        _adv_data_builder(_adv_buffer),
        current_index(0),
        mes("B08901039 B08901074 B08901041\r\n\0"){}
    ~STMDemo() {
        delete _button_service;
        delete _led_service;
    }

    void start(){
        printf("STM started\r\n");
        _ble.gap().setEventHandler(this);
        _ble.init(this, &STMDemo::on_init_complete);
        _event_queue.call_every(500, this, &STMDemo::checkAlive);
        _event_queue.dispatch_forever();
    }
private:
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params){
        if (params->error != BLE_ERROR_NONE){
            printf("BLE initialization failed.\r\n");
            return;
        }
        print_mac_address();

        _button_service = new ButtonService(_ble, false, STMDemo_SERVICE_UUID, STMDemo_BUTTON_CHARACTERISTIC_UUID);
        _led_service = new LEDService(_ble, false, STMDemo_SERVICE_UUID, STMDemo_LED_CHRACTERISTIC_UUID);
        
        _button.fall(Callback<void ()>(this, &STMDemo::button_pressed));
        _button.rise(Callback<void ()>(this, &STMDemo::button_released));
        _ble.gattServer().onDataWritten(this, &STMDemo::on_data_written);

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
            print_error(error, "_ble.gap().setAdvertisingParameters failed\r\n");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );
        if (error) {
            print_error(error, "_ble.gap().setAdvertisingPayload() failed\r\n");
            return;
        }

        // Start advertising
        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
        if (error){
            print_error(error, "_ble.gap().startAdvertising() failed\r\n");
            return;
        }
        printf("Advertisement success\r\n");
    }
    void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&) {
        _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    }
    void checkAlive(){
        _alive_led = !_alive_led;
    }
private:  // for button service
    void button_pressed(void){}
    void button_released(void){
        if (mes[current_index] != '\0'){
            _event_queue.call(Callback<void(char)>(_button_service, &ButtonService::updateButtonState), mes[current_index]);
            current_index ++;
        }
    }
private:  // for led service
    void on_data_written(const GattWriteCallbackParams *params){
        if ((params->handle == _led_service->getValueHandle()) && (params->len == 1)){
            _actuated_led = *(params->data);
        }
    }
private:
    const char mes[1024];
    int current_index;

    BLE &_ble;
    events::EventQueue &_event_queue;
    
    DigitalOut _alive_led;
    DigitalOut _actuated_led;
    InterruptIn _button;

    ButtonService *_button_service;
    LEDService *_led_service;

    UUID _stm32_uuid;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};

int main(){
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);
    
    STMDemo stm32service(ble, event_queue);
    stm32service.start();
}
