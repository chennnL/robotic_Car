## Ultrasonic

#### Port & Pins Used:
(1) P3.6    (Trigger)   
(2) P3.7    (Echo)
(3) P1.0    (LED)

#### Includes:
(1) #include "driverlib.h"
(2) #include <stdint.h>
(3) #include <stdio.h>
(4) #include <stdbool.h>

#### Defines:
(1) #define MIN_DISTANCE        16.0f
(2) #define DISTANCE_FORMULA    58.0f
(3) #define TICKPERIOD          1000

#### Global Variables:
(1) static volatile uint32_t interruptCount;
(2) static volatile uint32_t pulseDuration;
(3) static volatile float calculateDistance;
(4) static volatile float filteredDistance;

#### Ultrasonic Submodule Functions:
(1) static void Delay(uint32_t);
(2) void initHCSR04();
(3) static uint32_t getPulseTime();
(4) void launchPulse();
(5) double kalmanFilter(double);

#### Interrupt Used:
(1) void TA_0_IRQHandler(void);     // TIMER_A0_BASE
(2) void PORT3_IRQHandler(void);    // PORT3_INTERRUPT