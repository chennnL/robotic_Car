#Referred from https://www.hackster.io/lukasmaximus89/m5stack-micropython-simple-web-server-20a4c4
#Darren
#Teo Leng
#Jia Qi

from m5stack import *
from m5ui import *
from uiflow import *
import network      #import to connect to M5Stack AP

distance="0"         #Distance placeholder
label0 = M5TextBox(25,100, "DATA", lcd.FONT_Default, 0XFFFFFF, rotate=0)        #create label for m5stick to display data
setScreenColor(0x111111) #setting screen color to black     
uart1 = machine.UART(1,tx=32,rx=33) #Create a serial port instance with transmitter on port 32 and receiver on port 33
uart1.init(115200, bits=8, parity=None, stop=1) #Initialize the serial port with 115200 baud rate, 8 bits, no parity bit, and 1 stop bit 



try:
    import usocket as socket
except:
    import socket






def web_page():
  html = """<html><head>
    <title>CSC2003 Final</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="refresh" content="5" >
    <style>
      body { text-align: center; font-family: 'Trebuchet MS', 'Times New Roman';}
      table {border-collapse: collapse; width: 35%%; margin-left: auto; margin-right: auto; margin-bottom: 50px;}
      th {padding: 10px; background-color: #4B0082; color: white;}
      tr {border: 2px solid #ddd ;padding: 10px;}
      tr:hover {background-color: #bcbcbc;}
      td {border: none; padding: 12px;}
    </style>
  </head>
  <body>
    <h1>COMMUNICATIONS</h1>
    <table>
      <tr>
        <th>DATA</th>
        <th>VALUE</th>
      </tr>
      <tr>
        <td>Barcode:</td>
        <td></td>
      </tr>
      <tr>
        <td>Distance:</td>
        <td>""" + distance + """"cm<td>
      </tr>
      <tr>
        <td>Hump Height:</td>
        <td>  </td>
      </tr>
      <button type="button">START SCAN!</button> 
      <style></style>
      <button type="button">STOP SCAN!</button>


    </table>
  </body></html>"""
  return html





ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid='M5Stack')
ap.config(authmode=3, password='12345678')
lcd.clear()

response = None
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('192.168.4.1',80))
s.listen(5)

while True:
    conn, addr = s.accept()
    # request = conn.recv(1024)
    # request = str(request)
    # print ('Content = %s' % request)
    # lcd.print('Content = %s' % request,0,50,0xffffff)
    # if ap.isconnected() == True:
    #     lcd.print('connected',0,0,0xffffff)
    # else:
    #     lcd.print('not connected',0,0,0xffffff)
    response = web_page()
    # conn.send('HTTP/1.1 200 OK\n')
    # conn.send('Content-Type: text/html\n')
    # conn.send('Connection: close\n\n')
    while uart1.any(): #If there is any content in the cache
        if (uart1.read(10).decode("ascii")[0:3] == "696"):
            label0.setText("Data = " + uart1.read(10).decode("ascii")[4:8] + " cm ")
            distance = uart1.read(10).decode("ascii")[5:9] 
        break       
    conn.sendall(response)
    conn.close()
    while uart1.any(): #If there is any content in the cache
        if (uart1.read(10).decode("ascii")[0:3] == "696"):
            label0.setText("Data = " + uart1.read(10).decode("ascii")[4:8] + " cm ")
            distance = uart1.read(10).decode("ascii")[5:9] 
        break       

