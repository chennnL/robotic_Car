# robotic_Car
## PWM & PID



### Ports Used
Port 4 Pin 0 -> Motor
Port 4 Pin 2 -> Motor
Port 4 Pin 4 -> Motor
Port 4 Pin 5 -> Motor
Port 2 Pin 4 -> Motor Left Out
Port 2 Pin 5 -> Motor Right Out



### Functions we have
#### Motor_Drive_Forward_Slow
- Motor moves slowly
- PWM set at 5000,5000
- - target for fast pwm is 19 notch/second


####  Motor_Drive_Forward_Fast
- Motor moves fast
- Good for driving over humps
- PWM set at 7000,7000
- target for fast pwm is 22 notch/second

####  Motor_Turn_Left
- Motor turns left by 90 degrees

####  Motor_Turn_Right
- Motor turns right by 90 degrees

####  Motor_Turn_Right_45
- Motor turns right by 45 degrees

####  Motor_Turn_Left_45
- Motor turns left by 45 degrees

####  Motor_uTurn
- Motor uTurns
- Motor turns by 180 degrees (to the right)

####  Motor_Stop
- Motor stops moving

####  Motor_Reverse
- Motor reverses, wheels move backwords


### Author
Chen Wujie 2101169
Izz Danial 2100735
