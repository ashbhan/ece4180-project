#include "mbed.h"
#include "XNucleo53L0A1.h"
#include <stdio.h>
#include "Motor.h"
Serial pc(USBTX,USBRX);

/* Pin Definitions */
/* Distance Sensor */
#define VL53L0_I2C_SDA   p28
#define VL53L0_I2C_SCL   p27
DigitalOut shdn(p26);
static XNucleo53L0A1 *board=NULL;

/* H-Bridge */
#define PWMA p25
#define PWMB p24
#define AI1 p5
#define AI2 p6
#define BI1 p7
#define BI2 p8
Motor leftMotor(PWMA, AI1, AI2);
Motor rightMotor(PWMB, BI1, BI2);

/* Color Sensor */
#define COLOR_SDA p9
#define COLOR_SCL p10
//need init function

/* Bluetooth Module */
#define BLUE_RX p14
#define BLUE_TX p13
Serial bluemod(BLUE_TX, BLUE_RX);




