#References from https://docs.m5stack.com/en/mpy/advanced/uart self coded by communications team 
#https://stackoverflow.com/questions/37016946/remove-b-character-do-in-front-of-a-string-literal-in-python-3
#https://github.com/m5stack/UIFlow-Code 

from m5stack import *
from m5ui import * 
from uiflow import * 

label0 = M5TextBox(25,100, "DATA", lcd.FONT_Default, 0XFFFFFF, rotate=0) #Create a label textbox at position (x,y), Default font, White colour, no rotation

setScreenColor(0x111111) #setting screen color to black
uart1 = machine.UART(1,tx=32,rx=33) #Create a serial port instance with transmitter on port 32 and receiver on port 33
uart1.init(115200, bits=8, parity=None, stop=1) #Initialize the serial port with 115200 baud rate, 8 bits, no parity bit, and 1 stop bit
while True:
    if uart1.any(): #If there is any content in the cache
        label0.setText("Data = " + uart1.readline().decode("utf-8")+ "cm \n ") #Set the label's text as Data = (uart's read) (Decoded bytes) (cm new line)