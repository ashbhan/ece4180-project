#ifndef COLOR_SENSOR
#define COLOR_SENSOR

I2C i2c_color_sensor(p9, p10); //pins for I2C communication (SDA, SCL)

int sensor_addr = 41 << 1;

void color_sensor_init()
{
    char id_regval[1] = {146};
    char data[1] = {0};
    i2c_color_sensor.write(sensor_addr,id_regval,1, true);
    i2c_color_sensor.read(sensor_addr,data,1,false);

    if (data[0]==68) {
        wait (1);
    }

    char timing_register[2] = {129,0};
    i2c_color_sensor.write(sensor_addr,timing_register,2,false);

    char control_register[2] = {143,0};
    i2c_color_sensor.write(sensor_addr,control_register,2,false);

    char enable_register[2] = {128,3};
    i2c_color_sensor.write(sensor_addr,enable_register,2,false);
}

float color_sensor_red()
{
    char clear_reg[1] = {148};
    char clear_data[2] = {0,0};
    i2c_color_sensor.write(sensor_addr,clear_reg,1, true);
    i2c_color_sensor.read(sensor_addr,clear_data,2, false);

    float clear = (((int)clear_data[1] << 8) | clear_data[0]) * 1.0f;

    char red_reg[1] = {150};
    char red_data[2] = {0,0};
    i2c_color_sensor.write(sensor_addr,red_reg,1, true);
    i2c_color_sensor.read(sensor_addr,red_data,2, false);

    float red = (((uint32_t)red_data[1] << 8) | red_data[0]) * 1.0f;;

    red = 255 * red / clear;

    return red;
}

float color_sensor_green()
{
    char clear_reg[1] = {148};
    char clear_data[2] = {0,0};
    i2c_color_sensor.write(sensor_addr,clear_reg,1, true);
    i2c_color_sensor.read(sensor_addr,clear_data,2, false);

    float clear = (((int)clear_data[1] << 8) | clear_data[0]) * 1.0f;

    char green_reg[1] = {152};
    char green_data[2] = {0,0};
    i2c_color_sensor.write(sensor_addr,green_reg,1, true);
    i2c_color_sensor.read(sensor_addr,green_data,2, false);

    float green = (((uint32_t)green_data[1] << 8) | green_data[0]) * 1.0f;

    green = 255 * green / clear;

    return green;
}

float color_sensor_blue()
{
    char clear_reg[1] = {148};
    char clear_data[2] = {0,0};
    i2c_color_sensor.write(sensor_addr,clear_reg,1, true);
    i2c_color_sensor.read(sensor_addr,clear_data,2, false);

    float clear = (((int)clear_data[1] << 8) | clear_data[0]) * 1.0f;

    char blue_reg[1] = {154};
    char blue_data[2] = {0,0};
    i2c_color_sensor.write(sensor_addr,blue_reg,1, true);
    i2c_color_sensor.read(sensor_addr,blue_data,2, false);

    float blue = (((uint32_t)blue_data[1] << 8) | blue_data[0]) * 1.0f;

    blue = 255 * blue / clear;

    return blue;
}

#endif
