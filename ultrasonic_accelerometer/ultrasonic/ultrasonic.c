/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*******************************************************************************
 * MSP432 Timer Interrupt
 *
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P3.6  |---> Trigger
 *            |                  |
 *            |            P3.7  |<--- Echo
 *            |                  |
 *            |                  |
 *
 ******************************************************************************/
/*
 Team Members:  Lim Wei, Jourdan            [2102516]
                Nasruddine Louahemmsabah    [2100835]
                Perpetua Sorubha Raj        [2101771]
*/

/* HEADER */
#include "ultrasonic.h"

// -------------------------------------------------------------------------------------------------------------------

static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0 ; i < loop ; i++);
}

// -------------------------------------------------------------------------------------------------------------------

/* INITIALISING THE ULTRASONIC SENSOR*/
void initHCSR04(void) {
    /* Timer_A UpMode Configuration Parameters */
    const Timer_A_UpModeConfig upConfig =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source = 3MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_3,          // 3MHz / 3 = 1MHz
        TICKPERIOD,                             // 1 / 1MHz * 1000 = 1ms
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer Interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 Interrupt
        TIMER_A_DO_CLEAR                        // Clear Value
    };

    // Configuring P1.0 as output
    // P1.0 - LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    // Configuring P3.6 as output
    // P3.6 - Trigger
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);

    // Configuring P3.7 as input
    // P3.7 - Echo
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P3, GPIO_PIN7);

    // Configure interrupt at P3.7
    // Low-to-High (Positive-Edge)
    GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,
                             GPIO_LOW_TO_HIGH_TRANSITION);
    // Clear the interrupt flag
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN7);
    // Enable the interrupt on P3.7
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN7);
    // Enable the interrupt at P3
    Interrupt_enableInterrupt(INT_PORT3);

    // Configuring Timer_A0 for Up Mode
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    // Enable the interrupt for Timer_A0
    Interrupt_enableInterrupt(INT_TA0_0);
    // Clear the timer
    Timer_A_clearTimer(TIMER_A0_BASE);

}

// -------------------------------------------------------------------------------------------------------------------

/* GET THE PULSE DURATION */
static uint32_t getPulseTime(void) {
    uint32_t pulseTime = 0;

    // Number of times the interrupt occurred
    // 1 interrupt = 1000 ticks
    pulseTime = interruptCount * TICKPERIOD;

    // Excess ticks needs to be counted
    pulseTime += Timer_A_getCounterValue(TIMER_A0_BASE);

    // Clear the timer
    Timer_A_clearTimer(TIMER_A0_BASE);

    Delay(3000);

    return pulseTime;
}

// -------------------------------------------------------------------------------------------------------------------

/* LAUNCHES A PULSE FOR OBJECT DETECTION */
void launchPulse(void) {
    Delay(3000);
    // Output a pulse at P3.6
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
    // 10us delay
    Delay(30);
    // Receive the pulse at P3.7
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN6);
}

// -------------------------------------------------------------------------------------------------------------------

/* KALMAN FILTER */
double kalmanFilter(double U) {
    // The higher R is, the more noise it will reduce
    // (but will also take longer)
    static const double R = 40;

    // Measurement map scalar
    static const double H = 1.00;

    // Initial estimated covariance
    static double Q = 10;
    // Initial error covariance (starts at 0)
    static double P = 0;

    // Filtered estimated
    static double U_hat = 0;

    // Kalman gain
    static double K = 0;


    // Update Kalman gain
    K = P * H / (H * P * H + R);
    // Update the estimate (U_hat)
    U_hat = U_hat + K * (U - H * U_hat);
    // Update error covariances
    P = (1 - K * H) * P + Q;

    return U_hat;
}

// -------------------------------------------------------------------------------------------------------------------

/* INTERRUPTS AFTER THE TIMER HITS A CERTAIN VALUE */
void TA0_0_IRQHandler(void) {
    // Increases by one after an interrupt
    // (counts the number of interrupts occurred)
    interruptCount++;

    // Clear the interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

// -------------------------------------------------------------------------------------------------------------------

/* REPLACES WHILE LOOP, INTERRUPT ONCE PULSE IS RECEIVED */
void PORT3_IRQHandler(void) {
    // Starts true
    static bool flag = true;

    // Get the status of the interrupted GPIO
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);

    // Low-to-High
    if (flag) {
        // Reset all variables
        pulseDuration = 0;
        interruptCount = 0;
        calculateDistance = 0;
        filteredDistance = 0;

        // Clear the timer
        Timer_A_clearTimer(TIMER_A0_BASE);
        // Start the timer
        Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

        // High-to-Low (Negative Edge)
        GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,
                                 GPIO_HIGH_TO_LOW_TRANSITION);
    }
    // High-to-Low
    else {
        // Char array to store the distance value
        char unfiltered[50];
        char filtered[50];
        // Stop the timer
        Timer_A_stopTimer(TIMER_A0_BASE);

        // Get the duration of the pulse
        pulseDuration = getPulseTime();
        // Calculate the distance using the formula:
        // duration / 58.0f
        calculateDistance = pulseDuration / DISTANCE_FORMULA;
        // Filter the values using a Kalman Filter for a more accurate reading
        filteredDistance = kalmanFilter(calculateDistance);

        // If the distance is < the minimum distance (15.0f),
        if (filteredDistance < MIN_DISTANCE) {
            // Turn on the LED at P1.0
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            sprintf(unfiltered, "%f", calculateDistance);
            sprintf(filtered, "%f", filteredDistance);

            printf("\n%s", unfiltered);
            printf("\n%s", filtered);
        }
        // Else,
        else {
            // Turn off the LED at P1.0
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }

        // Low-to-High (Positive Edge)
        GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,
                                 GPIO_LOW_TO_HIGH_TRANSITION);
    }

    // Change the flag state
    flag = !flag;

    // Clear the interrupt flag of the GPIO
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
}

// -------------------------------------------------------------------------------------------------------------------

/* MAIN FUNCTION */
uint32_t main(void) {
    initHCSR04();

    while(1) {
        launchPulse();
     }
}
