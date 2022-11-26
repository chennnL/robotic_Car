# Infrared (Wheel Encoder & Barcode Scanner)
Refer to Barcode Analysis.docx for our detailed analysis.

/*---------------------------------------------------------------------------------------------------------------*/

Port & Pins Used:  
  
(1) P5.5           (ADC  input)    
(2) P6.5 and P6.4  (GPIO inputs)  

/*---------------------------------------------------------------------------------------------------------------*/

Defines:

(1) #define ON  1  
(2) #define OFF 0  
(3) #define ARRAY_SIZE      20000  
(4) #define SIGNAL_FILTER   1000 
(5) #define MAX_UINT8_VAL   255  
(6) #define SHORT_INTERVAL  0.04  
(7) #define BLACK           0  
(8) #define WHITE           1  
(9) #define TICK_DISTANCE   1.1    // each tick for wheel encoder 1.1 cm apart  
(10) typedef struct _wheelTime  
      {  
          uint16_t  milliseconds;  
          uint16_t       seconds;  
          uint16_t       minutes;  
          uint16_t         hours;  
      } wheelTime;  
  
/*---------------------------------------------------------------------------------------------------------------*/  
  
Global Variables:  
  
(1) static volatile uint8_t   ADC_State;  
(2) static volatile uint32_t  ADC_index;  
(3) static volatile uint32_t  sum_of_x_square;   // this is sum(x^2)  
(4) static volatile uint32_t  sum_of_x;          // this is sum(x)  
(5) static volatile uint8_t   x[ARRAY_SIZE];     // this is array of x - normalized 8 bit values (convert ADC values to 0-255 values)  
(6) static volatile wheelTime leftwheelTime = {0,0,0,0};  
(7) static volatile wheelTime rightwheelTime = {0,0,0,0};  
(8) static volatile double    leftWheelSpeed;  
(9) static volatile double    rightWheelSpeed;  
(10) static volatile char     decodedAlphabet;  
  
/*---------------------------------------------------------------------------------------------------------------*/  
  
Infrared Submodule Functions:  
  
(1) void setupADC(void);  
(2) void startADC(void);  
(3) void stopADC(void);  
(4) char guessBarcode(double);  
(5) char scanBarcode(void);  
(6) void setupWheelEncoders(void);  
(7) double getWheelTime( wheelTime* );  
(8) void leftWheelSpeed(void);  
(9) void rightWheelSpeed(void);  
  
/*---------------------------------------------------------------------------------------------------------------*/  

Interrupt Used:  
  
(1) void ADC14_IRQHandler(void);   // ADC_INT0  
(2) void TA1_0_IRQHandler(void);   // TIMER_A1_BASE  
(3) void PORT6_IRQHandler(void);    
  
/*---------------------------------------------------------------------------------------------------------------*/  
  
