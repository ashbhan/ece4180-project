#include "mbed.h"
//#include "XNucleo53L0A1.h"
#include <stdio.h>
#include "Motor.h"
#include "rtos.h"
#include "uLCD_4DGL.h"
Serial pc(USBTX,USBRX);

/* Pin Definitions */
/* Distance Sensor */
#define VL53L0_I2C_SDA   p28
#define VL53L0_I2C_SCL   p27
DigitalOut shdn(p29);


/* H-Bridge */
#define PWMA p21 //p25
#define PWMB p22 //p24
#define AI1 p5
#define AI2 p6
#define BI1 p7
#define BI2 p8
Motor motorR(PWMA, p6, p5);
//Motor motorL(PWMB, p15, p14);
Motor motorL(PWMB, p8, p7);

/* Color Sensor */
//#define COLOR_SDA p9
//#define COLOR_SCL p10
//need init function

/* Bluetooth Module */
#define BLUE_RX p27 //p14
#define BLUE_TX p28 //p13
Serial bluemod(p13, p14);
uLCD_4DGL uLCD(p13, p14, p29);






