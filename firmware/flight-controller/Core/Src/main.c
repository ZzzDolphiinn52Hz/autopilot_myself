#include "delay.h"
#include "icm42688.h"
#include "clock.h"

volatile float accel_x_g = 0.0f;
volatile float accel_y_g = 0.0f;
volatile float accel_z_g = 0.0f; // gia toc
volatile float gyro_x_dps = 0.0f;
volatile float gyro_y_dps = 0.0f;
volatile float gyro_z_dps = 0.0f; // toc do goc
volatile float temperature_c = 0.0f;

int main(void)
{
    clock_init();
    delay_init();
    ICM42688_Init();

    while (1)
    {
        ICM42688_Data_t sensor_data;

        ICM42688_ReadSensorData(&sensor_data);

        accel_x_g = sensor_data.accel_x_g;
        accel_y_g = sensor_data.accel_y_g;
        accel_z_g = sensor_data.accel_z_g;
        gyro_x_dps = sensor_data.gyro_x_dps;
        gyro_y_dps = sensor_data.gyro_y_dps;
        gyro_z_dps = sensor_data.gyro_z_dps;
        temperature_c = sensor_data.temperature_c;

        delay_ms(10);
    }
}