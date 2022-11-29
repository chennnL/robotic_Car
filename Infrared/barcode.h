#ifndef _BARCODE_
#define _BARCODE_


#include <stdio.h>      // for printf
#include <stdlib.h>
#include "driverlib.h"  // MSP432 DriverLib
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define ON  1
#define OFF 0

#define ARRAY_SIZE      20000
#define SIGNAL_FILTER   1000
#define MAX_UINT8_VAL   255
#define SHORT_INTERVAL  0.04
#define BLACK           0
#define WHITE           1
#define TICK_DISTANCE   1.1    // each tick for wheel encoder 1.1 cm apart

typedef struct _wheelTime
{
    uint16_t  milliseconds;
    uint16_t       seconds;
    uint16_t       minutes;
    uint16_t         hours;
} wheelTime;

void setupS1(void);
void setupS2(void);

void setupADC(void);
void startADC(void);
void stopADC(void);

void setupPWM(void);
void startPWM(uint_fast16_t,uint_fast16_t);
void stopPWM(void);

void setupWheelEncoders(void);
char guessBarcode(double);
char scanBarcode(void);

double getWheelTime( wheelTime* );
void LeftWheelSpeed(void);
void RightWheelSpeed(void);

void setupUART(void);
void uPrintf(unsigned char * TxArray);

/* global variables required by infrared barcode sub-module  */
static volatile uint8_t    ADC_State;
static volatile uint32_t   ADC_index;
static volatile uint32_t   sum_of_x_square;   // this is sum(x^2)
static volatile uint32_t   sum_of_x;          // this is sum(x)
static volatile uint8_t    x[ARRAY_SIZE];     // this is array of x - normalized 8 bit values (convert ADC values to 0-255 values)
static volatile wheelTime  leftwheelTime  = {0,0,0,0};
static volatile wheelTime  rightwheelTime = {0,0,0,0};
static volatile double     leftWheelSpeed;
static volatile double     rightWheelSpeed;
static volatile double     distanceTravelled;
static volatile char decodedAlphabet;

#endif
