#include "mbed.h"
#include "PinNames.h"
// #include "ThisThread.h"
#include <thread>
#include <iostream>

using namespace std;
DigitalInOut Left(D2);
DigitalInOut Right(D3);
// DigitalInOut Stop(D2);
// DigitalInOut Nothing(D3);

int main()
{
    while (1)
    {
        Left.input();
        Right.output();
        // Stop.output();
        // Nothing.output();
        // Left = !Left;
        Right = 1;
        ThisThread::sleep_for(100);

        // read from pin as input
        Left.output();
        Right.input();
        // Left = 1;
        // printf("Left.read() = %d \n\r", Left.read());
        ThisThread::sleep_for(100);
    }
}