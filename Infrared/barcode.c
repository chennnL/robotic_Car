// Team member for infrared module
// Lin WeiChen   (SIT 2101642)
// Kee Boon Hwee (SIT 2101797)
// Saif Ali Khan (SIT 2001559)

#include "barcode.h"

const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        1,                                              // BRDIV = 1
        10,                                             // UCxBRF = 10
        0,                                              // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                         // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};

Timer_A_PWMConfig rightDCMotor =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,  // P2.4
        TIMER_A_OUTPUTMODE_RESET_SET,
        5000
};

Timer_A_PWMConfig leftDCMotor =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_24,
        10000,
        TIMER_A_CAPTURECOMPARE_REGISTER_3,   // P2.6
        TIMER_A_OUTPUTMODE_RESET_SET,
        5000
};


void setupS1(void)
{
    MAP_GPIO_setAsInputPinWithPullUpResistor( GPIO_PORT_P1, GPIO_PIN1 );
    MAP_GPIO_clearInterruptFlag(              GPIO_PORT_P1, GPIO_PIN1 );
    MAP_GPIO_enableInterrupt(                 GPIO_PORT_P1, GPIO_PIN1 );
    MAP_Interrupt_enableInterrupt(               INT_PORT1);
}

void setupS2(void)
{
    MAP_GPIO_setAsInputPinWithPullUpResistor( GPIO_PORT_P1, GPIO_PIN4 );
    MAP_GPIO_clearInterruptFlag(              GPIO_PORT_P1, GPIO_PIN4 );
    MAP_GPIO_enableInterrupt(                 GPIO_PORT_P1, GPIO_PIN4 );
    MAP_Interrupt_enableInterrupt(               INT_PORT1);
}

void setupADC(void)
{
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    MAP_ADC14_enableModule();
    MAP_ADC14_initModule( ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1,ADC_DIVIDER_1,0);

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);

    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
    MAP_ADC14_enableConversion();
    MAP_ADC14_enableInterrupt(ADC_INT0);
    MAP_Interrupt_enableInterrupt(INT_ADC14);
}

void setupPWM(void)
{
    leftDCMotor.dutyCycle  = 0;
    rightDCMotor.dutyCycle = 0;

    /* Configuring P4.0 and P4.2 as Output. P2.4 as peripheral output for PWM and P1.1 for button interrupt */
    // left motor direction controller
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);   // left:IN4
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2);   // Left:IN3

    /* Configuring P4.4 and P4.5 as Output. P2.4 as peripheral output for PWM and P1.1 for button interrupt */
    // right motor direction controller

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);   // right:IN2
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);   // right:IN1

    // IN3 IN4  (left wheel)
    //  0   0  nothing happens
    //  0   1  forward
    //  1   0  reverse
    //  1   1  nothing happens

    // IN1 IN2  (right wheel)
    //  0   0  nothing happens
    //  0   1  reverse
    //  1   0  forward
    //  1   1  nothing happens
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);    // left:IN4  = 1
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);     // left:IN3  = 0
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);     // right:IN2 = 0
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);    // right:IN1 = 1

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
}

void setupWheelEncoders(void)
{
    const uint_fast16_t TIMER_PERIOD = 75;
    const Timer_A_UpModeConfig timerA_config =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source, 3MHz
            TIMER_A_CLOCKSOURCE_DIVIDER_40,         // SMCLK/4          = 75KHz
            TIMER_PERIOD,                           // 75000 ticks per 1 second or 75 ticks per 1 millisecond
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };


    MAP_Timer_A_configureUpMode(TIMER_A1_BASE, &timerA_config);
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    MAP_Interrupt_enableInterrupt(INT_TA1_0);

    // set Pin 6.5 as wheel encoder
    MAP_GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN5);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN5);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P6, GPIO_PIN5 , GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN5);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN5);

    // set Pin 6.4 as wheel encoder
    MAP_GPIO_setAsInputPin(GPIO_PORT_P6, GPIO_PIN4);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P6, GPIO_PIN4 , GPIO_LOW_TO_HIGH_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN4);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);      // set P1.0 LED1 as output
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);   // Turn LED1 Red on

    MAP_Interrupt_enableInterrupt(INT_PORT6);
}

void setupUART(void)
{
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts (Rx) */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
}

void startADC(void)
{
    uPrintf("ADC14 started \n\r");

    ADC_index = 0;
    ADC_State = ON;
    MAP_ADC14_toggleConversionTrigger();
}

void stopADC(void)
{
    ADC_State = OFF;
    uPrintf("ADC14 ended \n\r");
    leftWheelSpeed  = 0.0;
    rightWheelSpeed = 0.0;
}

void startPWM(uint_fast16_t left_dutyCycle , uint_fast16_t right_dutyCycle)
{
    leftDCMotor.dutyCycle  = left_dutyCycle;
    rightDCMotor.dutyCycle = right_dutyCycle;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &rightDCMotor);
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &leftDCMotor);
}

void stopPWM(void)
{
    leftDCMotor.dutyCycle  = 0;
    rightDCMotor.dutyCycle = 0;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &rightDCMotor);
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &leftDCMotor);
}


void uPrintf(unsigned char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if( status & GPIO_PIN1 )  // switch 1
    {
        startADC();
        startPWM(7000,7800);  // left, right
    }

    if( status & GPIO_PIN4 )  // switch 2
    {
        stopADC();
        startPWM(0,0);        // left, right
    }
}

void PORT6_IRQHandler(void)
{
    static double leftWheelDistance  = 0.0;
    static double rightWheelDistance = 0.0;

    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, status);

    if (status & GPIO_PIN5)
    {
        RightWheelSpeed();
        rightWheelDistance += TICK_DISTANCE;   // one tick is 1.1 cm
    }

    if (status & GPIO_PIN4)
    {
        LeftWheelSpeed();
        leftWheelDistance += TICK_DISTANCE;    // one tick is 1.1 cm
    }

    if(leftWheelDistance > 0.0 && rightWheelDistance > 0.0)
    {
        double averageDistance = (leftWheelDistance + rightWheelDistance) * 0.5;
        leftWheelDistance  = 0.0;
        rightWheelDistance = 0.0;
        distanceTravelled += averageDistance;
    }
}

void ADC14_IRQHandler(void)
{
    static uint8_t prev     = 0;
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
                      MAP_ADC14_clearInterruptFlag(status);

    if(ADC_INT0 & status)
    {
        if(ADC_State)
        {
            uint16_t data   =  MAP_ADC14_getResult(ADC_MEM0);
            uint8_t smaller =  data  * MAX_UINT8_VAL / (SIGNAL_FILTER - 1) ;     // normalize 16 bits values to 8 bits values, since filter is set as 1000, range of 16 bits values are between 0 to 999 only

            smaller = (uint8_t)(0.2 * prev) + (uint8_t)(0.8 * smaller);          // apply smoothing, (80% of latest plus 20% of previous)

            if(ADC_index < ARRAY_SIZE && data < SIGNAL_FILTER)
            {
                x[ADC_index]     =  smaller;
                sum_of_x        +=  smaller;            // uint32_t, sum( x )
                sum_of_x_square +=  smaller * smaller;  // uint32_t, sum( x^2 )
                ADC_index++;
            }
            prev = smaller;
            MAP_ADC14_toggleConversionTrigger();
        }
        else
        {
            decodedAlphabet = ScanBarcode();
            sum_of_x        = 0;
            sum_of_x_square = 0;
            prev            = 0;
        }
    }
}

void TA1_0_IRQHandler(void)
{
    static uint16_t counter = 0;
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    rightwheelTime.milliseconds++;
    if(rightwheelTime.milliseconds == 1000)
    {
        rightwheelTime.milliseconds = 0;
        rightwheelTime.seconds++;
        if(rightwheelTime.seconds == 60)
        {
            rightwheelTime.seconds = 0;
            rightwheelTime.minutes++;
            if(rightwheelTime.minutes == 60)
            {
                rightwheelTime.minutes = 0;
                rightwheelTime.hours++;
                if(rightwheelTime.hours == 24)
                {
                    rightwheelTime.hours = 0;
                }
            }
        }
    }

    leftwheelTime.milliseconds++;
    if(leftwheelTime.milliseconds == 1000)
    {
        leftwheelTime.milliseconds = 0;
        leftwheelTime.seconds++;
        if(leftwheelTime.seconds == 60)
        {
            leftwheelTime.seconds = 0;
            leftwheelTime.minutes++;
            if(leftwheelTime.minutes == 60)
            {
                leftwheelTime.minutes = 0;
                leftwheelTime.hours++;
                if(leftwheelTime.hours == 24)
                {
                    leftwheelTime.hours = 0;
                }
            }
        }
    }
}

char guessBarcode(double average_adjustment)
{
     if(ADC_index > 1)
     {
         char        patterns[30];  // either be L or S, even index is black, odd index is white, hence pattern[0] = "S" mean short black, patterns[1] = "L" means long white
         int         patternsIndex = -2;
         int         currentColor = BLACK;
         uint32_t    patternsLength[29] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
         uint32_t    prevADCIndex = 0;
         uint32_t    i            = 0;
         uint32_t    length       = 0;
         double right_side = 0.0;
         double variance   = 0.0;
         double stdev      = 0.0;
         double average    = 0.0;
         double z_score    = 0.0;
         uint32_t total    = 0;
         uint32_t barcode  = 0;
         right_side =  ((double)(sum_of_x) / ((double)ADC_index )) * (double)(sum_of_x);
         variance   =   ((double)sum_of_x_square - right_side)/((double)(ADC_index - 1));
         stdev      = sqrt(variance);
         average    = ((sum_of_x / ((double)ADC_index)) * average_adjustment);  // adjust average to find cutoff between black and white signals.
         //printf("\ncount is %u , sum_of_x is %u , sum_of_x_square is %u , right side is %lf , average is %lf , variance is %lf , stdev is %lf\n", ADC_index , sum_of_x, sum_of_x_square , right_side, average, variance , stdev);
         //uPrintf("Started\n\r");
         for( i=0 ; i < ADC_index && i < ARRAY_SIZE ; i++ )
         {
             z_score = (((double)x[i]) - average) / stdev;
             char buffer[6];
             ltoa(x[i], buffer, 10);
             //uPrintf(strcat(buffer, "\n\r"));
             if(z_score > 0)
             {
                 // black color
                 if( currentColor == WHITE )
                 {
                     length = i - prevADCIndex;
                     //printf("Black[%i] = %u - %u = %u\n", patternsIndex, i, prevADCIndex, length);
                     currentColor = BLACK;
                     patternsIndex++;
                     if(patternsIndex > 0 && patternsIndex <= 29)
                     {
                         patternsLength[patternsIndex - 1] = length;
                         total += (double)length;
                     }
                     prevADCIndex = i;
                 }
                 //uPrintf(strcat(buffer, " (black) \n\r"));
             }
             else
             {
                 // white color
                 if(currentColor == BLACK)
                 {
                     length = i - prevADCIndex;
                     //printf("White[%i] = %u - %u = %u\n", patternsIndex, i, prevADCIndex, length);
                     currentColor = WHITE;
                     patternsIndex++;
                     if(patternsIndex > 0 && patternsIndex <= 29)
                     {
                         patternsLength[patternsIndex - 1] = length;
                         total += (double)length;
                     }
                     prevADCIndex = i;
                 }
                 //uPrintf(strcat(buffer, " (white) \n\r"));
             }
         }
         //uPrintf("Ended\n\r");

         if(patternsIndex < 29) return '?';

         int j;
         for(j=0 ; j < 29 ; j++)
         {
           patterns[j] = 0;
           if(((double)patternsLength[j] / (double)total) < SHORT_INTERVAL)
           {
               patterns[j] = 'S';
               barcode = (barcode << 1) | 0;
           }
           else
           {
               patterns[j] = 'L';
               barcode = (barcode << 1) | 1;
           }
         }
         patterns[29] = 0;
         //printf("Total length is %u , patterns = %s , barcode = %u \n", total,patterns,barcode);

         char result = '?';
         switch(barcode)
         {
             case 155460756:
             case 86279250:
                                //printf("Barcode scanned was letter 'A'\n");
                                result = 'A';
                                break;
             case 155264148:
             case 86282322:
                                //printf("Barcode scanned was letter 'B'\n");
                                result = 'B';
                                break;
             case 155525268:
             case 86021202:
                                //printf("Barcode scanned was letter 'C'\n");
                                result = 'C';
                                break;
             case 155214996:
             case 86294610:
                                //printf("Barcode scanned was letter 'D'\n");
                                result = 'D';
                                break;
             case 155476116:
             case 86033490:
                                //printf("Barcode scanned was letter 'E'\n");
                                result = 'E';
                                break;
             case 155279508:
             case 86036562:
                                //printf("Barcode scanned was letter 'F'\n");
                                result = 'F';
                                break;
             case 155202708:
             case 86343762:
                                //printf("Barcode scanned was letter 'G'\n");
                                result = 'G';
                                break;
             case 155463828:
             case 86082642:
                                //printf("Barcode scanned was letter 'H'\n");
                                result = 'H';
                                break;
             case 155267220:
             case 86085714:
                                //printf("Barcode scanned was letter 'I'\n");
                                result = 'I';
                                break;
             case 155218068:
             case 86098002:
                                //printf("Barcode scanned was letter 'J'\n");
                                result = 'J';
                                break;
             case 155454612:
             case 86377554:
                                //printf("Barcode scanned was letter 'K'\n");
                                result = 'K';
                                break;
             case 155258004:
             case 86380626:
                                //printf("Barcode scanned was letter 'L'\n");
                                result = 'L';
                                break;
             case 155519124:
             case 86119506:
                                //printf("Barcode scanned was letter 'M'\n");
                                result = 'M';
                                break;
             case 155208852:
             case 86392914:
                                //printf("Barcode scanned was letter 'N'\n");
                                result = 'N';
                                break;
             case 155469972:
             case 86131794:
                                //printf("Barcode scanned was letter 'O'\n");
                                result = 'O';
                                break;
             case 155273364:
             case 86134866:
                                //printf("Barcode scanned was letter 'P'\n");
                                result = 'P';
                                break;
             case 155196564:
             case 86442066:
                                //printf("Barcode scanned was letter 'Q'\n");
                                result = 'Q';
                                break;
             case 155457684:
             case 86180946:
                                //printf("Barcode scanned was letter 'R'\n");
                                result = 'R';
                                break;
             case 155261076:
             case 86184018:
                                //printf("Barcode scanned was letter 'S'\n");
                                result = 'S';
                                break;
             case 155211924:
             case 86196306:
                                //printf("Barcode scanned was letter 'T'\n");
                                result = 'T';
                                break;
             case 155583636:
             case 86248530:
                                //printf("Barcode scanned was letter 'U'\n");
                                result = 'U';
                                break;
             case 155387028:
             case 86251602:
                                //printf("Barcode scanned was letter 'V'\n");
                                result = 'V';
                                break;
             case 155648148:
             case 85990482:
                                //printf("Barcode scanned was letter 'W'\n");
                                result = 'W';
                                break;
             case 155337876:
             case 86263890:
                                //printf("Barcode scanned was letter 'X'\n");
                                result = 'X';
                                break;
             case 155598996:
             case 86002770:
                                //printf("Barcode scanned was letter 'Y'\n");
                                result = 'Y';
                                break;
             case 155402388:
             case 86005842:
                                //printf("Barcode scanned was letter 'Z'\n");
                                result = 'Z';
                                break;
             default:
                                //printf("Barcode scanned was unknown symbol'\n");
                                result = '?';
                                break;
         }
       return result;
     }
     else
     {
       return '?';
     }
}

char scanBarcode(void)
{
    char barcode;
    int i;
    float adjustment = 0.91;

    printf("\nGuessing barcode, please wait....\n");
    for( i = 0 ; i < 19 ; i++)
    {
        barcode = guessBarcode(adjustment);
        if( barcode != '?' )
        {
         printf("Barcode found : '%c'\n",barcode);
         decodedAlphabet = barcode;
         return barcode;
        }
        else
        {
            printf("(attempt %i) guessing barcode with adjustment of %0.2f, unsuccessful...\n",i+1,adjustment);
            adjustment += 0.01;
        }
    }
    return '?';
}
double getWheelTime( wheelTime *wt )
{
    uint32_t result = ( ( ( (wt->hours * 60) +  wt->minutes ) * 60 ) + wt->seconds ) * 1000 + wt->milliseconds ;
    wt->hours = 0;
    wt->minutes = 0;
    wt->seconds = 0;
    wt->milliseconds = 0;

//    ltoa(result,buffer,10);
//    uPrintf(strcat(buffer, " milliseconds\n\r"));

    return ((double)result / 1000);
}

void leftWheelSpeed(void)
{
    if(leftDCMotor.dutyCycle == 0)
    {
        leftWheelSpeed = 0.0;
    }
    else
    {
        double seconds  = getWheelTime( &leftwheelTime );
        leftWheelSpeed  = TICK_DISTANCE / seconds; // distance in cm / time in seconds = speed in cm/s
    }
}

void rightWheelSpeed(void)
{
    if(rightDCMotor.dutyCycle == 0)
    {
        rightWheelSpeed = 0.0;
    }
    else
    {
        double seconds  = getWheelTime( &rightwheelTime );
        rightWheelSpeed = TICK_DISTANCE / seconds; // distance in cm / time in seconds = speed in cm/s
    }
}
