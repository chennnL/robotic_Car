
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>



void uPrintf(char * TxArray);

/* DEFINITIONS */
#define MIN_DISTANCE        10.0f
#define DISTANCE_FORMULA    58.0f
#define TICKPERIOD          1000

/* GLOBAL VARIABLES */
uint32_t interruptCount;
uint32_t pulseDuration;
float calculateDistance;
float filteredDistance;

/* FUNCTION DECLARTIONS */
static void Delay(uint32_t);
void initHCSR04();
static uint32_t getPulseTime();
void launchPulse();
double kalmanFilter(double);



static void Delay(uint32_t loop)
{
    volatile uint32_t i;

    for (i = 0 ; i < loop ; i++);
}



//![Simple UART Config]
/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source (3MHz)
        1,                                             // BRDIV = 1
        10,                                              // UCxBRF = 10
        0,                                              // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                        // NO Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};

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
    __delay_cycles(30);
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

           // uPrintf("Unfiltered distance is: " );
            //uPrintf(unfiltered);
            //uPrintf("E");

            //EUSCI_A2->TXBUF = (char**)&filtered;
            //uPrintf("Filtered distance is: ");
            uPrintf("696");
            uPrintf(filtered);


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

    /* Halting WDT  */
        WDT_A_holdTimer();

        /* Selecting P1.2 and P1.3 in UART mode */
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
        /* Configuring UART Module */
        UART_initModule(EUSCI_A2_BASE, &uartConfig);

        /* Enable UART module */
        UART_enableModule(EUSCI_A2_BASE);

        /* Enabling interrupts (Rx) */
        UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        Interrupt_enableInterrupt(INT_EUSCIA2);
        //Interrupt_enableMaster();


       // uPrintf("Uart from MSP432/n/r");


    while(1) {
        //PCM_gotoLPM3InterruptSafe();
        launchPulse();
     }
}



//![Simple UART Config]


void uPrintf(char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A2_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                            // Increment pointer to point to the next character
    }
}

/* EUSCI A2 UART ISR */
void EUSCIA2_IRQHandler(void)
{
    unsigned char a = 0;

    a = UART_receiveData(EUSCI_A2_BASE);
    UART_transmitData(EUSCI_A2_BASE, a);               //Transmit back to terminal/putty in uppercase
}
