#include "src/accelerate.h"
#include "src/bluetooth.h"
#include "src/Motion_detect_Source/motion_detect.h"

static EventQueue event_queue(10 * EVENTS_EVENT_SIZE);
Accelerator acc;

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
}

// model
int16_t motiondata_buffer[24];
MOTION_DETECT model(motiondata_buffer);

void detect(Bluetooth *stmservice){
    vector<vector<int16_t>> data;
    acc.update_current(8);
    acc.get_data(data);

    // update motiondata_buffer
    int index = 0;
    for (int i = 0; i < data.size(); i++)
        for (int j = 0; j < data[i].size(); j++){
            motiondata_buffer[i] = data[i][j];
            ++ index;
        }

    const STATE outState[] = {LEFT, RIGHT, STOP, NONE};
    // model prediction
    model.classify();
    const int choise = model.get_top_class(model.output);
    const STATE rd = outState[choise];

    event_queue.call(Callback<void(STATE)>(stmservice->get_ai_service(), &AIService::updateState), rd);
}

int main(){
    acc.init(128, 128);
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(schedule_ble_events);
    Bluetooth stm32service(ble, event_queue);
    stm32service.start(detect);
}