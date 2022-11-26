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
 * MSP432 Timer_A - Variable PWM
 *
 * Description: In this example, the Timer_A module is used to create a precision
 * PWM with an adjustable duty cycle. The PWM initial period is 5 ms and is
 * output on P5.6. The initial duty cycle of the PWM is 10%, however when the
 * button is pressed on P1.4 the duty cycle is sequentially increased by 10%.
 * Once the duty cycle reaches 90%, the duty cycle is reset to 10% on the
 * following button press.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.4  |<--Toggle Switch
 *            |                  |
 *            |                  |
 *            |            P5��6  |--> Output PWM
 *            |                  |
 *            |                  |
 *
 *******************************************************************************/
//Chen Wujie Student ID: 2101169
//Izz Danial Student ID: 2100735
/* DriverLib Includes */
#include <driverlib.h>
#include <stdio.h>
#include <math.h>
#include <Motor/PWM.h>
#include <stdlib.h>

int i = 0; //global value i use to detect S1 or S2 button been press anot if both press togehter will reset to 0
#define Input_PIN_Left GPIO_PORT_P1, GPIO_PIN1 //define input pin
#define Input_PIN_Right GPIO_PORT_P1, GPIO_PIN4 //define input pin


/* Statics */
uint32_t notchesdetected_left,rotation,notchesdetected_right;

#define Input_Encoder_Right GPIO_PORT_P2,GPIO_PIN7
#define Input_Encoder_Left GPIO_PORT_P2,GPIO_PIN6




int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    Motor_Init();
    setupWheelEncoders();
    setupADC();

    GPIO_setAsInputPinWithPullUpResistor(Input_PIN_Left);
    GPIO_clearInterruptFlag(Input_PIN_Left);
    GPIO_enableInterrupt(Input_PIN_Left);

    GPIO_setAsInputPinWithPullUpResistor(Input_PIN_Right);
    GPIO_clearInterruptFlag(Input_PIN_Right);
    GPIO_enableInterrupt(Input_PIN_Right);

    /* Configuring Timer_A to have a period of approximately 80ms and an initial duty cycle of 10% of that (1000 ticks)  */

    /* Enabling interrupts and starting the watchdog timer */
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableInterrupt(INT_PORT2); // enable interrupt for port 2

    Interrupt_enableSleepOnIsrExit();
    Interrupt_enableMaster();


    /* Sleeping when not in use */
    while (1)
    {
        // PCM_gotoLPM0();
        launchPulse();
    }
}



/* Port1 ISR - This ISR will progressively step up the duty cycle of the PWM on a button press */
void PORT1_IRQHandler(void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);

    if (status & GPIO_PIN4)
    {
        StartADC();
        i = i + 1;
        printf("i value is %d",i);
        if(i == 1){
            Motor_Drive_Forward_Slow();
        }
        else if(i == 2 ){
            Motor_Drive_Forward_Fast();
            printf("i value is %d",i);
        }
        else if (i == 3){
            Motor_Turn_Right();
            printf("i value is %d",i);
        }
        else if (i>3){
            i = 0;
        }
    }
    if (status & GPIO_PIN1){
        Motor_Stop();
        StopADC();
        i = 0;
    }
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);


}



