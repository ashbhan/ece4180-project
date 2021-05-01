#include "mbed.h"
#include "color_sensor.h"

Serial pc(USBTX, USBRX);

int main()
{
    color_sensor_init();

    while (true) {
        pc.printf("Red: (%f)", color_sensor_red());
        pc.printf("Green: (%f)", color_sensor_green());
        pc.printf("Blue: (%f)", color_sensor_blue());
        wait(1);
    }
}
