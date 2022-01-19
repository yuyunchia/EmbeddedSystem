#include "mbed.h"
#include "PinNames.h"
#include "LED.h"
// #include "ThisThread.h"
// #include <thread>
#include <chrono>
#include <iostream>

using namespace std;
DigitalInOut D_2(D2);
DigitalInOut D_3(D3);

int main()
{
    while (1)
    {
        Left(D_2, D_3);
        cout << D_2.read() << " " << D_3.read() << endl;
        ThisThread::sleep_for(1000);
        Right(D_2, D_3);
        cout << D_2.read() << " " << D_3.read() << endl;
        ThisThread::sleep_for(1000);
        Stop(D_2, D_3);
        cout << D_2.read() << " " << D_3.read() << endl;
        ThisThread::sleep_for(1000);
        Nothing(D_2, D_3);
        cout << D_2.read() << " " << D_3.read() << endl;
        ThisThread::sleep_for(1000);

    }
}