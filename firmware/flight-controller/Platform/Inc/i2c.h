#ifndef INC_SPI_H_
#define INC_SPI_H_

#include <stdint.h>


extern volatile uint32_t *GPIOA_MODER   ;
extern volatile uint32_t *GPIOA_OTYPER  ;
extern volatile uint32_t *GPIOA_OSPEEDR ;
extern volatile uint32_t *GPIOA_PUPDR   ;
extern volatile uint32_t *GPIOA_BSRR    ;
extern volatile uint32_t *GPIOA_AFRL   ;
extern volatile uint32_t *GPIOA_AFRH   ;

extern volatile uint32_t *SPI1_CR1     ;
extern volatile uint32_t *SPI1_CR2     ;
extern volatile uint32_t *SPI1_SR       ;
extern volatile uint32_t *SPI1_DR      ;
extern volatile uint32_t *SPI1_CRCPR   ;
extern volatile uint32_t *SPI1_RXCRCR  ;
extern volatile uint32_t *SPI1_TXCRCR  ;
typedef enum
{
    SPI_OK = 0,
    SPI_ERROR,
    SPI_TIMEOUT
} SPI_Status_t;

void i2c_init(void);
I2C_Status_t i2c_mem_write(uint8_t dev_addr, uint8_t mem_addr, const uint8_t *data, uint16_t len);
I2C_Status_t i2c_mem_read(uint8_t dev_addr, uint8_t mem_addr, uint8_t *data, uint16_t len);

#endif /* INC_SPI_H_ */
