#include "mbed.h"
#include "color_sensor.h"

Serial pc(USBTX, USBRX);    //Used to view the colors that are read in

int main()
{
    pc.baud(9600);

    color_sensor_init();

    while (true) {
        uint32_t color = color_sensor();

        pc.printf("Color (%x)\n", color);
        wait(1);
    }
}