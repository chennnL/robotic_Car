/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefor I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO PICO_DEFAULT_I2C_SDA_PIN (On Pico this is GP4 (pin 6)) -> SDA on MPU6050 board
   GPIO PICO_DEFAULT_I2C_SCL_PIN (On Pico this is GP5 (pin 7)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/

#define DSP_EMA_I32_ALPHA(x) ( (uint16_t)(x * 65535) )

#define MM_FORMULA 18

// By default these devices  are on bus address 0x68
static int addr = 0x68; //default address in the datasheet by I2C 

#ifdef i2c_default
static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x00}; //device defaults to sleep mode
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3]) { //int16_t *temp, int16_t gyro[3]
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]); //read the values 
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    // val = 0x43;
    // i2c_write_blocking(i2c_default, addr, &val, 1, true);
    // i2c_read_blocking(i2c_default, addr, buffer, 6, false);  // False - finished with bus

    // for (int i = 0; i < 3; i++) {
    //     gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    // }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    // val = 0x41;
    // i2c_write_blocking(i2c_default, addr, &val, 1, true);
    // i2c_read_blocking(i2c_default, addr, buffer, 2, false);  // False - finished with bus

    // *temp = buffer[0] << 8 | buffer[1];
}
#endif


int movingAvg(int *ptrArrNumbers, long *ptrSum, int pos, int len, int nextNum) //moving average algorithm 
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}

void convertAcceleration(int16_t arr[]) {
  float accelArr[3];
  // the size of this array represents how many numbers will be used
  // to calculate the average
  int arrNumbers[5] = {0};

  int pos = 0;
  long sum = 0;
  int len = sizeof(arrNumbers) / sizeof(int);
  int count = sizeof(accelArr) / sizeof(int);


  for(int i = 0; i < count; i++){
    accelArr[i] = movingAvg(arrNumbers, &sum, pos, len, arr[i]); //filter the acceleration 

    //accelArr[i] = accelArr[i] / 2048; //divide by 2048 based on datasheet (sensitivity)
    pos++;
    if (pos >= len){
      pos = 0;
    }
  }

  //algo to obtain difference in height

    static bool flag = true;
    static int diff = 0;
    static float diffToHeight = 0;
    static float height = 0;

    static int prevVal = 0;
    static int nextVal = 0;

  //------------------------------------------------------------------------------------------------------
  if (flag)
  {
      prevVal = nextVal = accelArr[2];
      flag = false;
  }
  else
  {
      nextVal = accelArr[2];
  }

  diff = nextVal - prevVal;
  diffToHeight = diff / 18.0;

  // Height to be printed out on the screen (should start at 0)
  height = (height + diffToHeight) / 10;
  printf("Height: %.2f\n", height);
  // printf("prevVal: %d\n", prevVal);
  // printf("nextVal: %d\n", nextVal);


  // Previous value now takes this value
  prevVal = nextVal;

  // Try to slow the accelerometer down a lil so that the values aren't as fast
//--------------------------------------------------------------------------------------------------------


  // if (accelArr[2] < 1.7) {
  //   if (accelArr[2] - 1.4 >= 0) {
  //     accelArr[2] = accelArr[2] - 1.4;  //Harcoded 
  //   }
  //   else {
  //     accelArr[2] = 0.2;
  //   }
  // }

  //printf("%.2f\n", accelArr[2]);

}

int main() {
    stdio_init_all();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    printf("Hello, MPU6050! Reading raw data from registers...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000); // Sets baudrate to 400 kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();

    int16_t acceleration[3]; //temp, gyro[3]

    while (1) {
        mpu6050_read_raw(acceleration); // &temp, gyro
        convertAcceleration(acceleration);


        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
        // printf("\n");
        // printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);

        // printf("\n");

        // printf("\n");
        // printf("Acc. X = %d, Y = %d, Z = %d\n", average_x, average_y, average_z);
        // printf("\n");

       

        // printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
        // printf("Temp. = %f\n", (temp / 340.0) + 36.53);
        //accel_x=(((1-alpha)*(gyro[0]+accel_x)+(alpha)*(acceleration[0]))/acceleration[0]);

        sleep_ms(2500);
    }

#endif
    return 0;
}
