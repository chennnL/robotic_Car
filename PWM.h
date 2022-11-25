/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>



void Motor_Init();
void Motor_Drive_Forward_Slow();
//void Motor_Drive_Forward_Middle();
void Motor_Drive_Forward_Fast();
void Motor_Turn_Left();
void Motor_Turn_Right();
void Motor_uTurn();
void Motor_Stop();
void Motor_Reverse();
void PID();
//void straightenCar(int leftDistanceToWall, int rightDistanceToWall)();
