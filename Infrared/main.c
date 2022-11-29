#include "barcode.h"


int main(void)
{
    MAP_WDT_A_holdTimer();

    ADC_State         = OFF;
    ADC_index         = 0;
    sum_of_x_square   = 0;
    sum_of_x          = 0;
    leftWheelSpeed    = 0.0;
    rightWheelSpeed   = 0.0;
    distanceTravelled = 0.0;

    setupS1();            // utility function to be removed (not part of infrared sub-module)
    setupS2();            // utility function to be removed (not part of infrared sub-module)
    setupPWM();           // utility function to be removed (not part of infrared sub-module)

    // Infrared
    setupWheelEncoders(); // port 6 interrupt input
    setupADC();

    setupUART();          // utility function to be removed (not part of infrared sub-module)

    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

    uPrintf("Ready \n\r");

    while(1)
    {
        MAP_PCM_gotoLPM0();
    }
}
