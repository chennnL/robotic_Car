/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>




/* Variable */
volatile static int16_t go_counter = 0;
volatile static int16_t go_left_counter = 0;
volatile static int16_t go_right_counter = 0;
volatile static int16_t go_prev_error_left = 0;
volatile static int16_t go_prev_error_right = 0;
volatile static int16_t go_sum_error_left = 0;
volatile static int16_t go_sum_error_right = 0;

volatile static float go_pid_left = 0;
volatile static float go_pid_right = 0;

volatile static uint16_t left_duty_cycle = 0;
volatile static uint16_t right_duty_cycle = 0;

volatile static uint16_t current_DC = 6000; // 60% Duty Cycle
volatile char car_move;

/* Method */
void Motor_Init();
void Motor_Drive_Forward_Slow();
//void Motor_Drive_Forward_Middle();
void Motor_Drive_Forward_Fast();
void Motor_Turn_Left();
void Motor_Turn_Right();
void Motor_uTurn();
void Motor_Stop();
void Motor_Reverse();
void PID_Target();
//void straightenCar(int leftDistanceToWall, int rightDistanceToWall)();
