#include <stdint.h>
#include <gpio.h>

#define GPIOA_ADDR 0x40020000
#define GPIOB_ADDR 0x40020400

volatile uint32_t *GPIOA_MODER   = (volatile uint32_t *)(GPIOA_ADDR + 0x00);
volatile uint32_t *GPIOA_OTYPER  = (volatile uint32_t *)(GPIOA_ADDR + 0x04);
volatile uint32_t *GPIOA_OSPEEDR = (volatile uint32_t *)(GPIOA_ADDR + 0x08);
volatile uint32_t *GPIOA_PUPDR   = (volatile uint32_t *)(GPIOA_ADDR + 0x0C);
volatile uint32_t *GPIOA_IDR     = (volatile uint32_t *)(GPIOA_ADDR + 0x10);
volatile uint32_t *GPIOA_ODR     = (volatile uint32_t *)(GPIOA_ADDR + 0x14);
volatile uint32_t *GPIOA_BSRR    = (volatile uint32_t *)(GPIOA_ADDR + 0x18);
volatile uint32_t *GPIOA_AFRL    = (volatile uint32_t *)(GPIOA_ADDR + 0x20);
volatile uint32_t *GPIOA_AFRH    = (volatile uint32_t *)(GPIOA_ADDR + 0x24);

volatile uint32_t *GPIOB_MODER   = (volatile uint32_t *)(GPIOB_ADDR + 0x00);
volatile uint32_t *GPIOB_OTYPER  = (volatile uint32_t *)(GPIOB_ADDR + 0x04);
volatile uint32_t *GPIOB_OSPEEDR = (volatile uint32_t *)(GPIOB_ADDR + 0x08);
volatile uint32_t *GPIOB_PUPDR   = (volatile uint32_t *)(GPIOB_ADDR + 0x0C);
volatile uint32_t *GPIOB_IDR     = (volatile uint32_t *)(GPIOB_ADDR + 0x10);
volatile uint32_t *GPIOB_ODR     = (volatile uint32_t *)(GPIOB_ADDR + 0x14);
volatile uint32_t *GPIOB_BSRR    = (volatile uint32_t *)(GPIOB_ADDR + 0x18);
volatile uint32_t *GPIOB_AFRL    = (volatile uint32_t *)(GPIOB_ADDR + 0x20);
volatile uint32_t *GPIOB_AFRH    = (volatile uint32_t *)(GPIOB_ADDR + 0x24);

void GPIOA_CONFIG(port_t Px, moder_t MODER, otyper_t OTYPER, ospeedr_t OSPEEDR, pupdr_t PUPDR, uint32_t ODR, af_t AFRL, af_t AFRH)
{
    *GPIOA_MODER    &=  ~(0b11  << Px *2);
    *GPIOA_OSPEEDR  &=  ~(0b11  << Px *2);
    *GPIOA_PUPDR    &=  ~(0b11  << Px *2);
    *GPIOA_OTYPER   &=  ~(0b1   << Px );
    *GPIOA_ODR      &=  ~(0b1   << Px );

    *GPIOA_MODER    |=  (MODER  << Px *2);
    *GPIOA_OTYPER   |=  (OTYPER << Px   );
    *GPIOA_OSPEEDR  |=  (OSPEEDR << Px *2);
    *GPIOA_PUPDR    |=  (PUPDR << Px *2);
    *GPIOA_ODR      |=  (ODR << Px);

    if (Px < 8)
    {
        *GPIOA_AFRL     &=  ~(0b1111 << Px *4);
        *GPIOA_AFRL     |=  (AFRL << Px *4);
    } else
    {
        *GPIOA_AFRH     &=  ~(0b1111 << (Px-8) *4 );
        *GPIOA_AFRH     |=  (AFRH << (Px-8) *4 );
    }
    
    
}

void GPIOB_CONFIG(port_t Px, moder_t MODER, otyper_t OTYPER, ospeedr_t OSPEEDR, pupdr_t PUPDR, uint32_t ODR, af_t AFRL, af_t AFRH)
{
    *GPIOB_MODER    &=  ~(0b11  << Px *2);
    *GPIOB_OSPEEDR  &=  ~(0b11  << Px *2);
    *GPIOB_PUPDR    &=  ~(0b11  << Px *2);
    *GPIOB_OTYPER   &=  ~(0b1   << Px );
    *GPIOB_ODR      &=  ~(0b1   << Px );

    *GPIOB_MODER    |=  (MODER  << Px *2);
    *GPIOB_OTYPER   |=  (OTYPER << Px   );
    *GPIOB_OSPEEDR  |=  (OSPEEDR << Px *2);
    *GPIOB_PUPDR    |=  (PUPDR << Px *2);
    *GPIOB_ODR      |=  (ODR << Px);

    if (Px < 8)
    {
        *GPIOB_AFRL     &=  ~(0b1111 << Px *4);
        *GPIOB_AFRL     |=  (AFRL << Px *4);
    } else
    {
        *GPIOB_AFRH     &=  ~(0b1111 << (Px-8) *4 );
        *GPIOB_AFRH     |=  (AFRH << (Px-8) *4 );
    }
    
    
}
