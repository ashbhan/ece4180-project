#include "mbed.h"
#include "Motor.h"
#include "rtos.h"
#include "XNucleo53L0A1.h"
#include <stdio.h>
Serial bluemod(p9,p10);
Motor motorR(p21, p6, p5); //right wheel
Motor motorL(p22, p15, p14); //left wheel
DigitalOut shdn(p17);
DigitalOut Red(p19);
DigitalOut Green(p20);
// This VL53L0X board test application performs a range measurement in polling mode
// Use 3.3(Vout) for Vin, p28 for SDA, p27 for SCL, P26 for shdn on mbed LPC1768

//I2C sensor pins
#define VL53L0_I2C_SDA   p28
#define VL53L0_I2C_SCL   p27

static XNucleo53L0A1 *board=NULL;

enum LED_state 
{
    STOPPED = 0,
    FORWARD = 1,
    TURNING = 2,
    REVERSE = 3
};
    
LED_state led_state;

Mutex lock;
Timer t;
Ticker distance_check;

float RightSpeed = 0.0; //speed right motor
float LeftSpeed = 0.0; //speed left motor

int status;
uint32_t distance;

bool checking_distance;

Thread t1;
Thread t2;

//C union can convert 4 chars to a float - puts them in same location in memory
//trick to pack the 4 bytes from Bluetooth serial port back into a 32-bit float
union f_or_char {
    float f;
    char  c[4];
};

void LED_thread() { //LED states to indicate direction/motion
    while(1){
        switch (led_state) {
            case STOPPED: 
                Red = 1;
                Green = 0;
                break;
            case FORWARD:
                Red = 0;
                Green = 1;
                break;
            case TURNING:
                Red = 0;
                Green = !Green;
                Thread::wait(50);
                break;
            case REVERSE:
                Green = 0;
                Red = !Red;
                Thread::wait(50);
                break;
        }
    }
}

void blue_thread(){
    printf("started blue thread\n");
    char bchecksum=0;
    char temp=0;
    union f_or_char x,y,z;
    t.start();
    while(1) {
       // printf("in blue loop\n");
        if (bluemod.readable()) {
          if (lock.trylock()){
            t.stop(); //don't know if this is necessary
            t.reset();
            t.start();
            bchecksum=0;
            Green = 1;
            Red = 0;
            if (bluemod.getc()=='!') {
                if (bluemod.getc()=='A') { //Accelerometer data packet
                    for (int i=0; i<4; i++) {
                        temp = bluemod.getc();
                        x.c[i] = temp;
                        bchecksum = bchecksum + temp;
                    }
                    for (int i=0; i<4; i++) {
                        temp = bluemod.getc();
                        y.c[i] = temp;
                        bchecksum = bchecksum + temp;
                    }
                    for (int i=0; i<4; i++) {
                        temp = bluemod.getc();
                        z.c[i] = temp;
                        bchecksum = bchecksum + temp;
                    }
                    if (bluemod.getc()==char(~('!' + 'A' + bchecksum))) { //checksum OK?
                        //printf("X = %f  Y = %f  Z = %f\n\r",x.f, y.f, z.f);
                        //right_led = left_led = forward_led = reverse_led = 0.0;
                        if ((x.f<=0.5) && (y.f>=0.5)) { //phone turned right
                            motorR.speed(-0.5);
                            motorL.speed(0.5);
                            led_state = TURNING;
                        }
                        else if ((x.f<=0.5) && (y.f<=-0.5)) { //phone turned left
                            motorR.speed(0.5);
                            motorL.speed(-0.5);
                            led_state = TURNING;  
                        }
                        else if ((x.f<=0.5 && x.f >= -0.5) && (y.f<=0.5 && y.f>=-0.5) && (z.f <= -0.5)){ //forward
                            motorR.speed(0.5);
                            motorL.speed(0.5);
                            led_state = FORWARD;
                        }
                        else if ((x.f<=0.5 && x.f >= -0.5) && (y.f<=0.5 && y.f>=-0.5) && (z.f >= 0.5)){ //reverse
                            motorR.speed(-0.5);
                            motorL.speed(-0.5);
                            led_state = REVERSE;
                        }
                        else {
                            RightSpeed = 0.0;
                            LeftSpeed = 0.0;
                            motorR.speed(0);
                            motorL.speed(0);
                            led_state = STOPPED;
                        }
                    }
                }
            }
            lock.unlock();
          } else {
                while(bluemod.readable()) {
                    bluemod.getc(); //clear the buffer
                }
            } 
        } else if (t.read() > 5) {
            motorR.speed(0);
            motorL.speed(0);
            led_state = STOPPED;
        }
        //probably should put a thread::wait in here?
    }
}

int main()
{
    DevI2C *device_i2c = new DevI2C(VL53L0_I2C_SDA, VL53L0_I2C_SCL);
    /* creates the 53L0A1 expansion board singleton obj */
    board = XNucleo53L0A1::instance(device_i2c, A2, D8, D2);
    shdn = 0; //must reset sensor for an mbed reset to work
    wait(0.1);
    shdn = 1;
    wait(0.1);
    /* init the 53L0A1 board with default values */
    status = board->init_board();
    while (status) {
        printf("Failed to init board! \r\n");
        status = board->init_board();
    }
    led_state = STOPPED;
    t2.start(LED_thread);
    t1.start(blue_thread);
    while(1) {
        printf("checking distance\n");
        status = -1;
        while(status == -1) { //can use GPIO pin instead
            status = board->sensor_centre->get_distance(&distance);
        }
        if (status == VL53L0X_ERROR_NONE) {
            printf("D=%ld mm\r\n", distance);
                if (distance < 200) { //if distance is less than 200mm, reverse for 1 second.
                    lock.lock();
                    motorR.speed(0);
                    motorL.speed(0);
                    printf("stopped");
                    led_state = STOPPED;
                    wait(0.5);
                    motorR.speed(-0.5);
                    motorL.speed(-0.5);
                    led_state = REVERSE;
                    wait(1);
                    motorR.speed(0);
                    motorL.speed(0);
                    led_state = STOPPED;
                } else {
                    lock.unlock();
                }
            }
        }
}
