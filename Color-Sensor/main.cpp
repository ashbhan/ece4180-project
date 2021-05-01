#include "mbed.h"
#include "color_sensor.h"

int main()
{
    color_sensor_init();

    while (true) {
        color_sensor();
        wait(1);
    }
}
