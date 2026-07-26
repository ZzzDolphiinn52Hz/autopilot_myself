#include "pwm.h"
#include "clock.h"

#define GPIOB_ADDR 0x40020400UL
#define TIM4_ADDR  0x40000800UL

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
    static volatile uint32_t* GPIOB_MODER   = (volatile uint32_t*)(GPIOB_ADDR + 0x00UL);
    static volatile uint32_t* GPIOB_OTYPER  = (volatile uint32_t*)(GPIOB_ADDR + 0x04UL);
    static volatile uint32_t* GPIOB_OSPEEDR = (volatile uint32_t*)(GPIOB_ADDR + 0x08UL);
    static volatile uint32_t* GPIOB_PUPDR   = (volatile uint32_t*)(GPIOB_ADDR + 0x0CUL);
    static volatile uint32_t* GPIOB_AFRL    = (volatile uint32_t*)(GPIOB_ADDR + 0x20UL);
    static volatile uint32_t* GPIOB_AFRH    = (volatile uint32_t*)(GPIOB_ADDR + 0x24UL);

    static volatile uint32_t* TIM4_CR1   = (volatile uint32_t*)(TIM4_ADDR + 0x00UL);
    static volatile uint32_t* TIM4_EGR   = (volatile uint32_t*)(TIM4_ADDR + 0x14UL);
    static volatile uint32_t* TIM4_CCMR1 = (volatile uint32_t*)(TIM4_ADDR + 0x18UL);
    static volatile uint32_t* TIM4_CCMR2 = (volatile uint32_t*)(TIM4_ADDR + 0x1CUL);
    static volatile uint32_t* TIM4_CCER  = (volatile uint32_t*)(TIM4_ADDR + 0x20UL);
    static volatile uint32_t* TIM4_PSC   = (volatile uint32_t*)(TIM4_ADDR + 0x28UL);
    static volatile uint32_t* TIM4_ARR   = (volatile uint32_t*)(TIM4_ADDR + 0x2CUL);
    static volatile uint32_t* TIM4_CCR1  = (volatile uint32_t*)(TIM4_ADDR + 0x34UL);
    static volatile uint32_t* TIM4_CCR2  = (volatile uint32_t*)(TIM4_ADDR + 0x38UL);
    static volatile uint32_t* TIM4_CCR3  = (volatile uint32_t*)(TIM4_ADDR + 0x3CUL);
    static volatile uint32_t* TIM4_CCR4  = (volatile uint32_t*)(TIM4_ADDR + 0x40UL);

    clock_enable_AHB1(GPIOB_peripheral);
    clock_enable_APB1(TIM4_peripheral);

    *TIM4_CR1 = 0; // tat timer

    // Cấu hình PB6, PB7, PB8, PB9 (Alternate Function mode)
    *GPIOB_MODER &= ~((0b11 << 12) | (0b11 << 14) | (0b11 << 16) | (0b11 << 18));
    *GPIOB_MODER |=  ((0b10 << 12) | (0b10 << 14) | (0b10 << 16) | (0b10 << 18));

    *GPIOB_OTYPER &= ~((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9)); // push - pull

    *GPIOB_OSPEEDR &= ~((0b11 << 12) | (0b11 << 14) | (0b11 << 16) | (0b11 << 18));
    *GPIOB_OSPEEDR |=  ((0b10 << 12) | (0b10 << 14) | (0b10 << 16) | (0b10 << 18)); // high speed

    *GPIOB_PUPDR &= ~((0b11 << 12) | (0b11 << 14) | (0b11 << 16) | (0b11 << 18)); // no pull-up, pull-down

    // AF2 cho TIM4 trên PB6, PB7 (thuộc AFRL)
    *GPIOB_AFRL &= ~((0xF << 24) | (0xF << 28));
    *GPIOB_AFRL |=  ((2 << 24) | (2 << 28));

    // AF2 cho TIM4 trên PB8, PB9 (thuộc AFRH)
    *GPIOB_AFRH &= ~((0xF << 0) | (0xF << 4));
    *GPIOB_AFRH |=  ((2 << 0) | (2 << 4));

    // Cấu hình TIM4
    *TIM4_PSC = 100 - 1; // 100MHz / 100 = 1MHz => 1cnt = 1us 
    *TIM4_ARR = 20000 - 1; // => 20000 cnt = 20ms

    *TIM4_CCR1 = PWM_MIN_US;
    *TIM4_CCR2 = PWM_MIN_US;
    *TIM4_CCR3 = PWM_MIN_US;
    *TIM4_CCR4 = PWM_MIN_US;

    // Mode 1 (PWM) - channel 1,2 ; preload cr1, cr2
    *TIM4_CCMR1 = (6 << 4) | (1 << 3) | (6 << 12) | (1 << 11); 

    // Mode 1 (PWM) - channel 3,4 ; preload cr3, cr4
    *TIM4_CCMR2 = (6 << 4) | (1 << 3) | (6 << 12) | (1 << 11);

    // Bật output cho CH1, CH2, CH3, CH4
    *TIM4_CCER = (1 << 0) | (1 << 4) | (1 << 8) | (1 << 12);

    *TIM4_CR1 |= (1 << 7); // ARPE
    *TIM4_EGR = 1;
    *TIM4_CR1 |= 1; // bật timer
}

void control_motor(uint8_t motor, uint16_t pulse_width)
{
    // Mapping: M1=PB6(CH1), M2=PB7(CH2), M3=PB8(CH3), M4=PB9(CH4)
    volatile uint32_t* TIM4_CCR[4] =
    {
        (volatile uint32_t*)(TIM4_ADDR + 0x34), // CCR1
        (volatile uint32_t*)(TIM4_ADDR + 0x38), // CCR2
        (volatile uint32_t*)(TIM4_ADDR + 0x3C), // CCR3
        (volatile uint32_t*)(TIM4_ADDR + 0x40)  // CCR4
    };

    if ((motor < 1) || (motor > 4))
    {
        return;
    }

    *TIM4_CCR[motor - 1] = pwm_clamp_pulse(pulse_width);
}
