#include "mbed.h"
#include "color_sensor.h"

Serial pc(USBTX, USBRX);

int main()
{
    color_sensor_init();

    while (true) {
        pc.printf("Red: (%d) ", color_sensor_red());
        pc.printf("Green: (%d) ", color_sensor_green());
        pc.printf("Blue: (%d)\n", color_sensor_blue());
        wait(1);
    }
}
