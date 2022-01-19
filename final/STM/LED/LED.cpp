#include "LED.h"

void Right(DigitalInOut& D_2, DigitalInOut& D_3){
    D_2.output();
    D_3.input();
    D_2 = 1;
}

void Left(DigitalInOut& D_2, DigitalInOut& D_3){
    D_2.input();
    D_3.output();
    D_3 = 1;
}

void Stop(DigitalInOut& D_2, DigitalInOut& D_3){
    D_2.output();
    D_3.output();
    D_2 = 1;
    D_3 = 1;
}

void Nothing(DigitalInOut& D_2, DigitalInOut& D_3){
    D_2.input();
    D_3.input();
}