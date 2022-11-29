# robotic_Car
#PWM & PID



Ports Used

Port 4 Pin 0 -> Motor

Port 4 Pin 2 -> Motor

Port 4 Pin 4 -> Motor

Port 4 Pin 5 -> Motor

Port 2 Pin 4 -> Motor Left Out

Port 2 Pin 5 -> Motor Right Out



Functions we have
1) Motor_Drive_Forward_Slow
- Motor moves slowly
- PWM set at 5000,5000
- - target for fast pwm is 20 notch/second


3) Motor_Drive_Forward_Fast
- Motor moves fast
- Good for driving over humps
- PWM set at 9000,9000
- target for fast pwm is 25 notch/second

4) Motor_Turn_Left
- Motor turns left by 90 degrees

5) Motor_Turn_Right
- Motor turns right by 90 degrees

6) Motor_uTurn
- Motor uTurns
- Motor turns by 180 degrees (to the right)

7) Motor_Stop
- Motor stops moving

8) Motor_Reverse
- Motor reverses, wheels move backwords