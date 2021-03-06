/*
 * Copyright (c) 2014-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "PinNames.h"
#include "ThisThread.h"
#include "arm_math.h"
#include "mbed.h"
#include <cstdint>
#include <cstdio>

// Adjust pin name to your board specification.
// You can use LED1/LED2/LED3/LED4 if any is connected to PWM capable pin,
// or use any PWM capable pin, and see generated signal on logical analyzer.
PwmOut led(PWM_OUT);

int main()
{
    int32_t signal = 0;    //LED brightness
    float32_t duty_cycle = 0.5f;
    while(1){
    // specify period first
    led.period(0.02f);      // 20 ms period
    led.write(duty_cycle);      
        while(duty_cycle < 1.0f){
            led.write(duty_cycle);      
            duty_cycle = duty_cycle + 0.05f ;   // increase duty cycle
            ThisThread::sleep_for(20);  // delay a period time
        }
        while(duty_cycle > 0){
            led.write(duty_cycle);      
            duty_cycle = duty_cycle - 0.05f ; // decrease duty cycle
            ThisThread::sleep_for(20);  // delay a period time
        }
    //led = 0.5f;          // shorthand for led.write()
    //led.pulsewidth(2);   // alternative to led.write, set duty cycle time in seconds
    }
}
