#include "icm42688.h"
#include "delay.h"
#include "spi.h"

#define ICM42688_BANK0                 0x00
#define ICM42688_SOFT_RESET           0x01
#define ICM42688_PWR_ACCEL_GYRO_LN    0x0F
#define ICM42688_FILTER_BW_ODR_DIV_4  0x11

static ICM42688_AccelFullScale_t accel_full_scale = ICM42688_ACCEL_FS_16G;
static ICM42688_GyroFullScale_t gyro_full_scale = ICM42688_GYRO_FS_2000DPS;

volatile int16_t debug_icm42688_accel_x;
volatile int16_t debug_icm42688_accel_y;
volatile int16_t debug_icm42688_accel_z;
volatile int16_t debug_icm42688_gyro_x;
volatile int16_t debug_icm42688_gyro_y;
volatile int16_t debug_icm42688_gyro_z;
volatile int16_t debug_icm42688_temp;
volatile uint8_t debug_icm42688_who_am_i;

static ICM42688_StatusTypeDef ICM42688_ConvertSPIStatus(SPI_Status_t status)
{
    if (status == SPI_OK)
    {
        return ICM42688_OK;
    }

    if (status == SPI_TIMEOUT)
    {
        return ICM42688_TIMEOUT;
    }

    return ICM42688_ERROR;
}

static ICM42688_StatusTypeDef ICM42688_WriteReg(uint8_t reg, uint8_t data)
{
    return ICM42688_ConvertSPIStatus(SPI_mem_write((uint8_t)(reg | ICM42688_SPI_WRITE_BIT), &data, 1));
}

static ICM42688_StatusTypeDef ICM42688_ReadReg(uint8_t reg, uint8_t *data)
{
    if (data == 0)
    {
        return ICM42688_ERROR;
    }

    return ICM42688_ConvertSPIStatus(SPI_mem_read((uint8_t)(reg | ICM42688_SPI_READ_BIT), data, 1));
}

static ICM42688_StatusTypeDef ICM42688_ReadRegs(uint8_t reg, uint8_t *data, uint8_t len)
{
    if ((data == 0) || (len == 0))
    {
        return ICM42688_ERROR;
    }

    return ICM42688_ConvertSPIStatus(SPI_mem_read((uint8_t)(reg | ICM42688_SPI_READ_BIT), data, len));
}

static ICM42688_StatusTypeDef ICM42688_SelectBank(uint8_t bank)
{
    return ICM42688_WriteReg(ICM42688_REG_BANK_SEL, (uint8_t)(bank & 0x07));
}

static ICM42688_StatusTypeDef ICM42688_CheckID(void)
{
    uint8_t who_am_i;

    if (ICM42688_ReadWhoAmI(&who_am_i) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (who_am_i != ICM42688_WHO_AM_I_VALUE)
    {
        return ICM42688_ERROR;
    }

    return ICM42688_OK;
}

static ICM42688_StatusTypeDef ICM42688_Reset(void)
{
    if (ICM42688_WriteReg(ICM42688_REG_DEVICE_CONFIG, ICM42688_SOFT_RESET) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    delay_ms(2);
    return ICM42688_OK;
}

static ICM42688_StatusTypeDef ICM42688_SetFilterBandwidth(void)
{
    return ICM42688_WriteReg(ICM42688_REG_GYRO_ACCEL_CONFIG0, ICM42688_FILTER_BW_ODR_DIV_4);
}

static float ICM42688_GetAccelSensitivity(void)
{
    switch (accel_full_scale)
    {
        case ICM42688_ACCEL_FS_2G:
            return 16384.0f;
        case ICM42688_ACCEL_FS_4G:
            return 8192.0f;
        case ICM42688_ACCEL_FS_8G:
            return 4096.0f;
        case ICM42688_ACCEL_FS_16G:
        default:
            return 2048.0f;
    }
}

static float ICM42688_GetGyroSensitivity(void)
{
    switch (gyro_full_scale)
    {
        case ICM42688_GYRO_FS_15_625DPS:
            return 2097.152f;
        case ICM42688_GYRO_FS_31_25DPS:
            return 1048.576f;
        case ICM42688_GYRO_FS_62_5DPS:
            return 524.288f;
        case ICM42688_GYRO_FS_125DPS:
            return 262.144f;
        case ICM42688_GYRO_FS_250DPS:
            return 131.072f;
        case ICM42688_GYRO_FS_500DPS:
            return 65.536f;
        case ICM42688_GYRO_FS_1000DPS:
            return 32.768f;
        case ICM42688_GYRO_FS_2000DPS:
        default:
            return 16.384f;
    }
}

ICM42688_StatusTypeDef ICM42688_Init(void)
{
    SPI_init();
    delay_ms(10);

    if (ICM42688_SelectBank(ICM42688_BANK0) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_CheckID() != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_Reset() != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    delay_ms(10);

    if (ICM42688_SelectBank(ICM42688_BANK0) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_CheckID() != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_SetGyroConfig(ICM42688_GYRO_FS_2000DPS, ICM42688_ODR_1KHZ) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_SetAccelConfig(ICM42688_ACCEL_FS_16G, ICM42688_ODR_1KHZ) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_SetFilterBandwidth() != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_WriteReg(ICM42688_REG_PWR_MGMT0, ICM42688_PWR_ACCEL_GYRO_LN) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    delay_ms(50);
    return ICM42688_OK;
}

ICM42688_StatusTypeDef ICM42688_ReadWhoAmI(uint8_t *who_am_i)
{
    if (who_am_i == 0)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_SelectBank(ICM42688_BANK0) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_ReadReg(ICM42688_REG_WHO_AM_I, who_am_i) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    debug_icm42688_who_am_i = *who_am_i;
    return ICM42688_OK;
}

ICM42688_StatusTypeDef ICM42688_SetAccelConfig(ICM42688_AccelFullScale_t full_scale, ICM42688_ODR_t odr)
{
    uint8_t config;

    if (full_scale > ICM42688_ACCEL_FS_2G)
    {
        return ICM42688_ERROR;
    }

    config = (uint8_t)(((uint8_t)full_scale << 5) | ((uint8_t)odr & 0x0F));

    if (ICM42688_WriteReg(ICM42688_REG_ACCEL_CONFIG0, config) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    accel_full_scale = full_scale;
    return ICM42688_OK;
}

ICM42688_StatusTypeDef ICM42688_SetGyroConfig(ICM42688_GyroFullScale_t full_scale, ICM42688_ODR_t odr)
{
    uint8_t config;

    if (full_scale > ICM42688_GYRO_FS_15_625DPS)
    {
        return ICM42688_ERROR;
    }

    config = (uint8_t)(((uint8_t)full_scale << 5) | ((uint8_t)odr & 0x0F));

    if (ICM42688_WriteReg(ICM42688_REG_GYRO_CONFIG0, config) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    gyro_full_scale = full_scale;
    return ICM42688_OK;
}

ICM42688_StatusTypeDef ICM42688_ReadRawData(ICM42688_RawData_t *raw_data)
{
    uint8_t raw[14];

    if (raw_data == 0)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_ReadRegs(ICM42688_REG_TEMP_DATA1, raw, 14) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    raw_data->temp = (int16_t)(((uint16_t)raw[0] << 8) | raw[1]);
    raw_data->accel_x = (int16_t)(((uint16_t)raw[2] << 8) | raw[3]);
    raw_data->accel_y = (int16_t)(((uint16_t)raw[4] << 8) | raw[5]);
    raw_data->accel_z = (int16_t)(((uint16_t)raw[6] << 8) | raw[7]);
    raw_data->gyro_x = (int16_t)(((uint16_t)raw[8] << 8) | raw[9]);
    raw_data->gyro_y = (int16_t)(((uint16_t)raw[10] << 8) | raw[11]);
    raw_data->gyro_z = (int16_t)(((uint16_t)raw[12] << 8) | raw[13]);

    debug_icm42688_temp = raw_data->temp;
    debug_icm42688_accel_x = raw_data->accel_x;
    debug_icm42688_accel_y = raw_data->accel_y;
    debug_icm42688_accel_z = raw_data->accel_z;
    debug_icm42688_gyro_x = raw_data->gyro_x;
    debug_icm42688_gyro_y = raw_data->gyro_y;
    debug_icm42688_gyro_z = raw_data->gyro_z;

    return ICM42688_OK;
}

ICM42688_StatusTypeDef ICM42688_ConvertRawToSensorData(const ICM42688_RawData_t *raw_data, ICM42688_Data_t *sensor_data)
{
    float accel_sensitivity;
    float gyro_sensitivity;

    if ((raw_data == 0) || (sensor_data == 0))
    {
        return ICM42688_ERROR;
    }

    accel_sensitivity = ICM42688_GetAccelSensitivity();
    gyro_sensitivity = ICM42688_GetGyroSensitivity();

    sensor_data->accel_x_g = (float)raw_data->accel_x / accel_sensitivity;
    sensor_data->accel_y_g = (float)raw_data->accel_y / accel_sensitivity;
    sensor_data->accel_z_g = (float)raw_data->accel_z / accel_sensitivity;
    sensor_data->gyro_x_dps = (float)raw_data->gyro_x / gyro_sensitivity;
    sensor_data->gyro_y_dps = (float)raw_data->gyro_y / gyro_sensitivity;
    sensor_data->gyro_z_dps = (float)raw_data->gyro_z / gyro_sensitivity;
    sensor_data->temperature_c = ((float)raw_data->temp / 132.48f) + 25.0f;

    return ICM42688_OK;
}

ICM42688_StatusTypeDef ICM42688_ReadSensorData(ICM42688_Data_t *sensor_data)
{
    ICM42688_RawData_t raw_data;

    if (sensor_data == 0)
    {
        return ICM42688_ERROR;
    }

    if (ICM42688_ReadRawData(&raw_data) != ICM42688_OK)
    {
        return ICM42688_ERROR;
    }

    return ICM42688_ConvertRawToSensorData(&raw_data, sensor_data);
}