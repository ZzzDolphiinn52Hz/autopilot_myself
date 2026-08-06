#include "uart6.h"
#include "clock.h"
#include <gpio.h>

#define USART6_ADDR     0x40011400

static volatile uint32_t *USART6_SR     = (volatile uint32_t *)(USART6_ADDR + 0x00);
static volatile uint32_t *USART6_DR     = (volatile uint32_t *)(USART6_ADDR + 0x04);
static volatile uint32_t *USART6_BRR    = (volatile uint32_t *)(USART6_ADDR + 0x08);
static volatile uint32_t *USART6_CR1    = (volatile uint32_t *)(USART6_ADDR + 0x0C);
static volatile uint32_t *USART6_CR2    = (volatile uint32_t *)(USART6_ADDR + 0x10);
static volatile uint32_t *USART6_CR3    = (volatile uint32_t *)(USART6_ADDR + 0x14);

#define USART_SR_TXE    (1 << 7)
#define USART_SR_TC     (1 << 6)
#define USART_SR_RXNE   (1 << 5)

#define USART_CR1_UE    (1 << 13)
#define USART_CR1_TE    (1 << 3)
#define USART_CR1_RE    (1 << 2)

static uint32_t uart6_calculate_brr(uint32_t pclk, uint32_t baudrate)
{
    return (pclk + (baudrate / 2U)) / baudrate;
}

void uart6_init(void)
{
    clock_enable_AHB1(GPIOA_peripheral);
    clock_enable_APB2(USART6_peripheral);

    GPIOA_CONFIG(P11, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_NOPULL, 0, 0, GPIO_AF8); 
    GPIOA_CONFIG(P12, GPIO_ALTERNATE, GPIO_PUSHPULL, GPIO_FAST, GPIO_PULLUP, 0, 0, GPIO_AF8); 

    *USART6_CR1 &= ~USART_CR1_UE;
    *USART6_CR2 = 0;
    *USART6_CR3 = 0;
    *USART6_BRR = uart6_calculate_brr(RCC_SYS_CLOCK_HZ, UART6_BAUDRATE); // USART6 is on APB2 (100MHz)
    *USART6_CR1 = USART_CR1_TE | USART_CR1_RE;
    *USART6_CR1 |= USART_CR1_UE;
}

void uart6_send_char(char data)
{
    while ((*USART6_SR & USART_SR_TXE) == 0);
    *USART6_DR = (uint8_t)data;
} // send 1 ky tu

int uart6_read_char(char *data)
{
    if (data == 0)
    {
        return 0;
    }

    if ((*USART6_SR & USART_SR_RXNE) == 0)
    {
        return 0;
    }

    *data = (char)(*USART6_DR & 0xFF);
    return 1;
}

void uart6_send_string(const char *str)
{
    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        uart6_send_char(*str);
        str++;
    }
} // send 1 chuoi => vong lap send 1 ky tu

void uart6_send_uint(uint32_t value)
{
    char buffer[10];
    uint8_t index = 0;

    if (value == 0)
    {
        uart6_send_char('0');
        return;
    }

    while (value > 0)
    {
        buffer[index++] = (char)('0' + (value % 10));
        value /= 10;
    }

    while (index > 0)
    {
        uart6_send_char(buffer[--index]);
    }
} // send 1 so uint nen tach tung chu so rồi gửi

void uart6_send_int(int32_t value)
{
    if (value < 0)
    {
        uart6_send_char('-');
        value = -value;
    }

    uart6_send_uint((uint32_t)value);
} // send 1 so nguyen co the am hoac duong

void uart6_send_float(float value, uint8_t decimal)
{
    uint32_t multiplier = 1;
    uint32_t fraction;

    if (value < 0.0f)
    {
        uart6_send_char('-');
        value = -value;
    }

    for (uint8_t i = 0; i < decimal; i++)
    {
        multiplier *= 10;
    }

    uart6_send_uint((uint32_t)value);

    if (decimal == 0)
    {
        return;
    }

    uart6_send_char('.');
    fraction = (uint32_t)((value - (float)((uint32_t)value)) * (float)multiplier + 0.5);

    if (fraction >= multiplier)
    {
        fraction = multiplier - 1;
    }

    for (uint32_t div = multiplier / 10; div > 0; div /= 10)
    {
        uart6_send_char((char)('0' + ((fraction / div) % 10)));
    }
}
