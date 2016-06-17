#include "mbed.h"

//#define CTN10K
#ifdef CTN10K

AnalogIn CTN(A0);

int main()
{
    float tension,temperature;
    while(1) {
        tension=CTN.read()*3.3;
        temperature=(29.85*tension)-19.7;
        printf("TENSION => %.2f\t",tension);
        printf("TEMP => %.2f\n\r",temperature);
        wait(0.5);
    }
}
#endif //CTN10K