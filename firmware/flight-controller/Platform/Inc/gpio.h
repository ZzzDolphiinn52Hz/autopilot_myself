#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include <stdint.h>

typedef enum
{
    GPIO_INPUT = 0b00,
    GPIO_OUTPUT = 0b01,
    GPIO_ALTERNATE = 0b10
} moder_t ;

typedef enum
{
    GPIO_PUSHPULL = 0b0,
    GPIO_OPENDRAIN = 0b1
} otyper_t;

typedef enum
{
    GPIO_LOW = 0b00,
    GPIO_MED = 0b01, 
    GPIO_FAST= 0b10,
    GPIO_HIGH = 0b11
} ospeedr_t;

typedef enum
{
    GPIO_NOPULL = 0b00,
    GPIO_PULLUP = 0b01,
    GPIO_PULLDOWN = 0b10
} pupdr_t;

typedef enum
{
    GPIO_AF0 = 0b0000,
    GPIO_AF1 = 0b0001,
    GPIO_AF2 = 0b0010,
    GPIO_AF3 = 0b0011,
    GPIO_AF4 = 0b0100,
    GPIO_AF5 = 0b0101,
    GPIO_AF6 = 0b0110,
    GPIO_AF7 = 0b0111,
    GPIO_AF8 = 0b1000,
    GPIO_AF9 = 0b1001,
    GPIO_AF10 = 0b1010,
    GPIO_AF11 = 0b1011,
    GPIO_AF12 = 0b1100,
    GPIO_AF13 = 0b1101,
    GPIO_AF14 = 0b1110,
    GPIO_AF15 = 0b1111
} af_t;

typedef enum
{
    P0 =0,
    P1,
    P2,
    P3,
    P4,
    P5,
    P6,
    P7,
    P8,
    P9,
    P10,
    P11,
    P12,
    P13,
    P14,
    P15,
} port_t;

void GPIOA_CONFIG(port_t Px, moder_t MODER, otyper_t OTYPER, ospeedr_t OSPEEDR, pupdr_t PUPDR, uint32_t ODR, af_t AFRL, af_t AFRH);
void GPIOB_CONFIG(port_t Px, moder_t MODER, otyper_t OTYPER, ospeedr_t OSPEEDR, pupdr_t PUPDR, uint32_t ODR, af_t AFRL, af_t AFRH);

#endif /* INC_GPIO_H_ */
