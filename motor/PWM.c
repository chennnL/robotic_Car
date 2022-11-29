#include "PWM.h"
#include <stdio.h>

/* Application Defines  */
#define TIMER_PERIOD 46875
#define one_tick 1.1 // for each tick the wheel moves by 1.1cm, one full rotation (20 ticks) is 22.0cm (circumference)

static volatile double totalDistanceTravelled = 0; // total distance traveled by car
static volatile float speed = 0;                   // left wheel speed minus right wheel speed
static volatile float deltaspeed = 0;              // left wheel speed minus right wheel speed

uint8_t TARGET = 20; // target notch count

// char used in motor function calls
char car_move;

// Initialised PID Values
static const float KP = 3;
static const float KI = 0.5;
static const float KD = 0.7;

uint16_t left_duty_cycle = 5000;
uint16_t right_duty_cycle = 5000;

#define DIV 1
/**
 * Get the bigger number between two numbers.
 */
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
/**
 * Get the smaller number between two numbers.
 */
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

volatile int sum_error = 0;
volatile short int prev_error = 0;
// gloable variable
volatile static int16_t go_left_counter = 0;     // to count notches on left wheel
volatile static int16_t go_right_counter = 0;    // to count notches on right wheel
volatile static int16_t go_prev_error_left = 0;  // to store previous left error
volatile static int16_t go_prev_error_right = 0; // to store previous right error
volatile static int16_t go_sum_error_left = 0;
volatile static int16_t go_sum_error_right = 0;

volatile static float go_pid_left = 0;
volatile static float go_pid_right = 0;

uint16_t DEFAULT_DC = 6000; // 60% Duty Cycle

/* Statics */
uint32_t notchesdetected_left, rotation, notchesdetected_right, old_notchesdetected_left, old_notchesdetected_right;

#define Input_Encoder_Right GPIO_PORT_P2, GPIO_PIN7
#define Input_Encoder_Left GPIO_PORT_P2, GPIO_PIN6

#define LED1 GPIO_PORT_P1, GPIO_PIN0         // LED1 RED
#define LEDRGB_RED GPIO_PORT_P2, GPIO_PIN0   // RGB LED RED
#define LEDRGB_GREEN GPIO_PORT_P2, GPIO_PIN1 // RGB LED GREEN
#define LEDRGB_BLUE GPIO_PORT_P2, GPIO_PIN2  // RGB LED BLUE

#define Left_Wheel_Pin0 GPIO_PORT_P4, GPIO_PIN0 // from top down view
#define Left_Wheel_Pin2 GPIO_PORT_P4, GPIO_PIN2
#define Right_Wheel_Pin4 GPIO_PORT_P4, GPIO_PIN4
#define Right_Wheel_Pin5 GPIO_PORT_P4, GPIO_PIN5

#define Output_PIN_Left GPIO_PORT_P2, GPIO_PIN4
#define Output_PIN_Right GPIO_PORT_P2, GPIO_PIN5

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig rightPWMConfig =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,         // SMCLK Clock Sorce = 3mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_3,     // 3mhz/1
        10000,                             // 3mhz/1                  // Tick Period
        TIMER_A_CAPTURECOMPARE_REGISTER_1, // CCR2
        TIMER_A_OUTPUTMODE_RESET_SET,      // Compare Output Mode
        1                                  // Duty Cycle
};

/* Timer_A PWM Configuration Parameter */
Timer_A_PWMConfig leftPWMConfig =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,         // SMCLK Clock Sorce = 3mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_3,     // 3mhz/1
        10000,                             // 3mhz/1                  // Tick Period
        TIMER_A_CAPTURECOMPARE_REGISTER_2, // CCR2
        TIMER_A_OUTPUTMODE_RESET_SET,      // Compare Output Mode
        1                                  // Duty Cycle
};

// 1 second timer
const Timer_A_UpModeConfig upConfig =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source = 3Mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_64,     // TACLK = 3MHz / 64
        TIMER_PERIOD,                       // 46875 ticks (CCR0)
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                    // Clear value
};

const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                // SMCLK Clock Source = 3MHZ
        1,                                             // BRDIV = 1
        10,                                            // UCxBRF = 10
        0,                                             // UCxBRS = 0
        EUSCI_A_UART_ODD_PARITY,                       // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                        // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                     // One stop bit
        EUSCI_A_UART_MODE,                             // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION, // Oversampling
};
//![Simple UART Config]

void uPrintf(unsigned char *TxArray)
{
    unsigned short i = 0;
    while (*(TxArray + i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray + i)); // Write the character at the location specified by pointer
        i++;                                              // Increment pointer to point to the next character
    }
}

void Motor_Init()
{

    /*Left Wheel*/
    GPIO_setAsOutputPin(Left_Wheel_Pin0);
    GPIO_setAsOutputPin(Left_Wheel_Pin2);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin0); // low = stop
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2); // low = stop
    /*Right Wheel*/
    GPIO_setAsOutputPin(Right_Wheel_Pin4);
    GPIO_setAsOutputPin(Right_Wheel_Pin5);
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);

    /*PWM port */
    GPIO_setAsPeripheralModuleFunctionOutputPin(Output_PIN_Right, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(Output_PIN_Left, GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsOutputPin(LEDRGB_RED);   /* Configuring P2.0 as output */
    GPIO_setAsOutputPin(LEDRGB_GREEN); /* Configuring P2.0 as output */
    GPIO_setAsOutputPin(LED1);         /* Configuring P2.0 as output */

    GPIO_setAsInputPinWithPullUpResistor(Input_Encoder_Right);
    GPIO_clearInterruptFlag(Input_Encoder_Right); // Clear selected pins interrupt
    GPIO_enableInterrupt(Input_Encoder_Right);    // Allows selected pins to cause interrupts

    GPIO_setAsInputPinWithPullUpResistor(Input_Encoder_Left);
    GPIO_clearInterruptFlag(Input_Encoder_Left); // Clear selected pins interrupt
    GPIO_enableInterrupt(Input_Encoder_Left);    // Allows selected pins to cause interrupts

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts (Rx) */
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);

    /* Configuring Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    Interrupt_enableInterrupt(INT_TA1_0);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    uPrintf("Going into LPM3\n\r");
}

/*First PID*/

void PID()
{
    int error = go_left_counter - go_right_counter; // Error between notches on left and right wheel

    go_left_counter = 0; // left counter wheel
    go_right_counter = 0;
    printf("\nerror : %d", error);

    sum_error += error;
    printf(" sum_error : %d", sum_error);

    // PID Formula
    int adjust = (KP * error) + (KI * sum_error) + (KD * (error - prev_error));
    prev_error = error;
    // printf(" used PWM: %d ",adjust);
    int l_used_PWM = leftPWMConfig.dutyCycle + adjust / 2;
    int r_used_PWM = rightPWMConfig.dutyCycle - adjust / 2;

    printf(" used PWM: %d ", l_used_PWM);
    printf("adjust value = %d", adjust);
    leftPWMConfig.dutyCycle = MAX(MIN(15000, l_used_PWM), 0);
    rightPWMConfig.dutyCycle = MAX(MIN(15000, r_used_PWM), 0);

    Timer_A_generatePWM(TIMER_A0_BASE, &leftPWMConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &rightPWMConfig);
}

/*Second PID*/
/* 1 PID for Left and Right wheel each*/

void PID_Target()
{

    // left and right wheel error
    int16_t error_left = TARGET - go_left_counter; // target notch count = 20
    int16_t error_right = TARGET - go_right_counter;

    // current - previous error
    int16_t derivative_left = error_left - go_prev_error_left;
    int16_t derivative_right = error_right - go_prev_error_right;

    // PID Formula
    go_pid_left = (error_left * KP) + (derivative_left * KD) + (go_sum_error_left * KI);
    go_pid_right = (error_right * KP) + (derivative_right * KD) + (go_sum_error_right * KI);

    // correct thee duty cycle by - or + the pid
    left_duty_cycle += go_pid_left;
    right_duty_cycle += go_pid_right;

    // if the duty cycle hit the max cycle, will use the max value not over shot
    left_duty_cycle = MAX(MIN(left_duty_cycle, 10000), 0);
    right_duty_cycle = MAX(MIN(right_duty_cycle, 10000), 0);

    // New PWM values to be set
    startPWM(left_duty_cycle, right_duty_cycle);

    print_pid_output(error_left, error_right, left_duty_cycle, right_duty_cycle);

    // clear counter
    go_left_counter = 0;
    go_right_counter = 0;
    // take down previous error for both wheel
    go_prev_error_left = error_left;
    go_prev_error_right = error_right;
    // sum error for both wheel
    go_sum_error_left += error_left;
    go_sum_error_right += error_right;
}

// Print function
void print_pid_output(int16_t error_left, int16_t error_right, uint16_t left_duty_cycle, uint16_t right_duty_cycle)
{
    printf("Left %d\t\t \tRight %d\n", go_left_counter, go_right_counter);
    printf("Error Left %d\t\t Error Right %d\n", error_left, error_right);
    printf("PrevError Left %d\t PrevError Right %d\n", go_prev_error_left, go_prev_error_right);
    printf("SumError Left %d\t SumError Right %d\n", go_sum_error_left, go_sum_error_right);
    printf("PID LEFT %.4f\tPID RIGHT %.4f\n", go_pid_left, go_pid_right);
    printf("LEFT DC %d\t\t Right DC %d\n", left_duty_cycle, right_duty_cycle);
    printf("=================================\n");
}

// Reset all global variables to 0
void clear_go_counter()
{
    go_left_counter = 0;
    go_right_counter = 0;
    go_prev_error_left = 0;
    go_prev_error_right = 0;
    go_sum_error_left = 0;
    go_sum_error_right = 0;
    go_pid_left = 0;
    go_pid_right = 0;
}

void TA1_0_IRQHandler(void)
{
    Encoder_speed();
    /*PID();*/
    if (DEFAULT_DC == 0)
    {
        Motor_Stop();
    }
    else if (car_move == 'W' || car_move == 'S')
    {
        PID_Target();
    }

    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

// PWM set to 9000
void Motor_Drive_Forward_Fast()
{

    TARGET = 20; // target notch count

    left_duty_cycle = 9000;
    right_duty_cycle = 9000;
    clear_go_counter();
    car_move = 'W';
    // 90%
    // Set right motor to forward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin5);

    // Set left motor to move forward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);
}

// PWM set to 7000
void Motor_Drive_Forward_Slow()
{
    clear_go_counter();
    TARGET = 18; // target notch count
    left_duty_cycle = 7000;
    right_duty_cycle = 7000;
    car_move = 'W';
    // 50%
    // Set right motor to forward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin5);

    // Set left motor to move forward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);

    startPWM(left_duty_cycle, right_duty_cycle);
}

// Car wheels move backward, reverse
void Motor_Reverse()
{

    clear_go_counter();
    car_move = 'S';
    // Set right motor to back
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin4);

    // Set left motor to move back
    GPIO_setOutputHighOnPin(Left_Wheel_Pin2);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin0);
}

// Stop both motors
void Motor_Stop()
{

    clear_go_counter();
    // Set right motor to forward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);

    // Set left motor to move forward
    GPIO_setOutputLowOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);
}

// Stop the car, right motor
void right_Motor_Stop()
{

    clear_go_counter();
    // Set right motor to forward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);
}

// Stop the car, left motor
void left_Motor_Stop()
{

    clear_go_counter();
    // Set left motor to move forward
    GPIO_setOutputLowOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);
}

// Left turn by 90 degrees
void Motor_Turn_Left()
{

    car_move = 'A';
    clear_go_counter();
    // Set right motor to forward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin5);
    // Set left motor to move backward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin2);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin0);

    startPWM(7000, 7000);
}

// Right turn by 90 degrees
void Motor_Turn_Right()
{

    car_move = 'D';
    clear_go_counter();
    // Set right motor to backward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin4);

    // Set left motor to move forward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);

    startPWM(7000, 7000);
}

// Small left turn by 45 degrees
void Motor_Turn_Left_45()
{

    car_move = 'K';
    clear_go_counter();
    // Set right motor to forward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin4);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin5);
    // Set left motor to move backward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin2);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin0);

    startPWM(7000, 7000);
}

// Small right turn by 45 degrees
void Motor_Turn_Right_45()
{

    car_move = 'J';
    clear_go_counter();
    // Set right motor to backward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin4);

    // Set left motor to move forward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);

    startPWM(7000, 7000);
}

// Turns car by 180 degrees
void Motor_uTurn()
{

    car_move = 'H';
    clear_go_counter();
    // Set right motor to backward
    GPIO_setOutputLowOnPin(Right_Wheel_Pin5);
    GPIO_setOutputHighOnPin(Right_Wheel_Pin4);

    // Set left motor to move forward
    GPIO_setOutputHighOnPin(Left_Wheel_Pin0);
    GPIO_setOutputLowOnPin(Left_Wheel_Pin2);

    startPWM(7000, 7000);
}

// run wheel
void startPWM(uint_fast16_t left_dutyCycle, uint_fast16_t right_dutyCycle)
{
    leftPWMConfig.dutyCycle = left_dutyCycle;
    rightPWMConfig.dutyCycle = right_dutyCycle;
    Timer_A_generatePWM(TIMER_A0_BASE, &rightPWMConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &leftPWMConfig);
}

// Encoder function to get speed of the wheel
void Encoder_speed()
{
    // speed = notches * 1.1cm

    float leftspeed = go_left_counter * one_tick; // 1 tick = 1.1, one full rotation (20 ticks) is 22.0cm (circumference)
    float rightspeed = go_right_counter * one_tick;

    deltaspeed = (leftspeed + rightspeed) / 2;
    printf("\n(1 second has passed.) Left wheel speed is %.2f cm/s;  Right wheel speed is %.2f cm/s. Delta speed is %.2fcm/s\n", leftspeed, rightspeed, deltaspeed);
    if (leftspeed > rightspeed)
    {

        GPIO_toggleOutputOnPin(LEDRGB_RED);   // RED RGB LED ON
        GPIO_setOutputLowOnPin(LEDRGB_GREEN); // Green RGB LED OFF
    }
    else if (rightspeed > leftspeed)
    {
        GPIO_toggleOutputOnPin(LEDRGB_GREEN); // RED RGB LED ON
        GPIO_setOutputLowOnPin(LEDRGB_RED);   // Green RGB LED OFF
    }
    else // if speeds are equal
    {
        GPIO_setOutputLowOnPin(LEDRGB_GREEN); // RED RGB LED ON
        GPIO_setOutputLowOnPin(LEDRGB_RED);   // Green RGB LED OFF
    }
    GPIO_toggleOutputOnPin(LED1); // RED RGB LED ON
}

/* EUSCI A0 UART ISR */
void EUSCIA0_IRQHandler(void)
{
    unsigned char a = 0;

    a = UART_receiveData(EUSCI_A0_BASE);

    // if letter a-m is pressed, turn on RED LED
    // return upper case letter
    if (a >= 'a' && a <= 'm')
    {
        GPIO_setOutputHighOnPin(LED1);
        UART_transmitData(EUSCI_A0_BASE, toupper(a));
    }

    // if letter z is pressed, turn off RED LED
    // return upper case letter
    else if (a == 'z')
    {
        GPIO_setOutputLowOnPin(LED1);
        UART_transmitData(EUSCI_A0_BASE, toupper(a));
    }
}

/* GPIO ISR */
void PORT2_IRQHandler(void)
{
    uint32_t status;
    status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P2); // get status

    if (status & GPIO_PIN6)
    {
        go_left_counter++;                           // increment left wheel counter
        GPIO_clearInterruptFlag(Input_Encoder_Left); // clear interrupt flag for port 2
    }

    if (status & GPIO_PIN7)
    {
        go_right_counter++;                           // increment right wheel counter
        GPIO_clearInterruptFlag(Input_Encoder_Right); // clear interrupt flag for port 2
    }
    // move 90 degree to left
    if (go_left_counter == 8 && car_move == 'A') // 8 notches to turn 90 degrees
    {
        Motor_Stop();
        printf("entered left");
    }
    // move 90 degree to right
    if (go_left_counter == 8 && car_move == 'D')
    {
        Motor_Stop();
        printf("entered right");
    }

    // move 180 degree to right
    if (go_left_counter == 16 && car_move == 'H') // 16 notches to turn 180 degrees
    {
        Motor_Stop();
        printf("entered uturn right");
    }
    // move 180 degree to left
    if (go_left_counter == 16 && car_move == 'I')
    {
        Motor_Stop();
        printf("entered utrun left");
    }
    // move 45 degree to right
    if (go_left_counter == 4 && car_move == 'J') // 4 notches to turn 45 degrees
    {
        Motor_Stop();
        printf("entered 45 degree right");
    }
    // move 45 degree to left
    if (go_left_counter == 4 && car_move == 'K')
    {
        Motor_Stop();
        printf("entered 45 degree left");
    }
}
