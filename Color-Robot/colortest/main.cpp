#include "mbed.h"
#include "Color.h"
#include "rtos.h"
#include "Robot.h"
Mutex lcd_lock;
Timer t;
Ticker distance_check;


//PwmOut r(p23);
//PwmOut g(p24);
//PwmOut b(p25);

float RightSpeed = 0.0; //speed right motor
float LeftSpeed = 0.0; //speed left motor

int status;
uint32_t distance;
Thread t1;
Thread t2;
Thread t3;

float r, g, b;
volatile bool white = false;

//C union can convert 4 chars to a float - puts them in same location in memory
//trick to pack the 4 bytes from Bluetooth serial port back into a 32-bit float
union f_or_char {
    float f;
    char  c[4];
};

//void led_thread() {
//    while (true) {
//         r = color_sensor_red()/255.0f;
//         g = color_sensor_green()/255.0f;
//         b = color_sensor_blue()/255.0f;
//        Thread::wait(1);
//    } 
//}

void lcd_color() {
    while(1) {
        r = color_sensor_red();
        g = color_sensor_green();
        b = color_sensor_blue();
        int color = 256 * 256 * r + 256 * g + b;
        lcd_lock.lock();
        if(!white) {
            uLCD.filled_rectangle(0, 0, 128, 128, WHITE);
            white = true;
        }
        uLCD.filled_circle(64, 64, 30, color);
        lcd_lock.unlock();
    }
}


void blue_thread(){
    printf("started blue thread\n");
    char bchecksum=0;
    char temp=0;
    union f_or_char x,y,z;
    t.start();
    while(1) {
        printf("in blue loop\n");
        if (bluemod.readable()) {
            lcd_lock.lock();
            t.stop(); //don't know if this is necessary
            t.reset();
            t.start();
            bchecksum=0;
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
                            RightSpeed = 0.0; //Scale to 0.0 to 1.0 for PWM so /10.0
                            LeftSpeed = -y.f/10.0;
                            motorR.speed(0.35);
                            motorL.speed(0.35);
                        }
                        else if ((x.f<=0.5) && (y.f<=-0.5)) { //phone turned left
                            RightSpeed = -y.f/10.0; //Scale to 0.0 to 1.0 for PWM so /10.0
                            LeftSpeed = 0.0;
                            motorR.speed(-0.35);
                            motorL.speed(-0.35);
                        }
                        else if ((x.f<=0.5 && x.f >= -0.5) && (y.f<=0.5 && y.f>=-0.5) && (z.f <= -0.5)){ //forward
                            motorR.speed(0.35);
                            motorL.speed(-0.35);
                        }
                        else if ((x.f<=0.5 && x.f >= -0.5) && (y.f<=0.5 && y.f>=-0.5) && (z.f >= 0.5)){ //reverse
                            motorR.speed(-0.35);
                            motorL.speed(0.35);
                        }
                        else {
                            RightSpeed = 0.0;
                            LeftSpeed = 0.0;
                            motorR.speed(0);
                            motorL.speed(0);
                        }
                    }
                }
                
            }
        lcd_lock.unlock();
        } else if (t.read() > 5) {
            motorR.speed(0);
            motorL.speed(0);
        }
        //probably should put a thread::wait in here?
    }
}

/*void check_distance() {
    printf("checking distance\n");
    status = -1;
    while(status == -1) { //can use GPIO pin instead
        status = board->sensor_centre->get_distance(&distance);
    }
    if (status == VL53L0X_ERROR_NONE) {
        printf("D=%ld mm\r\n", distance);
            if (distance < 200) {
                t1.terminate();
                motorR.speed(0);
                motorL.speed(0);
                wait(0.5);
                t1.start(blue_thread);
            }
    }
} */



int main()
{
    //DevI2C *device_i2c = new DevI2C(VL53L0_I2C_SDA, VL53L0_I2C_SCL);
    /* creates the 53L0A1 expansion board singleton obj */
    //board = XNucleo53L0A1::instance(device_i2c, A2, D8, D2);
    /*shdn = 0; //must reset sensor for an mbed reset to work
    wait(0.1);
    shdn = 1;
    wait(0.1); */
    /* init the 53L0A1 board with default values */
    //status = board->init_board();
    /*while (status) {
        printf("Failed to init board! \r\n");
        status = board->init_board();
    } */
    
    //distance_check.attach(&check_distance, 5);
    pc.printf("Start of program\n");
    color_sensor_init();
    pc.printf("Color sense init\n");
    pc.printf("lcd init\n");
    t1.start(blue_thread);
    pc.printf("bluethread start\n");
    //t2.start(led_thread);
    t3.start(lcd_color);
    pc.printf("lcdthread start\n");
    while(1) {
        //pc.printf("loop\n");
    }
}
