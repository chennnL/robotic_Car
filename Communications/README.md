# COMMUNICATIONS (MSP432, ESP01/8266, M5StickCPlus, UART)


#### Port & Pins Used:  
  M5StickCPlus UART
(1) P3.2           (TX)     
(2) P3.3           (RX)  
(3) 3.3V
(4) GND

ESP01/8266
(1) P3.2
(2) P3.3
(3) P6.1
(4) GND 
(5) & (6) 3.3V






#### Defines:


  
  
#### Global Variables:  
  const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // Baud Rate 115200
        1,                                             // BRDIV = 1
        10,                                              // UCxBRF = 10
        0,                                              // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                        // NO Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};

  
  
#### Communications Submodule Functions:  
Communication between MSP and M5StickCPlus via UART
Communication between MSP and ESP01/8266 via UART
Communication between M5StickCplus and external devices through connection of AP(Access Point) and usage of web server (html)
Web server display data (HC-SR04)
Displaying data of sensor on M5StickCPlus LCD (HC-SR04)
Plot charts to illustrate the difference in performance (M5StickCPlus & ESP01)
Video Demonstration

  

  

#### Interrupt Handler used:  
EUSCIA2_IRQHandler
  
