#include "ThisThread.h"
#include "mbed.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <vector>

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
    void get_data(vector<float*> buffer, const bool &refresh=true){
        if (refresh)
            buffer.clear();
        float data[6];
        for (int i = 0; i < sequense_length; i++){
            data[0] = acce_x[i];
            data[1] = acce_y[i];
            data[2] = acce_z[i];
            data[3] = gyro_x[i];
            data[4] = gyro_y[i];
            data[5] = gyro_z[i];
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
            printf("%d ", int16_t(acce_x[i]));
        printf("\n");
        printf("Seq gyro_y: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(acce_y[i]));
        printf("\n");
        printf("Seq gyro_z: ");
        for (int i = 0; i < sequense_length; i++)
            printf("%d ", int16_t(acce_z[i]));
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

// main() runs in its own thread in the OS
int main(){
    Accelerator accelerator;
    accelerator.init(128, 128);

    while (true) {
        accelerator.print_offset();
        accelerator.print_data();
        accelerator.update_current(4);
        ThisThread::sleep_for(chrono::seconds(5));
    }
}

