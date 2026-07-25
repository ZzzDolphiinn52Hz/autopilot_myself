#ifndef INC_SPI_H_
#define INC_SPI_H_

#include <stdint.h>

typedef enum
{
    SPI_OK = 0,
    SPI_ERROR,
    SPI_TIMEOUT
} SPI_Status_t;

void SPI_init(void);
SPI_Status_t SPI_transmit(const uint8_t *data, uint16_t len);
SPI_Status_t SPI_receive(uint8_t *data, uint16_t len);
SPI_Status_t SPI_transmit_receive(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len);
SPI_Status_t SPI_transfer_byte(uint8_t tx_data, uint8_t *rx_data);

#endif /* INC_SPI_H_ */