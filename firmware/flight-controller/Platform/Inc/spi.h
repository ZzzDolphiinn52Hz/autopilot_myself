#ifndef INC_SPI_H_
#define INC_SPI_H_

#include <stdint.h>


#define GPIOA_ADDR      0x40020000
#define SPI1_ADDR       0x40013000

#define GPIOA_MODER     ((volatile uint32_t *)(GPIOA_ADDR + 0x00))
#define GPIOA_OTYPER    ((volatile uint32_t *)(GPIOA_ADDR + 0x04))
#define GPIOA_OSPEEDR   ((volatile uint32_t *)(GPIOA_ADDR + 0x08))
#define GPIOA_PUPDR     ((volatile uint32_t *)(GPIOA_ADDR + 0x0C))
#define GPIOA_BSRR      ((volatile uint32_t *)(GPIOA_ADDR + 0x18))
#define GPIOA_AFRL      ((volatile uint32_t *)(GPIOA_ADDR + 0x20))
#define GPIOA_AFRH      ((volatile uint32_t *)(GPIOA_ADDR + 0x24))

#define SPI1_CR1        ((volatile uint32_t *)(SPI1_ADDR + 0x00))
#define SPI1_CR2        ((volatile uint32_t *)(SPI1_ADDR + 0x04))
#define SPI1_SR         ((volatile uint32_t *)(SPI1_ADDR + 0x08))
#define SPI1_DR         ((volatile uint32_t *)(SPI1_ADDR + 0x0C))
#define SPI1_CRCPR      ((volatile uint32_t *)(SPI1_ADDR + 0x10))
#define SPI1_RXCRCR     ((volatile uint32_t *)(SPI1_ADDR + 0x14))
#define SPI1_TXCRCR     ((volatile uint32_t *)(SPI1_ADDR + 0x18))
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
