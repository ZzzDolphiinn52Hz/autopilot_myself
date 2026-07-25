#ifndef INC_ICM42688_H_
#define INC_ICM42688_H_

#include <stdint.h>

#define ICM42688_REG_DEVICE_CONFIG       0x11
#define ICM42688_REG_TEMP_DATA1          0x1D
#define ICM42688_REG_ACCEL_DATA_X1       0x1F
#define ICM42688_REG_GYRO_DATA_X1        0x25
#define ICM42688_REG_SIGNAL_PATH_RESET   0x4B
#define ICM42688_REG_PWR_MGMT0           0x4E
#define ICM42688_REG_GYRO_CONFIG0        0x4F
#define ICM42688_REG_ACCEL_CONFIG0       0x50
#define ICM42688_REG_GYRO_ACCEL_CONFIG0  0x52
#define ICM42688_REG_WHO_AM_I            0x75
#define ICM42688_REG_BANK_SEL            0x76

#define ICM42688_WHO_AM_I_VALUE          0x47
#define ICM42688_SPI_READ_BIT            0x80
#define ICM42688_SPI_WRITE_BIT           0x00

typedef enum
{
    ICM42688_OK = 0,
    ICM42688_ERROR,
    ICM42688_BUSY,
    ICM42688_TIMEOUT
} ICM42688_StatusTypeDef;

typedef enum
{
    ICM42688_ACCEL_FS_16G = 0,
    ICM42688_ACCEL_FS_8G  = 1,
    ICM42688_ACCEL_FS_4G  = 2,
    ICM42688_ACCEL_FS_2G  = 3
} ICM42688_AccelFullScale_t;

typedef enum
{
    ICM42688_GYRO_FS_2000DPS   = 0,
    ICM42688_GYRO_FS_1000DPS   = 1,
    ICM42688_GYRO_FS_500DPS    = 2,
    ICM42688_GYRO_FS_250DPS    = 3,
    ICM42688_GYRO_FS_125DPS    = 4,
    ICM42688_GYRO_FS_62_5DPS   = 5,
    ICM42688_GYRO_FS_31_25DPS  = 6,
    ICM42688_GYRO_FS_15_625DPS = 7
} ICM42688_GyroFullScale_t;

typedef enum
{
    ICM42688_ODR_32KHZ    = 0x01,
    ICM42688_ODR_16KHZ    = 0x02,
    ICM42688_ODR_8KHZ     = 0x03,
    ICM42688_ODR_4KHZ     = 0x04,
    ICM42688_ODR_2KHZ     = 0x05,
    ICM42688_ODR_1KHZ     = 0x06,
    ICM42688_ODR_200HZ    = 0x07,
    ICM42688_ODR_100HZ    = 0x08,
    ICM42688_ODR_50HZ     = 0x09,
    ICM42688_ODR_25HZ     = 0x0A,
    ICM42688_ODR_12_5HZ   = 0x0B,
    ICM42688_ODR_6_25HZ   = 0x0C,
    ICM42688_ODR_3_125HZ  = 0x0D,
    ICM42688_ODR_1_5625HZ = 0x0E,
    ICM42688_ODR_500HZ    = 0x0F
} ICM42688_ODR_t;

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t temp;
} ICM42688_RawData_t;

typedef struct
{
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;
    float gyro_x_dps;
    float gyro_y_dps;
    float gyro_z_dps;
    float temperature_c;
} ICM42688_Data_t;

ICM42688_StatusTypeDef ICM42688_Init(void);
ICM42688_StatusTypeDef ICM42688_ReadWhoAmI(uint8_t *who_am_i);
ICM42688_StatusTypeDef ICM42688_SetAccelConfig(ICM42688_AccelFullScale_t full_scale, ICM42688_ODR_t odr);
ICM42688_StatusTypeDef ICM42688_SetGyroConfig(ICM42688_GyroFullScale_t full_scale, ICM42688_ODR_t odr);
ICM42688_StatusTypeDef ICM42688_ReadRawData(ICM42688_RawData_t *raw_data);
ICM42688_StatusTypeDef ICM42688_ConvertRawToSensorData(const ICM42688_RawData_t *raw_data, ICM42688_Data_t *sensor_data);
ICM42688_StatusTypeDef ICM42688_ReadSensorData(ICM42688_Data_t *sensor_data);

#endif /* INC_ICM42688_H_ */