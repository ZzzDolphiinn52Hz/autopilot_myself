#include "pwm.h"
#include "clock.h"

#define GPIOA_ADDR 0x40020000UL
#define GPIOB_ADDR 0x40020400UL
#define TIM3_ADDR  0x40000400UL

uint16_t pwm_clamp_pulse(uint16_t pulse_width)
{
    if (pulse_width < PWM_MIN_US)
    {
        return PWM_MIN_US;
    }

    if (pulse_width > PWM_MAX_US)
    {
        return PWM_MAX_US;
    }

    return pulse_width;
}

void pwm_init(void)
{
    volatile uint32_t* GPIOA_MODER   = (volatile uint32_t*)(GPIOA_ADDR + 0x00UL);
    volatile uint32_t* GPIOA_OTYPER  = (volatile uint32_t*)(GPIOA_ADDR + 0x04UL);
    volatile uint32_t* GPIOA_OSPEEDR = (volatile uint32_t*)(GPIOA_ADDR + 0x08UL);
    volatile uint32_t* GPIOA_PUPDR   = (volatile uint32_t*)(GPIOA_ADDR + 0x0CUL);
    volatile uint32_t* GPIOA_AFRL    = (volatile uint32_t*)(GPIOA_ADDR + 0x20UL);

    volatile uint32_t* GPIOB_MODER   = (volatile uint32_t*)(GPIOB_ADDR + 0x00UL);
    volatile uint32_t* GPIOB_OTYPER  = (volatile uint32_t*)(GPIOB_ADDR + 0x04UL);
    volatile uint32_t* GPIOB_OSPEEDR = (volatile uint32_t*)(GPIOB_ADDR + 0x08UL);
    volatile uint32_t* GPIOB_PUPDR   = (volatile uint32_t*)(GPIOB_ADDR + 0x0CUL);
    volatile uint32_t* GPIOB_AFRL    = (volatile uint32_t*)(GPIOB_ADDR + 0x20UL);

    volatile uint32_t* TIM3_CR1   = (volatile uint32_t*)(TIM3_ADDR + 0x00UL);
    volatile uint32_t* TIM3_EGR   = (volatile uint32_t*)(TIM3_ADDR + 0x14UL);
    volatile uint32_t* TIM3_CCMR1 = (volatile uint32_t*)(TIM3_ADDR + 0x18UL);
    volatile uint32_t* TIM3_CCMR2 = (volatile uint32_t*)(TIM3_ADDR + 0x1CUL);
    volatile uint32_t* TIM3_CCER  = (volatile uint32_t*)(TIM3_ADDR + 0x20UL);
    volatile uint32_t* TIM3_PSC   = (volatile uint32_t*)(TIM3_ADDR + 0x28UL);
    volatile uint32_t* TIM3_ARR   = (volatile uint32_t*)(TIM3_ADDR + 0x2CUL);
    volatile uint32_t* TIM3_CCR1  = (volatile uint32_t*)(TIM3_ADDR + 0x34UL);
    volatile uint32_t* TIM3_CCR2  = (volatile uint32_t*)(TIM3_ADDR + 0x38UL);
    volatile uint32_t* TIM3_CCR3  = (volatile uint32_t*)(TIM3_ADDR + 0x3CUL);
    volatile uint32_t* TIM3_CCR4  = (volatile uint32_t*)(TIM3_ADDR + 0x40UL);

    clock_enable_AHB1(GPIOA_peripheral);
    clock_enable_AHB1(GPIOB_peripheral);
    clock_enable_APB1(TIM3_peripheral);

    *TIM3_CR1 = 0; // tat timer

    // Cấu hình PA6 (CH1), PA7 (CH2)
    *GPIOA_MODER &= ~((0b11 << 12) | (0b11 << 14));
    *GPIOA_MODER |=  ((0b10 << 12) | (0b10 << 14)); // alternate function mode

    *GPIOA_OTYPER &= ~((1 << 6) | (1 << 7)); // push - pull

    *GPIOA_OSPEEDR &= ~((0b11 << 12) | (0b11 << 14));
    *GPIOA_OSPEEDR |=  ((0b10 << 12) | (0b10 << 14)); // high speed

    *GPIOA_PUPDR &= ~((0b11 << 12) | (0b11 << 14)); // no pull-up, pull-down

    *GPIOA_AFRL &= ~((0xF << 24) | (0xF << 28));
    *GPIOA_AFRL |=  ((2 << 24) | (2 << 28)); // AF2 cho TIM3

    // Cấu hình PB0 (CH3), PB1 (CH4)
    *GPIOB_MODER &= ~((0b11 << 0) | (0b11 << 2));
    *GPIOB_MODER |=  ((0b10 << 0) | (0b10 << 2)); // alternate function mode

    *GPIOB_OTYPER &= ~((1 << 0) | (1 << 1)); // push - pull

    *GPIOB_OSPEEDR &= ~((0b11 << 0) | (0b11 << 2));
    *GPIOB_OSPEEDR |=  ((0b10 << 0) | (0b10 << 2)); // high speed

    *GPIOB_PUPDR &= ~((0b11 << 0) | (0b11 << 2)); // no pull-up, pull-down

    *GPIOB_AFRL &= ~((0xF << 0) | (0xF << 4));
    *GPIOB_AFRL |=  ((2 << 0) | (2 << 4)); // AF2 cho TIM3

    *TIM3_PSC = 100 - 1; // 100MHz / 100 = 1MHz => 1cnt = 1us 
    *TIM3_ARR = 20000 - 1; // => 20000 cnt = 20ms

    *TIM3_CCR1 = PWM_MIN_US;
    *TIM3_CCR2 = PWM_MIN_US;
    *TIM3_CCR3 = PWM_MIN_US;
    *TIM3_CCR4 = PWM_MIN_US;

    *TIM3_CCMR1 = (6 << 4) | (1 << 3) | (6 << 12) | (1 << 11); 
    // Mode 1 - channel 1,2 ; preload cr1, cr2

    *TIM3_CCMR2 = (6 << 4) | (1 << 3) | (6 << 12) | (1 << 11);
    // Mode 1 - channel 3,4 ; preload cr3, cr4

    *TIM3_CCER = (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);

    *TIM3_CR1 |= (1 << 7);
    *TIM3_EGR = 1;
    *TIM3_CR1 |= 1; // bat timer
}

void control_motor(uint8_t motor, uint16_t pulse_width)
{
    volatile uint32_t* TIM3_CCR[4] =
    {
        (volatile uint32_t*)(TIM3_ADDR + 0x34),
        (volatile uint32_t*)(TIM3_ADDR + 0x38),
        (volatile uint32_t*)(TIM3_ADDR + 0x3C),
        (volatile uint32_t*)(TIM3_ADDR + 0x40)
    };

    if ((motor < 1) || (motor > 4))
    {
        return;
    }

    *TIM3_CCR[motor - 1] = pwm_clamp_pulse(pulse_width);
}
